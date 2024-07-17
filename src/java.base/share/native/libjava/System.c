/*
 * Copyright (c) 1994, 2024, Oracle and/or its affiliates. All rights reserved.
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

#include <string.h>

#include "jni.h"
#include "jni_util.h"
#include "jvm.h"
#include "java_props.h"

#include "java_lang_System.h"

#define OBJ "Ljava/lang/Object;"

/* Only register the performance-critical methods */
static JNINativeMethod methods[] = {
    {"currentTimeMillis", "()J",              (void *)&JVM_CurrentTimeMillis},
    {"nanoTime",          "()J",              (void *)&JVM_NanoTime},
    {"arraycopy",     "(" OBJ "I" OBJ "II)V", (void *)&JVM_ArrayCopy},
};

#undef OBJ

JNIEXPORT void JNICALL
Java_java_lang_System_registerNatives(JNIEnv *env, jclass cls)
{
    (*env)->RegisterNatives(env, cls,
                            methods, sizeof(methods)/sizeof(methods[0]));
}

JNIEXPORT jint JNICALL
Java_java_lang_System_identityHashCode(JNIEnv *env, jobject this, jobject x)
{
    return JVM_IHashCode(env, x);
}

/* VENDOR, VENDOR_URL, VENDOR_URL_BUG are set in VersionProps.java.template. */

#define PUTPROP(props, key, val)                                     \
    if (1) {                                                         \
        jstring jkey, jval;                                          \
        jobject r;                                                   \
        jkey = (*env)->NewStringUTF(env, key);                       \
        if (jkey == NULL) return NULL;                               \
        jval = (*env)->NewStringUTF(env, val);                       \
        if (jval == NULL) return NULL;                               \
        r = (*env)->CallObjectMethod(env, props, putID, jkey, jval); \
        if ((*env)->ExceptionOccurred(env)) return NULL;             \
        (*env)->DeleteLocalRef(env, jkey);                           \
        (*env)->DeleteLocalRef(env, jval);                           \
        (*env)->DeleteLocalRef(env, r);                              \
    } else ((void) 0)

/*  "key" is a char type string with only ASCII character in it.
    "val" is a nchar (typedefed in java_props.h) type string  */

#define PUTPROP_ForPlatformNString(props, key, val)                  \
    if (1) {                                                         \
        jstring jkey, jval;                                          \
        jobject r;                                                   \
        jkey = (*env)->NewStringUTF(env, key);                       \
        if (jkey == NULL) return NULL;                               \
        jval = GetStringPlatform(env, val);                          \
        if (jval == NULL) return NULL;                               \
        r = (*env)->CallObjectMethod(env, props, putID, jkey, jval); \
        if ((*env)->ExceptionOccurred(env)) return NULL;             \
        (*env)->DeleteLocalRef(env, jkey);                           \
        (*env)->DeleteLocalRef(env, jval);                           \
        (*env)->DeleteLocalRef(env, r);                              \
    } else ((void) 0)
#define REMOVEPROP(props, key)                                    \
    if (1) {                                                      \
        jstring jkey;                                             \
        jobject r;                                                \
        jkey = JNU_NewStringPlatform(env, key);                   \
        if (jkey == NULL) return NULL;                            \
        r = (*env)->CallObjectMethod(env, props, removeID, jkey); \
        if ((*env)->ExceptionOccurred(env)) return NULL;          \
        (*env)->DeleteLocalRef(env, jkey);                        \
        (*env)->DeleteLocalRef(env, r);                           \
    } else ((void) 0)
#define GETPROP(props, key, jret)                                     \
    if (1) {                                                          \
        jstring jkey = JNU_NewStringPlatform(env, key);               \
        if (jkey == NULL) return NULL;                                \
        jret = (*env)->CallObjectMethod(env, props, getPropID, jkey); \
        if ((*env)->ExceptionOccurred(env)) return NULL;              \
        (*env)->DeleteLocalRef(env, jkey);                            \
    } else ((void) 0)

#ifdef JAVA_SPECIFICATION_VENDOR /* Third party may NOT overwrite this. */
  #error "ERROR: No override of JAVA_SPECIFICATION_VENDOR is allowed"
#else
  #define JAVA_SPECIFICATION_VENDOR "Oracle Corporation"
#endif

