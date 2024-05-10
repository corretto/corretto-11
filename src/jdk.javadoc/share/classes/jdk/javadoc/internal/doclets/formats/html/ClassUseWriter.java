/*
 * Copyright (c) 1998, 2018, Oracle and/or its affiliates. All rights reserved.
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

import jdk.javadoc.internal.doclets.formats.html.markup.Table;

import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import javax.lang.model.element.Element;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import javax.tools.Diagnostic;

import jdk.javadoc.internal.doclets.formats.html.markup.ContentBuilder;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlConstants;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlStyle;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlTag;
import jdk.javadoc.internal.doclets.formats.html.markup.HtmlTree;
import jdk.javadoc.internal.doclets.formats.html.markup.Navigation;
import jdk.javadoc.internal.doclets.formats.html.markup.Navigation.PageMode;
import jdk.javadoc.internal.doclets.formats.html.markup.StringContent;
import jdk.javadoc.internal.doclets.toolkit.Content;
import jdk.javadoc.internal.doclets.toolkit.util.ClassTree;
import jdk.javadoc.internal.doclets.toolkit.util.ClassUseMapper;
import jdk.javadoc.internal.doclets.toolkit.util.DocFileIOException;
import jdk.javadoc.internal.doclets.toolkit.util.DocPath;
import jdk.javadoc.internal.doclets.toolkit.util.DocPaths;


/**
 * Generate class usage information.
 *
 *  <p><b>This is NOT part of any supported API.
 *  If you write code that depends on this, you do so at your own risk.
 *  This code and its internal interfaces are subject to change or
 *  deletion without notice.</b>
 *
 * @author Robert G. Field
 * @author Bhavesh Patel (Modified)
 */
public class ClassUseWriter extends SubWriterHolderWriter {

    final TypeElement typeElement;
    Set<PackageElement> pkgToPackageAnnotations = null;
    final Map<PackageElement, List<Element>> pkgToClassTypeParameter;
    final Map<PackageElement, List<Element>> pkgToClassAnnotations;
    final Map<PackageElement, List<Element>> pkgToMethodTypeParameter;
    final Map<PackageElement, List<Element>> pkgToMethodArgTypeParameter;
    final Map<PackageElement, List<Element>> pkgToMethodReturnTypeParameter;
    final Map<PackageElement, List<Element>> pkgToMethodAnnotations;
    final Map<PackageElement, List<Element>> pkgToMethodParameterAnnotations;
    final Map<PackageElement, List<Element>> pkgToFieldTypeParameter;
    final Map<PackageElement, List<Element>> pkgToFieldAnnotations;
    final Map<PackageElement, List<Element>> pkgToSubclass;
    final Map<PackageElement, List<Element>> pkgToSubinterface;
    final Map<PackageElement, List<Element>> pkgToImplementingClass;
    final Map<PackageElement, List<Element>> pkgToField;
    final Map<PackageElement, List<Element>> pkgToMethodReturn;
    final Map<PackageElement, List<Element>> pkgToMethodArgs;
    final Map<PackageElement, List<Element>> pkgToMethodThrows;
    final Map<PackageElement, List<Element>> pkgToConstructorAnnotations;
    final Map<PackageElement, List<Element>> pkgToConstructorParameterAnnotations;
    final Map<PackageElement, List<Element>> pkgToConstructorArgs;
    final Map<PackageElement, List<Element>> pkgToConstructorArgTypeParameter;
    final Map<PackageElement, List<Element>> pkgToConstructorThrows;
    final SortedSet<PackageElement> pkgSet;
    final MethodWriterImpl methodSubWriter;
    final ConstructorWriterImpl constrSubWriter;
    final FieldWriterImpl fieldSubWriter;
    final NestedClassWriterImpl classSubWriter;
    // Summary for various use tables.
    final String classUseTableSummary;
    final String subclassUseTableSummary;
    final String subinterfaceUseTableSummary;
    final String fieldUseTableSummary;
    final String methodUseTableSummary;
    final String constructorUseTableSummary;
    final String packageUseTableSummary;
    private final Navigation navBar;

    /**
     * The HTML tree for main tag.
     */
    protected HtmlTree mainTree = HtmlTree.MAIN();

