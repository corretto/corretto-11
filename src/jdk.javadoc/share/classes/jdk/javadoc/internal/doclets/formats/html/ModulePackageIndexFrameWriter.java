/*
 * Copyright (c) 2013, 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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

package jdk.javadoc.internal.doclets.formats.html;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.lang.model.element.ModuleElement;
import javax.lang.model.element.PackageElement;

import jdk.javadoc.internal.doclets.formats.html.markup.HtmlConstants;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlStyle;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlTag;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlTree;
import jdk.javadoc.internal.doclets.formats.html.markup.RawHtml;
import jdk.javadoc.internal.doclets.formats.html.markup.StringContent;
import jdk.javadoc.internal.doclets.toolkit.Content;
import jdk.javadoc.internal.doclets.toolkit.util.DocFileIOException;
import jdk.javadoc.internal.doclets.toolkit.util.DocPath;
import jdk.javadoc.internal.doclets.toolkit.util.DocPaths;

/**
 * Generate the module package index for the left-hand frame in the generated output.
 * A click on the package name in this frame will update the page in the bottom
 * left hand frame with the listing of contents of the clicked module package.
 *
 *  <p><b>This is NOT part of any supported API.
 *  If you write code that depends on this, you do so at your own risk.
 *  This code and its internal interfaces are subject to change or
 *  deletion without notice.</b>
 *
 * @author Bhavesh Patel
 */
public class ModulePackageIndexFrameWriter extends AbstractModuleIndexWriter {

    /**
     * Construct the ModulePackageIndexFrameWriter object.
     *
     * @param configuration the configuration object
     * @param filename Name of the package index file to be generated.
     */
    public ModulePackageIndexFrameWriter(HtmlConfiguration configuration, DocPath filename)  {
        super(configuration, filename);
    }

    /**
     * Generate the module package index file.
     * @throws DocFileIOException
     * @param configuration the configuration object
     * @param mdle the module being documented
     */
    public static void generate(HtmlConfiguration configuration, ModuleElement mdle) throws DocFileIOException {
        DocPath filename = configuration.docPaths.moduleFrame(mdle);
        ModulePackageIndexFrameWriter modpackgen = new ModulePackageIndexFrameWriter(configuration, filename);
        modpackgen.buildModulePackagesIndexFile("doclet.Window_Overview", false, mdle);
    }

    /**
     * {@inheritDoc}
     */
    protected void addModulePackagesList(Map<ModuleElement, Set<PackageElement>> modules, String text,
            String tableSummary, Content main, ModuleElement mdle) {
        Content profNameContent = new StringContent(mdle.getQualifiedName().toString());
        Content heading = HtmlTree.HEADING(HtmlConstants.PACKAGE_HEADING, true,
                getTargetModuleLink("classFrame", profNameContent, mdle));
        heading.add(Contents.SPACE);
        heading.add(contents.packagesLabel);
        HtmlTree htmlTree = HtmlTree.DIV(HtmlStyle.indexContainer, heading);
        HtmlTree ul = new HtmlTree(HtmlTag.UL);
        ul.setTitle(contents.packagesLabel);
        List<PackageElement> packages = new ArrayList<>(modules.get(mdle));
        for (PackageElement pkg : packages) {
            if ((!(configuration.nodeprecated && utils.isDeprecated(pkg)))) {
                ul.add(getPackage(pkg, mdle));
            }
        }
        htmlTree.add(ul);
        main.add(htmlTree);
    }

    /**
     * {@inheritDoc}
     */
    protected void addModulePackagesList(Set<ModuleElement> modules, String text,
            String tableSummary, Content body, ModuleElement mdle) {
        Content moduleNameContent = new StringContent(mdle.getQualifiedName().toString());
        Content heading = HtmlTree.HEADING(HtmlConstants.PACKAGE_HEADING, true,
                getTargetModuleLink("classFrame", moduleNameContent, mdle));
        heading.add(Contents.SPACE);
        heading.add(contents.packagesLabel);
        HtmlTree htmlTree = (configuration.allowTag(HtmlTag.MAIN))
                ? HtmlTree.MAIN(HtmlStyle.indexContainer, heading)
                : HtmlTree.DIV(HtmlStyle.indexContainer, heading);
        HtmlTree ul = new HtmlTree(HtmlTag.UL);
        ul.setTitle(contents.packagesLabel);
        Set<PackageElement> modulePackages = configuration.modulePackages.get(mdle);
        for (PackageElement pkg: modulePackages) {
            if ((!(configuration.nodeprecated && utils.isDeprecated(pkg)))) {
                ul.add(getPackage(pkg, mdle));
            }
        }
        htmlTree.add(ul);
        body.add(htmlTree);
    }