jobject fillI18nProps(JNIEnv *env, jobject props, char *baseKey,
                      char *platformDispVal, char *platformFmtVal,
                      jmethodID putID, jmethodID getPropID) {
    jstring jVMBaseVal = NULL;

    GETPROP(props, baseKey, jVMBaseVal);
    if (jVMBaseVal) {
        // user specified the base property.  there's nothing to do here.
        (*env)->DeleteLocalRef(env, jVMBaseVal);
    } else {
        char buf[64];
        jstring jVMVal = NULL;
        const char *baseVal = "";

        /* user.xxx base property */
        if (platformDispVal) {
            PUTPROP(props, baseKey, platformDispVal);
            baseVal = platformDispVal;
        }

        /* user.xxx.display property */
        jio_snprintf(buf, sizeof(buf), "%s.display", baseKey);
        GETPROP(props, buf, jVMVal);
        if (jVMVal == NULL) {
            if (platformDispVal && (strcmp(baseVal, platformDispVal) != 0)) {
                PUTPROP(props, buf, platformDispVal);
            }
        } else {
            (*env)->DeleteLocalRef(env, jVMVal);
        }

        /* user.xxx.format property */
        jio_snprintf(buf, sizeof(buf), "%s.format", baseKey);
        GETPROP(props, buf, jVMVal);
        if (jVMVal == NULL) {
            if (platformFmtVal && (strcmp(baseVal, platformFmtVal) != 0)) {
                PUTPROP(props, buf, platformFmtVal);
            }
        } else {
            (*env)->DeleteLocalRef(env, jVMVal);
        }
    }

    return NULL;
}

