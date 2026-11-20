#!/usr/bin/env bash
#
# Merges an upstream branch into the current branch.
#
# If the full merge conflicts, this script finds the longest prefix of the
# upstream first-parent history that merges cleanly, merges it, and reports
# the commit right after it, which conflicts. Whoever resolves the conflict
# then has to merge only that one commit by hand. A conflict that a later
# upstream commit cancels (for example a backout) is inside the clean prefix,
# so it never needs a manual merge.
#
# <upstream-remote> is a URL or the name of a configured remote.
#
# Every merge creates a merge commit, even where a fast-forward is possible.
#
# Writes these step outputs to $GITHUB_OUTPUT:
#   conflict         true if a conflicting upstream commit was left unmerged
#   conflict_commit  full SHA of that commit (only when conflict=true)

set -euo pipefail

usage="usage: $0 <upstream-remote> <upstream-branch> <merge-message>"
UPSTREAM_REMOTE=${1:?${usage}}
UPSTREAM_BRANCH=${2:?${usage}}
MERGE_MESSAGE=${3:?${usage}}

GITHUB_OUTPUT=${GITHUB_OUTPUT:-/dev/null}
GITHUB_STEP_SUMMARY=${GITHUB_STEP_SUMMARY:-/dev/null}

LOCAL_BRANCH=$(git branch --show-current)

git fetch "${UPSTREAM_REMOTE}" "${UPSTREAM_BRANCH}"
UPSTREAM_HEAD=$(git rev-parse FETCH_HEAD)

if git merge-base --is-ancestor "${UPSTREAM_HEAD}" HEAD; then
  echo "${LOCAL_BRANCH} already contains ${UPSTREAM_BRANCH} (${UPSTREAM_HEAD})"
  echo "conflict=false" >> "${GITHUB_OUTPUT}"
  exit 0
fi

# Succeeds if merging $1 into HEAD has no conflicts, and sets merge_result to
# the merged tree. Does not touch the index or work tree.
merges_cleanly() {
  local status=0
  merge_result=$(git merge-tree --write-tree --no-messages HEAD "$1") || status=$?
  case ${status} in
    0) return 0 ;;
    1) return 1 ;;
    *) echo "::error::git merge-tree failed with status ${status} for $1"; exit "${status}" ;;
  esac
}

# Commits merge_result as the merge of $1 into HEAD. Call it right after
# merges_cleanly "$1" succeeds.
commit_merge() {
  local commit
  commit=$(git commit-tree -p HEAD -p "$1" -m "${MERGE_MESSAGE}" "${merge_result}")
  git merge --ff-only --quiet "${commit}"
  echo "Merged $1 into ${LOCAL_BRANCH} as ${commit}"
}

if merges_cleanly "${UPSTREAM_HEAD}"; then
  commit_merge "${UPSTREAM_HEAD}"
  echo "conflict=false" >> "${GITHUB_OUTPUT}"
  exit 0
fi

# Upstream commits not yet merged, oldest first. --first-parent keeps merge
# commits from upstream stabilization branches as single steps.
mapfile -t commits < <(git rev-list --first-parent --reverse "HEAD..${UPSTREAM_HEAD}")
total=${#commits[@]}

# Find the longest prefix of commits that merges cleanly by scanning back from
# the newest commit. A binary search would be faster but can stop at an earlier
# conflict that a later commit cancels. The whole list conflicts, so the scan
# starts one commit shorter. commits[merged] is the commit right after the
# prefix, which conflicts.
merged=$(( total - 1 ))
while (( merged > 0 )) && ! merges_cleanly "${commits[merged - 1]}"; do
  merged=$(( merged - 1 ))
done
if (( merged > 0 )); then
  echo "Merging ${merged} of ${total} upstream commits"
  commit_merge "${commits[merged - 1]}"
fi

conflict_commit=${commits[merged]}
conflict_subject=$(git log -1 --format=%s "${conflict_commit}")
remaining=$(( total - merged - 1 ))
# Link to the commit on the upstream host, resolving a remote name to its URL.
upstream_url=$(git remote get-url "${UPSTREAM_REMOTE}" 2> /dev/null || echo "${UPSTREAM_REMOTE}")
conflict_url="${upstream_url%.git}/commit/${conflict_commit}"
# Some conflicts, such as a directory rename split, have no conflicted file,
# so report the CONFLICT messages.
conflicts=$(git merge-tree --write-tree HEAD "${conflict_commit}" | grep '^CONFLICT' || true)
resolve_branch="merge-upstream-${conflict_commit:0:12}"
# GITHUB_SERVER_URL and GITHUB_REPOSITORY are set by GitHub Actions.
compare_url="${GITHUB_SERVER_URL:-https://github.com}/${GITHUB_REPOSITORY:-<owner>/<repo>}/compare/${LOCAL_BRANCH}...${resolve_branch}?expand=1"

printf 'conflict=true\nconflict_commit=%s\n' "${conflict_commit}" >> "${GITHUB_OUTPUT}"

echo "::error title=Merge conflict with ${UPSTREAM_BRANCH}::Upstream commit ${conflict_commit} (${conflict_subject}) conflicts with ${LOCAL_BRANCH}. See the job summary for how to resolve it."

report=$(cat <<EOF
## Upstream merge conflict

Merged ${merged} of ${total} new commits from \`${UPSTREAM_REMOTE} ${UPSTREAM_BRANCH}\` into \`${LOCAL_BRANCH}\`.
The next commit conflicts and has to be merged by hand:

[\`${conflict_commit:0:12}\`](${conflict_url}) ${conflict_subject}

\`\`\`
${conflicts}
\`\`\`

To resolve, merge only that commit on a new branch:

\`\`\`sh
git fetch origin ${LOCAL_BRANCH}
git checkout -b ${resolve_branch} origin/${LOCAL_BRANCH}
git fetch ${UPSTREAM_REMOTE} ${UPSTREAM_BRANCH}
git merge -m ${MERGE_MESSAGE@Q} ${conflict_commit}
# Fix the conflicts, then:
git add <files>
git commit --no-edit
git push origin ${resolve_branch}
\`\`\`

Then [open a pull request](${compare_url}) into \`${LOCAL_BRANCH}\` for review.
Merge it with **Create a merge commit**. Squash or rebase would drop the
upstream commit from the history of \`${LOCAL_BRANCH}\`, so later runs would
treat it as unmerged.
EOF
)
if (( remaining > 0 )); then
  report+=$'\n\n'"After the pull request is merged, re-run this workflow to merge the remaining ${remaining} upstream commits."
  # FETCH_HEAD, because fetching from a URL does not create a branch ref.
  report+=$(cat <<EOF


To merge all ${UPSTREAM_BRANCH} commits at once instead, merge the upstream head
on the same branch and open the pull request the same way:

\`\`\`sh
git fetch origin ${LOCAL_BRANCH}
git checkout -b ${resolve_branch} origin/${LOCAL_BRANCH}
git fetch ${UPSTREAM_REMOTE} ${UPSTREAM_BRANCH}
git merge -m ${MERGE_MESSAGE@Q} FETCH_HEAD
# Fix the conflicts, then:
git add <files>
git commit --no-edit
git push origin ${resolve_branch}
\`\`\`
EOF
)
fi

tee -a "${GITHUB_STEP_SUMMARY}" <<< "${report}"
