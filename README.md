## Corretto 11

Amazon Corretto is a no-cost, multiplatform,
production-ready distribution of the Open Java Development Kit (OpenJDK).
Corretto is used internally at Amazon for production services.
With Corretto, you can develop and run Java applications 
on operating systems such as Amazon Linux 2, Windows, and macOS.

The latest binary Corretto 11 release builds can be downloaded from [https://github.com/corretto/corretto-11/releases](https://github.com/corretto/corretto-11/releases).

Documentation is available at [https://docs.aws.amazon.com/corretto](https://docs.aws.amazon.com/corretto).

### Licenses and Trademarks

Please read these files: "LICENSE", "ADDITIONAL_LICENSE_INFO", "ASSEMBLY_EXCEPTION", "TRADEMARKS.md".

### Branches

_develop_
: The default branch. It absorbs active development contributions from forks or topic branches via pull requests that pass smoke testing and are accepted.

_master_
: The stable branch. Starting point for the release process. It absorbs contributions from the develop branch that pass more thorough testing and are selected for releasing.

_ga-release_
: The source code of the GA release on 03/19/2019.

_release-11.0.XX.YY.Z_
: The source code for each release is recorded by a branch or a tag with a name of this form. XX stands for the OpenJDK 11 update number, YY for the OpenJDK 11 build number, and Z for the Corretto-specific revision number. The latter starts at 1 and is incremented in subsequent releases as long as the update and build numbers remain constant.
