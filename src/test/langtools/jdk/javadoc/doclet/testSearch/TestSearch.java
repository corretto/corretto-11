/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

/*
 * @test
 * @bug 8141492 8071982 8141636 8147890 8166175 8168965 8176794 8175218 8147881
 *      8181622 8182263 8074407 8187521 8198522 8182765 8199278 8196201 8196202
 *      8214468
 * @summary Test the search feature of javadoc.
 * @author bpatel
 * @library ../lib
 * @modules jdk.javadoc/jdk.javadoc.internal.tool
 * @build JavadocTester
 * @run main TestSearch
 */
public class TestSearch extends JavadocTester {

    public static void main(String... args) throws Exception {
        TestSearch tester = new TestSearch();
        tester.runTests();
    }

    @Test
    void test1() {
        javadoc("-d", "out-1",
                "-sourcepath",
                "-use",
                testSrc("UnnamedPkgClass.java"));
        checkExit(Exit.OK);
        checkSearchOutput("UnnamedPkgClass.html", true, true);
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(false,
                "tag-search-index.zip",
                "tag-search-index.js");
        checkFiles(true,
                "package-search-index.zip",
                "member-search-index.zip",
                "type-search-index.zip",
                "package-search-index.js",
                "member-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test2() {
        javadoc("-d", "out-2",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkInvalidUsageIndexTag();
        checkSearchOutput(true);
        checkSingleIndex(true, true);
        checkSingleIndexSearchTagDuplication();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkAllPkgsAllClasses();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test2_html4() {
        javadoc("-d", "out-2-html4",
                "-html4",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSingleIndex(true, false);
    }

    @Test
    void test2a() {
        javadoc("-d", "out-2a",
                "-Xdoclint:all",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.ERROR);
        checkDocLintErrors();
        checkSearchOutput(true);
        checkSingleIndex(true, true);
        checkSingleIndexSearchTagDuplication();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test2a_html4() {
        javadoc("-d", "out-2a-html4",
                "-html4",
                "-Xdoclint:all",
                "-sourcepath", testSrc,
                "-use",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkSingleIndex(true, false);
    }

    @Test
    void test3() {
        javadoc("-d", "out-3",
                "-noindex",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(false);
        checkJqueryAndImageFiles(false);
        checkFiles(false,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js",
                "index-all.html",
                "allpackages-index.html",
                "allclasses-index.html");
    }

    @Test
    void test4() {
        javadoc("-d", "out-4",
                "-html5",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(true);
        checkSingleIndex(true, true);
        checkSingleIndexSearchTagDuplication();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test5() {
        javadoc("-d", "out-5",
                "-html5",
                "-noindex",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(false);
        checkJqueryAndImageFiles(false);
        checkFiles(false,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js",
                "index-all.html");
    }

    @Test
    void test6() {
        javadoc("-d", "out-6",
                "-nocomment",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(true);
        checkIndexNoComment();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test7() {
        javadoc("-d", "out-7",
                "-nodeprecated",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");

        checkExit(Exit.OK);
        checkSearchOutput(true);
        checkIndexNoDeprecated();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test8() {
        javadoc("-d", "out-8",
                "-splitindex",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkInvalidUsageIndexTag();
        checkSearchOutput(true);
        checkSplitIndex();
        checkSplitIndexSearchTagDuplication();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "tag-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "tag-search-index.js",
                "type-search-index.js");
    }

    @Test
    void test9() {
        javadoc("-d", "out-9",
                "-sourcepath", testSrc,
                "-javafx",
                "--disable-javafx-strict-checks",
                "-package",
                "-use",
                "--frames",
                "pkgfx", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(true);
        checkJavaFXOutput();
        checkJqueryAndImageFiles(true);
        checkSearchJS();
        checkFiles(false,
                "tag-search-index.zip",
                "tag-search-index.js");
        checkFiles(true,
                "member-search-index.zip",
                "package-search-index.zip",
                "type-search-index.zip",
                "member-search-index.js",
                "package-search-index.js",
                "type-search-index.js");
    }

    @Test
    void testNoModuleDirectories() {
        javadoc("-d", "out-noMdlDir",
                "--no-module-directories",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchOutput(true, false);
        checkSearchJS();
    }

    @Test
    void testURLEncoding() {
        javadoc("-d", "out-encode-html5",
                "--no-module-directories",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchJS();
        checkSearchIndex(true);
    }

    @Test
    void testURLEncoding_html4() {
        javadoc("-d", "out-encode-html4",
                "-html4",
                "--no-module-directories",
                "-Xdoclint:none",
                "-sourcepath", testSrc,
                "-use",
                "--frames",
                "pkg", "pkg1", "pkg2", "pkg3");
        checkExit(Exit.OK);
        checkSearchJS();
        checkSearchIndex(false);
    }

    void checkDocLintErrors() {
        checkOutput(Output.OUT, true,
                "A sample method. Testing search tag for {@index \"unclosed quote}.",
                "Another test class. Testing empty {@index }.",
                "Constant field. Testing no text in index tag {@index}.",
                "A test field. Testing only white-spaces in index tag text {@index       }.");
    }

    void checkSearchOutput(boolean expectedOutput) {
        checkSearchOutput("overview-summary.html", expectedOutput, true);
    }

    void checkSearchIndex(boolean expectedOutput) {
        checkOutput("member-search-index.js", expectedOutput,
                "{\"p\":\"pkg\",\"c\":\"AnotherClass\",\"l\":\"AnotherClass()\",\"url\":\"%3Cinit%3E()\"}",
                "{\"p\":\"pkg1\",\"c\":\"RegClass\",\"l\":\"RegClass()\",\"url\":\"%3Cinit%3E()\"}",
                "{\"p\":\"pkg2\",\"c\":\"TestError\",\"l\":\"TestError()\",\"url\":\"%3Cinit%3E()\"}",
                "{\"p\":\"pkg\",\"c\":\"AnotherClass\",\"l\":\"method(byte[], int, String)\",\"url\":\"method(byte[],int,java.lang.String)\"}");
        checkOutput("member-search-index.js", !expectedOutput,
                "{\"p\":\"pkg\",\"c\":\"AnotherClass\",\"l\":\"method(RegClass)\",\"url\":\"method-pkg1.RegClass-\"}",
                "{\"p\":\"pkg2\",\"c\":\"TestClass\",\"l\":\"TestClass()\",\"url\":\"TestClass--\"}",
                "{\"p\":\"pkg\",\"c\":\"TestError\",\"l\":\"TestError()\",\"url\":\"TestError--\"}",
                "{\"p\":\"pkg\",\"c\":\"AnotherClass\",\"l\":\"method(byte[], int, String)\",\"url\":\"method-byte:A-int-java.lang.String-\"}");
    }

    void checkSearchOutput(boolean expectedOutput, boolean moduleDirectoriesVar) {
        checkSearchOutput("overview-summary.html", expectedOutput, moduleDirectoriesVar);
    }

    void checkSearchOutput(String fileName, boolean expectedOutput, boolean moduleDirectoriesVar) {
        // Test for search related markup
        checkOutput(fileName, expectedOutput,
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"jquery/jquery-ui.css\" title=\"Style\">\n",
                "<script type=\"text/javascript\" src=\"jquery/jszip/dist/jszip.min.js\"></script>\n",
                "<script type=\"text/javascript\" src=\"jquery/jszip-utils/dist/jszip-utils.min.js\"></script>\n",
                "<!--[if IE]>\n",
                "<script type=\"text/javascript\" src=\"jquery/jszip-utils/dist/jszip-utils-ie.min.js\"></script>\n",
                "<![endif]-->\n",
                "<script type=\"text/javascript\" src=\"jquery/jquery-3.3.1.js\"></script>\n",
                "<script type=\"text/javascript\" src=\"jquery/jquery-migrate-3.0.1.js\"></script>\n",
                "<script type=\"text/javascript\" src=\"jquery/jquery-ui.js\"></script>",
                "var pathtoroot = \"./\";\n"
                + "var useModuleDirectories = " + moduleDirectoriesVar + ";\n"
                + "loadScripts(document, 'script');",
                "<ul class=\"navListSearch\">\n",
                "<li><label for=\"search\">SEARCH:</label>\n"
                + "<input type=\"text\" id=\"search\" value=\"search\" disabled=\"disabled\">\n"
                + "<input type=\"reset\" id=\"reset\" value=\"reset\" disabled=\"disabled\">\n");
        checkOutput(fileName, true,
                "<div class=\"fixedNav\">");
    }

    void checkSingleIndex(boolean expectedOutput, boolean html5) {
        String html_span_see_span = html5 ? "html%3Cspan%3Esee%3C/span%3E" : "html-span-see-/span-";

        // Test for search tags markup in index file.
        checkOutput("index-all.html", expectedOutput,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#phrasewithspaces\">"
                + "phrase with spaces</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#pkg\">"
                + "pkg</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#pkg2.5\">"
                + "pkg2.5</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#r\">"
                + "r</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#searchphrase\">"
                + "search phrase</a></span> - Search tag in pkg1.RegClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#SearchWordWithDescription\">"
                + "SearchWordWithDescription</a></span> - Search tag in pkg1.RegClass.CONSTANT_FIELD_1</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestAnnotationType.html#searchphrasewithdescdeprecated\">"
                + "search phrase with desc deprecated</a></span> - Search tag in pkg2.TestAnnotationType</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestClass.html#SearchTagDeprecatedClass\">"
                + "SearchTagDeprecatedClass</a></span> - Search tag in pkg2.TestClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#SingleWord\">"
                + "SingleWord</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/AnotherClass.ModalExclusionType.html"
                + "#nested%7B@indexnested_tag_test%7D\">nested {@index nested_tag_test}</a></span> - "
                + "Search tag in pkg.AnotherClass.ModalExclusionType.NO_EXCLUDE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/AnotherClass.ModalExclusionType.html"
                + "#" + html_span_see_span + "\">html &lt;span&gt; see &lt;/span&gt;</a></span> - Search "
                + "tag in pkg.AnotherClass.ModalExclusionType.APPLICATION_EXCLUDE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/AnotherClass.html#quoted\">quoted</a>"
                + "</span> - Search tag in pkg.AnotherClass.CONSTANT1</dt>",
                "<dt><span class=\"memberNameLink\"><a href=\"pkg2/TestEnum.html#ONE\">ONE</a></span> - "
                + "pkg2.<a href=\"pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>",
                "<dt><span class=\"memberNameLink\"><a href=\"pkg2/TestEnum.html#THREE\">THREE</a></span> - "
                + "pkg2.<a href=\"pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>",
                "<dt><span class=\"memberNameLink\"><a href=\"pkg2/TestEnum.html#TWO\">TWO</a></span> - "
                + "pkg2.<a href=\"pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>");
        checkOutput("index-all.html", true,
                "<div class=\"deprecationComment\">class_test1 passes. Search tag"
                + " <a id=\"SearchTagDeprecatedClass\" class=\"searchTagResult\">SearchTagDeprecatedClass</a></div>",
                "<div class=\"deprecationComment\">error_test3 passes. Search tag for\n"
                + " method <a id=\"SearchTagDeprecatedMethod\" class=\"searchTagResult\">SearchTagDeprecatedMethod</a></div>");
    }

    void checkSplitIndex() {
        // Test for search tags markup in split index file.
        checkOutput("index-files/index-13.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg1/RegClass.html#searchphrase\">"
                + "search phrase</a></span> - Search tag in pkg1.RegClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg1/RegClass.html#SearchWordWithDescription\">"
                + "SearchWordWithDescription</a></span> - Search tag in pkg1.RegClass.CONSTANT_FIELD_1</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestAnnotationType.html#searchphrasewithdescdeprecated\">"
                + "search phrase with desc deprecated</a></span> - Search tag in pkg2.TestAnnotationType</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestClass.html#SearchTagDeprecatedClass\">"
                + "SearchTagDeprecatedClass</a></span> - Search tag in pkg2.TestClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/package-summary.html#SingleWord\">"
                + "SingleWord</a></span> - Search tag in pkg</dt>",
                "<br><a href=\"../allclasses-index.html\">All&nbsp;Classes</a>&nbsp;"
                + "<a href=\"../allpackages-index.html\">All&nbsp;Packages</a>");
        checkOutput("index-files/index-10.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/package-summary.html#phrasewithspaces\">"
                + "phrase with spaces</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/package-summary.html#pkg\">"
                + "pkg</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/package-summary.html#pkg2.5\">"
                + "pkg2.5</a></span> - Search tag in pkg</dt>");
        checkOutput("index-files/index-12.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/package-summary.html#r\">"
                + "r</a></span> - Search tag in pkg</dt>");
        checkOutput("index-files/index-8.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/AnotherClass.ModalExclusionType.html"
                + "#nested%7B@indexnested_tag_test%7D\">nested {@index nested_tag_test}</a></span> - "
                + "Search tag in pkg.AnotherClass.ModalExclusionType.NO_EXCLUDE</dt>");
        checkOutput("index-files/index-5.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/AnotherClass.ModalExclusionType.html"
                + "#html%3Cspan%3Esee%3C/span%3E\">html &lt;span&gt; see &lt;/span&gt;</a></span> - Search "
                + "tag in pkg.AnotherClass.ModalExclusionType.APPLICATION_EXCLUDE</dt>");
        checkOutput("index-files/index-11.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg/AnotherClass.html#quoted\">quoted</a>"
                + "</span> - Search tag in pkg.AnotherClass.CONSTANT1</dt>");
        checkOutput("index-files/index-9.html", true,
                "<dt><span class=\"memberNameLink\"><a href=\"../pkg2/TestEnum.html#ONE\">ONE</a>"
                + "</span> - pkg2.<a href=\"../pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>");
        checkOutput("index-files/index-14.html", true,
                "<dt><span class=\"memberNameLink\"><a href=\"../pkg2/TestEnum.html#THREE\">THREE</a></span> - "
                + "pkg2.<a href=\"../pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>",
                "<dt><span class=\"memberNameLink\"><a href=\"../pkg2/TestEnum.html#TWO\">TWO</a></span> - "
                + "pkg2.<a href=\"../pkg2/TestEnum.html\" title=\"enum in pkg2\">TestEnum</a></dt>");
    }

    void checkIndexNoComment() {
        // Test for search tags markup in index file when javadoc is executed with -nocomment.
        checkOutput("index-all.html", false,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#phrasewithspaces\">"
                + "phrase with spaces</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#pkg\">"
                + "pkg</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#pkg2.5\">"
                + "pkg2.5</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#r\">"
                + "r</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#searchphrase\">"
                + "search phrase</a></span> - Search tag in pkg1.RegClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#SearchWordWithDescription\">"
                + "SearchWordWithDescription</a></span> - Search tag in pkg1.RegClass.CONSTANT_FIELD_1</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestAnnotationType.html#searchphrasewithdescdeprecated\">"
                + "search phrase with desc deprecated</a></span> - Search tag in pkg2.TestAnnotationType</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestClass.html#SearchTagDeprecatedClass\">"
                + "SearchTagDeprecatedClass</a></span> - Search tag in pkg2.TestClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#SingleWord\">"
                + "SingleWord</a></span> - Search tag in pkg</dt>",
                "<div class=\"deprecationComment\">class_test1 passes. Search tag"
                + " <a id=\"SearchTagDeprecatedClass\">SearchTagDeprecatedClass</a></div>",
                "<div class=\"deprecationComment\">error_test3 passes. Search tag for\n"
                + " method <a id=\"SearchTagDeprecatedMethod\">SearchTagDeprecatedMethod</a></div>");
        checkOutput("index-all.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>");
    }

    void checkIndexNoDeprecated() {
        // Test for search tags markup in index file when javadoc is executed using -nodeprecated.
        checkOutput("index-all.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#phrasewithspaces\">"
                + "phrase with spaces</a></span> - Search tag in pkg</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#searchphrase\">"
                + "search phrase</a></span> - Search tag in pkg1.RegClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg1/RegClass.html#SearchWordWithDescription\">"
                + "SearchWordWithDescription</a></span> - Search tag in pkg1.RegClass.CONSTANT_FIELD_1</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg/package-summary.html#SingleWord\">"
                + "SingleWord</a></span> - Search tag in pkg</dt>");
        checkOutput("index-all.html", false,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestAnnotationType.html#searchphrasewithdescdeprecated\">"
                + "search phrase with desc deprecated</a></span> - Search tag in pkg2.TestAnnotationType</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestClass.html#SearchTagDeprecatedClass\">"
                + "SearchTagDeprecatedClass</a></span> - Search tag in pkg2.TestClass</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestEnum.html#searchphrasedeprecated\">"
                + "search phrase deprecated</a></span> - Search tag in pkg2.TestEnum.ONE</dt>",
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>",
                "<div class=\"deprecationComment\">class_test1 passes. Search tag"
                + " <a id=\"SearchTagDeprecatedClass\">SearchTagDeprecatedClass</a></div>",
                "<div class=\"deprecationComment\">error_test3 passes. Search tag for\n"
                + " method <a id=\"SearchTagDeprecatedMethod\">SearchTagDeprecatedMethod</a></div>");
    }

    void checkJavaFXOutput() {
        checkOutput("index-all.html", false, "test treat as private");
    }

    void checkInvalidUsageIndexTag() {
        checkOutput(Output.OUT, true,
                "AnotherClass.java:29: warning - invalid usage of tag {@index",
                "AnotherClass.java:41: warning - invalid usage of tag {@index",
                "AnotherClass.java:36: warning - invalid usage of tag {@index",
                "AnotherClass.java:70: warning - invalid usage of tag {@index");
    }

    void checkJqueryAndImageFiles(boolean expectedOutput) {
        checkFiles(expectedOutput,
                "search.js",
                "jquery/jquery-3.3.1.js",
                "jquery/jquery-migrate-3.0.1.js",
                "jquery/jquery-ui.js",
                "jquery/jquery-ui.css",
                "jquery/jquery-ui.min.js",
                "jquery/jquery-ui.min.css",
                "jquery/jquery-ui.structure.min.css",
                "jquery/jquery-ui.structure.css",
                "jquery/external/jquery/jquery.js",
                "jquery/jszip/dist/jszip.js",
                "jquery/jszip/dist/jszip.min.js",
                "jquery/jszip-utils/dist/jszip-utils.js",
                "jquery/jszip-utils/dist/jszip-utils.min.js",
                "jquery/jszip-utils/dist/jszip-utils-ie.js",
                "jquery/jszip-utils/dist/jszip-utils-ie.min.js",
                "jquery/images/ui-bg_glass_65_dadada_1x400.png",
                "jquery/images/ui-icons_454545_256x240.png",
                "jquery/images/ui-bg_glass_95_fef1ec_1x400.png",
                "jquery/images/ui-bg_glass_75_dadada_1x400.png",
                "jquery/images/ui-bg_highlight-soft_75_cccccc_1x100.png",
                "jquery/images/ui-icons_888888_256x240.png",
                "jquery/images/ui-icons_2e83ff_256x240.png",
                "jquery/images/ui-icons_cd0a0a_256x240.png",
                "jquery/images/ui-bg_glass_55_fbf9ee_1x400.png",
                "jquery/images/ui-icons_222222_256x240.png",
                "jquery/images/ui-bg_glass_75_e6e6e6_1x400.png",
                "resources/x.png",
                "resources/glass.png");
    }

    void checkSearchJS() {
        checkOutput("search.js", true,
                "camelCaseRegexp = ($.ui.autocomplete.escapeRegex(request.term)).split(/(?=[A-Z])/).join(\"([a-z0-9_$]*?)\");",
                "var camelCaseMatcher = new RegExp(\"^\" + camelCaseRegexp);",
                "camelCaseMatcher.test(item.l)",
                "var secondaryresult = new Array();",
                "function nestedName(e) {",
                "function concatResults(a1, a2) {",
                "if (exactMatcher.test(item.l)) {\n"
                + "                        presult.push(item);",
                "$(\"#search\").on('click keydown', function() {\n"
                + "        if ($(this).val() == watermark) {\n"
                + "            $(this).val('').removeClass('watermark');\n"
                + "        }\n"
                + "    });",
                "function getURLPrefix(ui) {\n"
                + "    var urlPrefix=\"\";\n"
                + "    if (useModuleDirectories) {\n"
                + "        var slash = \"/\";\n"
                + "        if (ui.item.category === catModules) {\n"
                + "            return ui.item.l + slash;\n"
                + "        } else if (ui.item.category === catPackages && ui.item.m) {\n"
                + "            return ui.item.m + slash;\n"
                + "        } else if ((ui.item.category === catTypes && ui.item.p) || ui.item.category === catMembers) {\n"
                + "            $.each(packageSearchIndex, function(index, item) {\n"
                + "                if (ui.item.p == item.l) {\n"
                + "                    urlPrefix = item.m + slash;\n"
                + "                }\n"
                + "            });\n"
                + "            return urlPrefix;\n"
                + "        } else {\n"
                + "            return urlPrefix;\n"
                + "        }\n"
                + "    }\n"
                + "    return urlPrefix;\n"
                + "}",
                "url += ui.item.l;");
    }

    void checkSingleIndexSearchTagDuplication() {
        // Test for search tags duplication in index file.
        checkOutput("index-all.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>");
        checkOutput("index-all.html", false,
                "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>\n"
                + "<dt><span class=\"searchTagLink\"><a href=\"pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>");
    }

    void checkSplitIndexSearchTagDuplication() {
        // Test for search tags duplication in index file.
        checkOutput("index-files/index-13.html", true,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>");
        checkOutput("index-files/index-13.html", false,
                "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>\n"
                + "<dt><span class=\"searchTagLink\"><a href=\"../pkg2/TestError.html#SearchTagDeprecatedMethod\">"
                + "SearchTagDeprecatedMethod</a></span> - Search tag in pkg2.TestError</dt>\n"
                + "<dd>with description</dd>");
    }

    void checkAllPkgsAllClasses() {
        checkOutput("allclasses-index.html", true,
                "<table class=\"typeSummary\">\n"
                + "<caption><span id=\"t0\" class=\"activeTableTab\"><span>All Classes</span>"
                + "<span class=\"tabEnd\">&nbsp;</span></span><span id=\"t1\" class=\"tableTab\">"
                + "<span><a href=\"javascript:show(1);\">Interface Summary</a></span><span class=\"tabEnd\">"
                + "&nbsp;</span></span><span id=\"t2\" class=\"tableTab\"><span><a href=\"javascript:show(2);\">"
                + "Class Summary</a></span><span class=\"tabEnd\">&nbsp;</span></span><span id=\"t3\" class=\"tableTab\">"
                + "<span><a href=\"javascript:show(4);\">Enum Summary</a></span><span class=\"tabEnd\">&nbsp;"
                + "</span></span><span id=\"t4\" class=\"tableTab\"><span><a href=\"javascript:show(8);\">"
                + "Exception Summary</a></span><span class=\"tabEnd\">&nbsp;</span></span>"
                + "<span id=\"t5\" class=\"tableTab\"><span><a href=\"javascript:show(16);\">"
                + "Error Summary</a></span><span class=\"tabEnd\">&nbsp;</span></span>"
                + "<span id=\"t6\" class=\"tableTab\"><span><a href=\"javascript:show(32);\">Annotation Types Summary"
                + "</a></span><span class=\"tabEnd\">&nbsp;</span></span></caption>\n"
                + "<tr>\n"
                + "<th class=\"colFirst\" scope=\"col\">Class</th>\n"
                + "<th class=\"colLast\" scope=\"col\">Description</th>\n"
                + "</tr>",
                "var data = {\"i0\":32,\"i1\":2,\"i2\":4,\"i3\":2,\"i4\":2,\"i5\":1,\"i6\":2,\"i7\":32,"
                + "\"i8\":2,\"i9\":4,\"i10\":16,\"i11\":16,\"i12\":8,\"i13\":8,\"i14\":1,\"i15\":2};");
        checkOutput("allpackages-index.html", true,
                "<table class=\"packagesSummary\">\n"
                + "<caption><span>Package Summary</span><span class=\"tabEnd\">&nbsp;</span></caption>\n"
                + "<tr>\n"
                + "<th class=\"colFirst\" scope=\"col\">Package</th>\n"
                + "<th class=\"colLast\" scope=\"col\">Description</th>\n"
                + "</tr>\n");
        checkOutput("type-search-index.js", true,
                "{\"l\":\"All Classes\",\"url\":\"allclasses-index.html\"}");
        checkOutput("package-search-index.js", true,
                "{\"l\":\"All Packages\",\"url\":\"allpackages-index.html\"}");
        checkOutput("index-all.html", true,
                "<br><a href=\"allclasses-index.html\">All&nbsp;Classes</a>&nbsp;"
                + "<a href=\"allpackages-index.html\">All&nbsp;Packages</a>");
}
}