    /**
     * Constructor.
     *
     * @param filename the file to be generated.
     */
    public ClassUseWriter(HtmlConfiguration configuration,
                          ClassUseMapper mapper, DocPath filename,
                          TypeElement typeElement) {
        super(configuration, filename);
        this.typeElement = typeElement;
        if (mapper.classToPackageAnnotations.containsKey(typeElement)) {
            pkgToPackageAnnotations = new TreeSet<>(utils.makeClassUseComparator());
            pkgToPackageAnnotations.addAll(mapper.classToPackageAnnotations.get(typeElement));
        }
        configuration.currentTypeElement = typeElement;
        this.pkgSet = new TreeSet<>(utils.makePackageComparator());
        this.pkgToClassTypeParameter = pkgDivide(mapper.classToClassTypeParam);
        this.pkgToClassAnnotations = pkgDivide(mapper.classToClassAnnotations);
        this.pkgToMethodTypeParameter = pkgDivide(mapper.classToMethodTypeParam);
        this.pkgToMethodArgTypeParameter = pkgDivide(mapper.classToMethodArgTypeParam);
        this.pkgToFieldTypeParameter = pkgDivide(mapper.classToFieldTypeParam);
        this.pkgToFieldAnnotations = pkgDivide(mapper.annotationToField);
        this.pkgToMethodReturnTypeParameter = pkgDivide(mapper.classToMethodReturnTypeParam);
        this.pkgToMethodAnnotations = pkgDivide(mapper.classToMethodAnnotations);
        this.pkgToMethodParameterAnnotations = pkgDivide(mapper.classToMethodParamAnnotation);
        this.pkgToSubclass = pkgDivide(mapper.classToSubclass);
        this.pkgToSubinterface = pkgDivide(mapper.classToSubinterface);
        this.pkgToImplementingClass = pkgDivide(mapper.classToImplementingClass);
        this.pkgToField = pkgDivide(mapper.classToField);
        this.pkgToMethodReturn = pkgDivide(mapper.classToMethodReturn);
        this.pkgToMethodArgs = pkgDivide(mapper.classToMethodArgs);
        this.pkgToMethodThrows = pkgDivide(mapper.classToMethodThrows);
        this.pkgToConstructorAnnotations = pkgDivide(mapper.classToConstructorAnnotations);
        this.pkgToConstructorParameterAnnotations = pkgDivide(mapper.classToConstructorParamAnnotation);
        this.pkgToConstructorArgs = pkgDivide(mapper.classToConstructorArgs);
        this.pkgToConstructorArgTypeParameter = pkgDivide(mapper.classToConstructorArgTypeParam);
        this.pkgToConstructorThrows = pkgDivide(mapper.classToConstructorThrows);
        //tmp test
        if (pkgSet.size() > 0 &&
            mapper.classToPackage.containsKey(this.typeElement) &&
            !pkgSet.equals(mapper.classToPackage.get(this.typeElement))) {
            configuration.reporter.print(Diagnostic.Kind.WARNING,
                    "Internal error: package sets don't match: "
                    + pkgSet + " with: " + mapper.classToPackage.get(this.typeElement));
        }

        methodSubWriter = new MethodWriterImpl(this);
        constrSubWriter = new ConstructorWriterImpl(this);
        fieldSubWriter = new FieldWriterImpl(this);
        classSubWriter = new NestedClassWriterImpl(this);

        String useTableSummary = resources.getText("doclet.Use_Table_Summary");
        classUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.classes"));
        subclassUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.subclasses"));
        subinterfaceUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.subinterfaces"));
        fieldUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.fields"));
        methodUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.methods"));
        constructorUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.constructors"));
        packageUseTableSummary = MessageFormat.format(useTableSummary,
                resources.getText("doclet.packages"));
        this.navBar = new Navigation(typeElement, configuration, fixedNavDiv, PageMode.USE, path);
    }

    /**
     * Write out class use pages.
     *
     * @param configuration the configuration for this doclet
     * @param classtree the class tree hierarchy
     * @throws DocFileIOException if there is an error while generating the documentation
     */
    public static void generate(HtmlConfiguration configuration, ClassTree classtree) throws DocFileIOException  {
        ClassUseMapper mapper = new ClassUseMapper(configuration, classtree);
        for (TypeElement aClass : configuration.getIncludedTypeElements()) {
            // If -nodeprecated option is set and the containing package is marked
            // as deprecated, do not generate the class-use page. We will still generate
            // the class-use page if the class is marked as deprecated but the containing
            // package is not since it could still be linked from that package-use page.
            if (!(configuration.nodeprecated &&
                  configuration.utils.isDeprecated(configuration.utils.containingPackage(aClass))))
                ClassUseWriter.generate(configuration, mapper, aClass);
        }
        for (PackageElement pkg : configuration.packages) {
            // If -nodeprecated option is set and the package is marked
            // as deprecated, do not generate the package-use page.
            if (!(configuration.nodeprecated && configuration.utils.isDeprecated(pkg)))
                PackageUseWriter.generate(configuration, mapper, pkg);
        }
    }