JNIEXPORT jobject JNICALL
Java_java_lang_System_initProperties(JNIEnv *env, jclass cla, jobject props)
{
    char buf[128];
    java_props_t *sprops;
    jmethodID putID, removeID, getPropID;
    jobject ret = NULL;
    jstring jVMVal = NULL;

    if ((*env)->EnsureLocalCapacity(env, 50) < 0) {
        return NULL;
    }
    sprops = GetJavaProperties(env);
    CHECK_NULL_RETURN(sprops, NULL);

    putID = (*env)->GetMethodID(env,
                                (*env)->GetObjectClass(env, props),
                                "put",
            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    CHECK_NULL_RETURN(putID, NULL);

    removeID = (*env)->GetMethodID(env,
                                   (*env)->GetObjectClass(env, props),
                                   "remove",
            "(Ljava/lang/Object;)Ljava/lang/Object;");
    CHECK_NULL_RETURN(removeID, NULL);

    getPropID = (*env)->GetMethodID(env,
                                    (*env)->GetObjectClass(env, props),
                                    "getProperty",
            "(Ljava/lang/String;)Ljava/lang/String;");
    CHECK_NULL_RETURN(getPropID, NULL);

    PUTPROP(props, "java.specification.version",
            VERSION_SPECIFICATION);
    PUTPROP(props, "java.specification.maintenance.version",
            "3");
    PUTPROP(props, "java.specification.name",
            "Java Platform API Specification");
    PUTPROP(props, "java.specification.vendor",
            JAVA_SPECIFICATION_VENDOR);

    jio_snprintf(buf, sizeof(buf), "%d.%d", JVM_CLASSFILE_MAJOR_VERSION,
                                            JVM_CLASSFILE_MINOR_VERSION);
    PUTPROP(props, "java.class.version", buf);

    if (sprops->awt_toolkit) {
        PUTPROP(props, "awt.toolkit", sprops->awt_toolkit);
    }
#ifdef MACOSX
    if (sprops->awt_headless) {
        PUTPROP(props, "java.awt.headless", sprops->awt_headless);
    }
#endif

    /* os properties */
    PUTPROP(props, "os.name", sprops->os_name);
    PUTPROP(props, "os.version", sprops->os_version);
    PUTPROP(props, "os.arch", sprops->os_arch);

#ifdef JDK_ARCH_ABI_PROP_NAME
    PUTPROP(props, "sun.arch.abi", sprops->sun_arch_abi);
#endif

    /* file system properties */
    PUTPROP(props, "file.separator", sprops->file_separator);
    PUTPROP(props, "path.separator", sprops->path_separator);
    PUTPROP(props, "line.separator", sprops->line_separator);

    /*
     * file encoding for stdout and stderr
     */
    if (sprops->sun_stdout_encoding != NULL) {
        PUTPROP(props, "sun.stdout.encoding", sprops->sun_stdout_encoding);
    }
    if (sprops->sun_stderr_encoding != NULL) {
        PUTPROP(props, "sun.stderr.encoding", sprops->sun_stderr_encoding);
    }

    /* unicode_encoding specifies the default endianness */
    PUTPROP(props, "sun.io.unicode.encoding", sprops->unicode_encoding);
    PUTPROP(props, "sun.cpu.isalist",
            (sprops->cpu_isalist ? sprops->cpu_isalist : ""));
    PUTPROP(props, "sun.cpu.endian",  sprops->cpu_endian);


#ifdef MACOSX
    /* Proxy setting properties */
    if (sprops->httpProxyEnabled) {
        PUTPROP(props, "http.proxyHost", sprops->httpHost);
        PUTPROP(props, "http.proxyPort", sprops->httpPort);
    }

    if (sprops->httpsProxyEnabled) {
        PUTPROP(props, "https.proxyHost", sprops->httpsHost);
        PUTPROP(props, "https.proxyPort", sprops->httpsPort);
    }

    if (sprops->ftpProxyEnabled) {
        PUTPROP(props, "ftp.proxyHost", sprops->ftpHost);
        PUTPROP(props, "ftp.proxyPort", sprops->ftpPort);
    }

    if (sprops->socksProxyEnabled) {
        PUTPROP(props, "socksProxyHost", sprops->socksHost);
        PUTPROP(props, "socksProxyPort", sprops->socksPort);
    }

    if (sprops->gopherProxyEnabled) {
        // The gopher client is different in that it expects an 'is this set?' flag that the others don't.
        PUTPROP(props, "gopherProxySet", "true");
        PUTPROP(props, "gopherProxyHost", sprops->gopherHost);
        PUTPROP(props, "gopherProxyPort", sprops->gopherPort);
    } else {
        PUTPROP(props, "gopherProxySet", "false");
    }

    // Mac OS X only has a single proxy exception list which applies
    // to all protocols
    if (sprops->exceptionList) {
        PUTPROP(props, "http.nonProxyHosts", sprops->exceptionList);
        PUTPROP(props, "ftp.nonProxyHosts", sprops->exceptionList);
        PUTPROP(props, "socksNonProxyHosts", sprops->exceptionList);
    }
#endif

    /* !!! DO NOT call PUTPROP_ForPlatformNString before this line !!!
     * !!! The platform native encoding for strings has not been set up yet !!!
     */
    InitializeEncoding(env, sprops->sun_jnu_encoding);

    /* Printing properties */
    /* Note: java.awt.printerjob is an implementation private property which
     * just happens to have a java.* name because it is referenced in
     * a java.awt class. It is the mechanism by which the implementation
     * finds the appropriate class in the JRE for the platform.
     * It is explicitly not designed to be overridden by clients as
     * a way of replacing the implementation class, and in any case
     * the mechanism by which the class is loaded is constrained to only
     * find and load classes that are part of the JRE.
     * This property may be removed if that mechanism is redesigned
     */
    PUTPROP(props, "java.awt.printerjob", sprops->printerJob);

    /* data model */
    if (sizeof(sprops) == 4) {
        sprops->data_model = "32";
    } else if (sizeof(sprops) == 8) {
        sprops->data_model = "64";
    } else {
        sprops->data_model = "unknown";
    }
    PUTPROP(props, "sun.arch.data.model",  \
                    sprops->data_model);

    /* patch level */
    PUTPROP(props, "sun.os.patch.level",  \
                    sprops->patch_level);

    /* Java2D properties */
    /* Note: java.awt.graphicsenv is an implementation private property which
     * just happens to have a java.* name because it is referenced in
     * a java.awt class. It is the mechanism by which the implementation
     * finds the appropriate class in the JRE for the platform.
     * It is explicitly not designed to be overridden by clients as
     * a way of replacing the implementation class, and in any case
     * the mechanism by which the class is loaded is constrained to only
     * find and load classes that are part of the JRE.
     * This property may be removed if that mechanism is redesigned
     */
    PUTPROP(props, "java.awt.graphicsenv", sprops->graphics_env);
    if (sprops->font_dir != NULL) {
        PUTPROP_ForPlatformNString(props,
                                   "sun.java2d.fontpath", sprops->font_dir);
    }

    PUTPROP_ForPlatformNString(props, "java.io.tmpdir", sprops->tmp_dir);

    PUTPROP_ForPlatformNString(props, "user.name", sprops->user_name);
    PUTPROP_ForPlatformNString(props, "user.home", sprops->user_home);

    PUTPROP(props, "user.timezone", sprops->timezone);

    PUTPROP_ForPlatformNString(props, "user.dir", sprops->user_dir);

    /* This is a sun. property as it is currently only set for Gnome and
     * Windows desktops.
     */
    if (sprops->desktop != NULL) {
        PUTPROP(props, "sun.desktop", sprops->desktop);
    }

    ret = JVM_InitProperties(env, props);

    /* reconstruct i18n related properties */
    fillI18nProps(env, props, "user.language", sprops->display_language,
        sprops->format_language, putID, getPropID);
    fillI18nProps(env, props, "user.script",
        sprops->display_script, sprops->format_script, putID, getPropID);
    fillI18nProps(env, props, "user.country",
        sprops->display_country, sprops->format_country, putID, getPropID);
    fillI18nProps(env, props, "user.variant",
        sprops->display_variant, sprops->format_variant, putID, getPropID);
    GETPROP(props, "file.encoding", jVMVal);
    if (jVMVal == NULL) {
#ifdef MACOSX
        /*
         * Since sun_jnu_encoding is now hard-coded to UTF-8 on Mac, we don't
         * want to use it to overwrite file.encoding
         */
        PUTPROP(props, "file.encoding", sprops->encoding);
#else
        PUTPROP(props, "file.encoding", sprops->sun_jnu_encoding);
#endif
    } else {
        (*env)->DeleteLocalRef(env, jVMVal);
    }

    // Platform defined encoding properties override any on the command line
    PUTPROP(props, "sun.jnu.encoding", sprops->sun_jnu_encoding);

    return ret;
}

/*
 * The following three functions implement setter methods for
 * java.lang.System.{in, out, err}. They are natively implemented
 * because they violate the semantics of the language (i.e. set final
 * variable).
 */
JNIEXPORT void JNICALL
Java_java_lang_System_setIn0(JNIEnv *env, jclass cla, jobject stream)
{
    jfieldID fid =
        (*env)->GetStaticFieldID(env,cla,"in","Ljava/io/InputStream;");
    if (fid == 0)
        return;
    (*env)->SetStaticObjectField(env,cla,fid,stream);
}

JNIEXPORT void JNICALL
Java_java_lang_System_setOut0(JNIEnv *env, jclass cla, jobject stream)
{
    jfieldID fid =
        (*env)->GetStaticFieldID(env,cla,"out","Ljava/io/PrintStream;");
    if (fid == 0)
        return;
    (*env)->SetStaticObjectField(env,cla,fid,stream);
}

JNIEXPORT void JNICALL
Java_java_lang_System_setErr0(JNIEnv *env, jclass cla, jobject stream)
{
    jfieldID fid =
        (*env)->GetStaticFieldID(env,cla,"err","Ljava/io/PrintStream;");
    if (fid == 0)
        return;
    (*env)->SetStaticObjectField(env,cla,fid,stream);
}

static void cpchars(jchar *dst, char *src, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

JNIEXPORT jstring JNICALL
Java_java_lang_System_mapLibraryName(JNIEnv *env, jclass ign, jstring libname)
{
    int len;
    int prefix_len = (int) strlen(JNI_LIB_PREFIX);
    int suffix_len = (int) strlen(JNI_LIB_SUFFIX);

    jchar chars[256];
    if (libname == NULL) {
        JNU_ThrowNullPointerException(env, 0);
        return NULL;
    }
    len = (*env)->GetStringLength(env, libname);
    if (len > 240) {
        JNU_ThrowIllegalArgumentException(env, "name too long");
        return NULL;
    }
    cpchars(chars, JNI_LIB_PREFIX, prefix_len);
    (*env)->GetStringRegion(env, libname, 0, len, chars + prefix_len);
    len += prefix_len;
    cpchars(chars + len, JNI_LIB_SUFFIX, suffix_len);
    len += suffix_len;

    return (*env)->NewString(env, chars, len);
}