    /**
     * Returns each package name as a separate link.
     *
     * @param pkg PackageElement
     * @param mdle the module being documented
     * @return content for the package link
     */
    protected Content getPackage(PackageElement pkg, ModuleElement mdle) {
        Content packageLinkContent;
        Content pkgLabel;
        if (!pkg.isUnnamed()) {
            pkgLabel = getPackageLabel(utils.getPackageName(pkg));
            packageLinkContent = links.createLink(pathString(pkg,
                     DocPaths.PACKAGE_FRAME), pkgLabel, "",
                    "packageFrame");
        } else {
            pkgLabel = new StringContent("<unnamed package>");
            packageLinkContent = links.createLink(DocPaths.PACKAGE_FRAME,
                    pkgLabel, "", "packageFrame");
        }
        Content li = HtmlTree.LI(packageLinkContent);
        return li;
    }

    /**
     * {@inheritDoc}
     */
    protected void addNavigationBarHeader(Content header) {
        Content headerContent;
        if (configuration.packagesheader.length() > 0) {
            headerContent = new RawHtml(replaceDocRootDir(configuration.packagesheader));
        } else {
            headerContent = new RawHtml(replaceDocRootDir(configuration.header));
        }
        Content heading = HtmlTree.HEADING(HtmlConstants.TITLE_HEADING, true,
                HtmlStyle.bar, headerContent);
        header.add(heading);
    }

    /**
     * Do nothing as there is no overview information in this page.
     */
    protected void addOverviewHeader(Content body) {
    }

    /**
     * Do nothing as there is no modules list on this page.
     */
    protected void addModulesList(Content body) {
    }

    /**
     * Adds "All Classes" link for the top of the left-hand frame page to the
     * documentation tree.
     *
     * @param ul the Content object to which the all classes link should be added
     */
    protected void addAllClassesLink(Content ul) {
        DocPath allClassesFrame = configuration.useModuleDirectories
                ? DocPaths.DOT_DOT.resolve(DocPaths.ALLCLASSES_FRAME)
                : DocPaths.ALLCLASSES_FRAME;
        Content linkContent = links.createLink(allClassesFrame,
                contents.allClassesLabel, "", "packageFrame");
        Content li = HtmlTree.LI(linkContent);
        ul.add(li);
    }

    /**
     * Adds "All Packages" link for the top of the left-hand frame page to the
     * documentation tree.
     *
     * @param ul the Content object to which the all packages link should be added
     */
    protected void addAllPackagesLink(Content ul) {
        DocPath overviewFrame = configuration.useModuleDirectories
                ? DocPaths.DOT_DOT.resolve(DocPaths.OVERVIEW_FRAME)
                : DocPaths.OVERVIEW_FRAME;
        Content linkContent = links.createLink(overviewFrame,
                contents.allPackagesLabel, "", "packageListFrame");
        Content li = HtmlTree.LI(linkContent);
        ul.add(li);
    }

    /**
     * Adds "All Modules" link for the top of the left-hand frame page to the
     * documentation tree.
     *
     * @param ul the Content object to which the all modules link should be added
     */
    protected void addAllModulesLink(Content ul) {
        DocPath moduleOverviewFrame = configuration.useModuleDirectories
                ? DocPaths.DOT_DOT.resolve(DocPaths.MODULE_OVERVIEW_FRAME)
                : DocPaths.MODULE_OVERVIEW_FRAME;
        Content linkContent = links.createLink(moduleOverviewFrame,
                contents.allModulesLabel, "", "packageListFrame");
        Content li = HtmlTree.LI(linkContent);
        ul.add(li);
    }

    /**
     * {@inheritDoc}
     */
    protected void addNavigationBarFooter(Content footer) {
        Content p = HtmlTree.P(Contents.SPACE);
        footer.add(p);
    }
}