    private Map<PackageElement, List<Element>> pkgDivide(Map<TypeElement, ? extends List<? extends Element>> classMap) {
        Map<PackageElement, List<Element>> map = new HashMap<>();
        List<? extends Element> elements = (List<? extends Element>) classMap.get(typeElement);
        if (elements != null) {
            Collections.sort(elements, utils.makeClassUseComparator());
            for (Element e : elements) {
                PackageElement pkg = utils.containingPackage(e);
                pkgSet.add(pkg);
                List<Element> inPkg = map.get(pkg);
                if (inPkg == null) {
                    inPkg = new ArrayList<>();
                    map.put(pkg, inPkg);
                }
                inPkg.add(e);
            }
        }
        return map;
    }

    /**
     * Generate a class page.
     *
     * @throws DocFileIOException if there is a problem while generating the documentation
     */
    public static void generate(HtmlConfiguration configuration, ClassUseMapper mapper,
                                TypeElement typeElement) throws DocFileIOException {
        ClassUseWriter clsgen;
        DocPath path = configuration.docPaths.forPackage(typeElement)
                              .resolve(DocPaths.CLASS_USE)
                              .resolve(configuration.docPaths.forName( typeElement));
        clsgen = new ClassUseWriter(configuration, mapper, path, typeElement);
        clsgen.generateClassUseFile();
    }

    /**
     * Generate the class use elements.
     *
     * @throws DocFileIOException if there is a problem while generating the documentation
     */
    protected void generateClassUseFile() throws DocFileIOException {
        HtmlTree body = getClassUseHeader();
        HtmlTree div = new HtmlTree(HtmlTag.DIV);
        div.setStyle(HtmlStyle.classUseContainer);
        if (pkgSet.size() > 0) {
            addClassUse(div);
        } else {
            div.add(contents.getContent("doclet.ClassUse_No.usage.of.0",
                    utils.getFullyQualifiedName(typeElement)));
        }
        if (configuration.allowTag(HtmlTag.MAIN)) {
            mainTree.add(div);
            body.add(mainTree);
        } else {
            body.add(div);
        }
        HtmlTree htmlTree = (configuration.allowTag(HtmlTag.FOOTER))
                ? HtmlTree.FOOTER()
                : body;
        navBar.setUserFooter(getUserHeaderFooter(false));
        htmlTree.add(navBar.getContent(false));
        addBottom(htmlTree);
        if (configuration.allowTag(HtmlTag.FOOTER)) {
            body.add(htmlTree);
        }
        printHtmlDocument(null, true, body);
    }

    /**
     * Add the class use documentation.
     *
     * @param contentTree the content tree to which the class use information will be added
     */
    protected void addClassUse(Content contentTree) {
        HtmlTree ul = new HtmlTree(HtmlTag.UL);
        ul.setStyle(HtmlStyle.blockList);
        if (configuration.packages.size() > 1) {
            addPackageList(ul);
            addPackageAnnotationList(ul);
        }
        addClassList(ul);
        contentTree.add(ul);
    }

    /**
     * Add the packages elements that use the given class.
     *
     * @param contentTree the content tree to which the packages elements will be added
     */
    protected void addPackageList(Content contentTree) {
        Content caption = getTableCaption(contents.getContent(
                "doclet.ClassUse_Packages.that.use.0",
                getLink(new LinkInfoImpl(configuration,
                        LinkInfoImpl.Kind.CLASS_USE_HEADER, typeElement))));
        Table table = new Table(configuration.htmlVersion, HtmlStyle.useSummary)
                .setSummary(packageUseTableSummary)
                .setCaption(caption)
                .setHeader(getPackageTableHeader())
                .setColumnStyles(HtmlStyle.colFirst, HtmlStyle.colLast);
        for (PackageElement pkg : pkgSet) {
            addPackageUse(pkg, table);
        }
        Content li = HtmlTree.LI(HtmlStyle.blockList, table.toContent());
        contentTree.add(li);
    }

    /**
     * Add the package annotation elements.
     *
     * @param contentTree the content tree to which the package annotation elements will be added
     */
    protected void addPackageAnnotationList(Content contentTree) {
        if (!utils.isAnnotationType(typeElement) ||
                pkgToPackageAnnotations == null ||
                pkgToPackageAnnotations.isEmpty()) {
            return;
        }
        Content caption = getTableCaption(contents.getContent(
                "doclet.ClassUse_PackageAnnotation",
                getLink(new LinkInfoImpl(configuration,
                        LinkInfoImpl.Kind.CLASS_USE_HEADER, typeElement))));

        Table table = new Table(configuration.htmlVersion, HtmlStyle.useSummary)
                .setSummary(packageUseTableSummary)
                .setCaption(caption)
                .setHeader(getPackageTableHeader())
                .setColumnStyles(HtmlStyle.colFirst, HtmlStyle.colLast);
        for (PackageElement pkg : pkgToPackageAnnotations) {
            Content summary = new ContentBuilder();
            addSummaryComment(pkg, summary);
            table.addRow(getPackageLink(pkg), summary);
        }
        Content li = HtmlTree.LI(HtmlStyle.blockList, table.toContent());
        contentTree.add(li);
    }

    /**
     * Add the class elements that use the given class.
     *
     * @param contentTree the content tree to which the class elements will be added
     */
    protected void addClassList(Content contentTree) {
        HtmlTree ul = new HtmlTree(HtmlTag.UL);
        ul.setStyle(HtmlStyle.blockList);
        for (PackageElement pkg : pkgSet) {
            Content markerAnchor = links.createAnchor(getPackageAnchorName(pkg));
            HtmlTree htmlTree = (configuration.allowTag(HtmlTag.SECTION))
                    ? HtmlTree.SECTION(markerAnchor)
                    : HtmlTree.LI(HtmlStyle.blockList, markerAnchor);
            Content link = contents.getContent("doclet.ClassUse_Uses.of.0.in.1",
                    getLink(new LinkInfoImpl(configuration, LinkInfoImpl.Kind.CLASS_USE_HEADER,
                            typeElement)),
                    getPackageLink(pkg, utils.getPackageName(pkg)));
            Content heading = HtmlTree.HEADING(HtmlConstants.SUMMARY_HEADING, link);
            htmlTree.add(heading);
            addClassUse(pkg, htmlTree);
            if (configuration.allowTag(HtmlTag.SECTION)) {
                ul.add(HtmlTree.LI(HtmlStyle.blockList, htmlTree));
            } else {
                ul.add(htmlTree);
            }
        }
        Content li = HtmlTree.LI(HtmlStyle.blockList, ul);
        contentTree.add(li);
    }

    /**
     * Add the package use information.
     *
     * @param pkg the package that uses the given class
     * @param table the table to which the package use information will be added
     */
    protected void addPackageUse(PackageElement pkg, Table table) {
        Content pkgLink =
                links.createLink(getPackageAnchorName(pkg), new StringContent(utils.getPackageName(pkg)));
        Content summary = new ContentBuilder();
        addSummaryComment(pkg, summary);
        table.addRow(pkgLink, summary);
    }

    /**
     * Add the class use information.
     *
     * @param pkg the package that uses the given class
     * @param contentTree the content tree to which the class use information will be added
     */
    protected void addClassUse(PackageElement pkg, Content contentTree) {
        Content classLink = getLink(new LinkInfoImpl(configuration,
            LinkInfoImpl.Kind.CLASS_USE_HEADER, typeElement));
        Content pkgLink = getPackageLink(pkg, utils.getPackageName(pkg));
        classSubWriter.addUseInfo(pkgToClassAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_Annotation", classLink,
                pkgLink), classUseTableSummary, contentTree);
        classSubWriter.addUseInfo(pkgToClassTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_TypeParameter", classLink,
                pkgLink), classUseTableSummary, contentTree);
        classSubWriter.addUseInfo(pkgToSubclass.get(pkg),
                configuration.getContent("doclet.ClassUse_Subclass", classLink,
                pkgLink), subclassUseTableSummary, contentTree);
        classSubWriter.addUseInfo(pkgToSubinterface.get(pkg),
                configuration.getContent("doclet.ClassUse_Subinterface", classLink,
                pkgLink), subinterfaceUseTableSummary, contentTree);
        classSubWriter.addUseInfo(pkgToImplementingClass.get(pkg),
                configuration.getContent("doclet.ClassUse_ImplementingClass", classLink,
                pkgLink), classUseTableSummary, contentTree);
        fieldSubWriter.addUseInfo(pkgToField.get(pkg),
                configuration.getContent("doclet.ClassUse_Field", classLink,
                pkgLink), fieldUseTableSummary, contentTree);
        fieldSubWriter.addUseInfo(pkgToFieldAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_FieldAnnotations", classLink,
                pkgLink), fieldUseTableSummary, contentTree);
        fieldSubWriter.addUseInfo(pkgToFieldTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_FieldTypeParameter", classLink,
                pkgLink), fieldUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodAnnotations", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodParameterAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodParameterAnnotations", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodTypeParameter", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodReturn.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodReturn", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodReturnTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodReturnTypeParameter", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodArgs.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodArgs", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodArgTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodArgsTypeParameters", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        methodSubWriter.addUseInfo(pkgToMethodThrows.get(pkg),
                configuration.getContent("doclet.ClassUse_MethodThrows", classLink,
                pkgLink), methodUseTableSummary, contentTree);
        constrSubWriter.addUseInfo(pkgToConstructorAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_ConstructorAnnotations", classLink,
                pkgLink), constructorUseTableSummary, contentTree);
        constrSubWriter.addUseInfo(pkgToConstructorParameterAnnotations.get(pkg),
                configuration.getContent("doclet.ClassUse_ConstructorParameterAnnotations", classLink,
                pkgLink), constructorUseTableSummary, contentTree);
        constrSubWriter.addUseInfo(pkgToConstructorArgs.get(pkg),
                configuration.getContent("doclet.ClassUse_ConstructorArgs", classLink,
                pkgLink), constructorUseTableSummary, contentTree);
        constrSubWriter.addUseInfo(pkgToConstructorArgTypeParameter.get(pkg),
                configuration.getContent("doclet.ClassUse_ConstructorArgsTypeParameters", classLink,
                pkgLink), constructorUseTableSummary, contentTree);
        constrSubWriter.addUseInfo(pkgToConstructorThrows.get(pkg),
                configuration.getContent("doclet.ClassUse_ConstructorThrows", classLink,
                pkgLink), constructorUseTableSummary, contentTree);
    }

    /**
     * Get the header for the class use Listing.
     *
     * @return a content tree representing the class use header
     */
    protected HtmlTree getClassUseHeader() {
        String cltype = configuration.getText(utils.isInterface(typeElement)
                ? "doclet.Interface"
                : "doclet.Class");
        String clname = utils.getFullyQualifiedName(typeElement);
        String title = configuration.getText("doclet.Window_ClassUse_Header",
                cltype, clname);
        HtmlTree bodyTree = getBody(true, getWindowTitle(title));
        HtmlTree htmlTree = (configuration.allowTag(HtmlTag.HEADER))
                ? HtmlTree.HEADER()
                : bodyTree;
        addTop(htmlTree);
        Content mdleLinkContent = getModuleLink(utils.elementUtils.getModuleOf(typeElement),
                contents.moduleLabel);
        navBar.setNavLinkModule(mdleLinkContent);
        Content classLinkContent = getLink(new LinkInfoImpl(
                configuration, LinkInfoImpl.Kind.CLASS_USE_HEADER, typeElement)
                .label(configuration.getText("doclet.Class")));
        navBar.setNavLinkClass(classLinkContent);
        navBar.setUserHeader(getUserHeaderFooter(true));
        htmlTree.add(navBar.getContent(true));
        if (configuration.allowTag(HtmlTag.HEADER)) {
            bodyTree.add(htmlTree);
        }
        ContentBuilder headContent = new ContentBuilder();
        headContent.add(contents.getContent("doclet.ClassUse_Title", cltype));
        headContent.add(new HtmlTree(HtmlTag.BR));
        headContent.add(clname);
        Content heading = HtmlTree.HEADING(HtmlConstants.CLASS_PAGE_HEADING,
                true, HtmlStyle.title, headContent);
        Content div = HtmlTree.DIV(HtmlStyle.header, heading);
        if (configuration.allowTag(HtmlTag.MAIN)) {
            mainTree.add(div);
        } else {
            bodyTree.add(div);
        }
        return bodyTree;
    }
}
