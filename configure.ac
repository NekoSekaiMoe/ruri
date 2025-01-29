# SPDX-License-Identifier: MIT
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([ruri], [3.8], [moe-hacker@crack.moe])
AC_PROG_CC
AM_INIT_AUTOMAKE
m4_pattern_allow([AC_CHECK_LDFLAGS])
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_LN_S
AC_SEARCH_LIBS([strerror], [cposix])
AC_PROG_MAKE_SET
AC_SYS_LARGEFILE
PKG_PROG_PKG_CONFIG
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T
AC_PROG_LN_S
CC_DYNAMIC_INITIALIZERS
gl_cv_func_printf_directive_n=yes
gl_cv_func_printf_infinite_long_double=yes
gl_cv_func_printf_long_double=yes
gl_cv_func_snprintf_directive_n=yes
gl_cv_glob_lists_symlinks=yes
AC_SEARCH_LIBS([strerror], [cposix])
AM_MAINTAINER_MODE([enable])

dnl Internationalization macros.

AM_CONDITIONAL(USE_NLS, test x$USE_NLS = xyes)

# Define AC_CHECK_CFLAGS macro
AC_DEFUN([AC_CHECK_CFLAGS], [
    AC_MSG_CHECKING([whether $1 is supported])
    save_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $1"
    AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no]); CFLAGS=$save_CFLAGS]
    )
])

AC_CONFIG_SRCDIR([src/ruri.c])
AC_CONFIG_HEADERS([config.h])

# Check for programs
AC_PROG_MAKE_SET
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
# Initialize supported flags variables
SUPPORTED_CFLAGS=""
SUPPORTED_LDFLAGS=""

# Check strip and upx
AC_CHECK_TOOL([STRIP], [strip], [no])
AC_CHECK_PROG([UPX], [upx], [upx], [no])
AM_CONDITIONAL([ENABLE_STRIP], [test "x$STRIP" != "xno"])
AM_CONDITIONAL([ENABLE_UPX], [test "x$UPX" != "xno"])

if test "x$STRIP" != "xno"; then
    AC_MSG_NOTICE([STRIP detected, enabling binary stripping.])
else
    AC_MSG_NOTICE([STRIP not found, skipping binary stripping.])
fi

if test "x$UPX" != "xno"; then
    AC_MSG_NOTICE([UPX detected, enabling binary compression.])
else
    AC_MSG_NOTICE([UPX not found, skipping binary compression.])
fi

# Check for required headers
AC_CHECK_HEADERS([time.h grp.h fcntl.h sys/ioctl.h sys/mount.h sys/socket.h sys/wait.h linux/fs.h linux/version.h linux/sched.h], [], AC_MSG_ERROR([Missing required headers in standard C library.]))
AC_CHECK_HEADERS([sys/capability.h seccomp.h pthread.h], [], AC_MSG_ERROR([Missing required headers for libraries.]))

# Check for libraries
AC_SEARCH_LIBS([cap_init], [cap], [], AC_MSG_ERROR([libcap is required]))
AC_SEARCH_LIBS([seccomp_init], [seccomp], [], AC_MSG_ERROR([libseccomp is required]))
AC_SEARCH_LIBS([pthread_create], [pthread], [], AC_MSG_ERROR([pthread is required]))

# Enable optional flags
CFLAGS_LIST="-flto=auto -fPIE -pie -fstack-protector-all -fdata-sections -fno-omit-frame-pointer -fno-stack-protector -ftrivial-auto-var-init=pattern -fstack-clash-protection -Wno-unused-result -mshstk -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all -Wl,--disable-new-dtags -Wl,--build-id=sha1 -Wl,-z,norelro -Wl,-z,execstack -Wall -Wextra -Wconversion -pedantic -pipe"
LDFLAGS_LIST="-pipe -Wl,-z,relro -Wl,-z,noexecstack -Wl,-z,now"

# Handle optional flags for debug and static builds
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable debug build]),
    [if test "x$enable_debug" = "xyes"; then CFLAGS="$CFLAGS  -g3 -O0 -DDEBUG_BUILD -DRURI_DEBUG -DRURI_DEV" && AC_MSG_NOTICE([Warning: DEBUG mode is enabled.]) && CFLAGS_LIST="$CFLAGS_LIST -g3 -O0 -DDEBUG_BUILD -DRURI_DEBUG -DRURI_DEV"; else CFLAGS="$CFLAGS -O3 -g0 -DNDEBUG" && CFLAGS_LIST="$CFLAGS_LIST -O3 -g0 -DNDEBUG"; fi])
    AM_CONDITIONAL([ENABLE_DEBUG], [test "x$enable_debug" = "xyes"])
AC_ARG_ENABLE([static],
    AS_HELP_STRING([--enable-static], [Enable static linking]),
    [if test "x$enable_static" = "xyes"; then LDFLAGS="$LDFLAGS -static"; fi])

# Check CFLAGS
AC_MSG_CHECKING([whether the following CFLAGS are supported])
# Check compiler flags
AC_MSG_NOTICE([Checking compiler flag support...])
for flag in $CFLAGS_LIST; do
    save_CFLAGS="$CFLAGS"
    AC_MSG_CHECKING([whether $CC supports $flag])
    CFLAGS="$CFLAGS $flag"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        [AC_MSG_RESULT([yes])
         SUPPORTED_CFLAGS="$SUPPORTED_CFLAGS $flag"],
        [AC_MSG_RESULT([no])
         CFLAGS="$save_CFLAGS"])
done

# Check linker flags
AC_MSG_NOTICE([Checking linker flag support...])
for flag in $LDFLAGS_LIST; do
    save_LDFLAGS="$LDFLAGS"
    AC_MSG_CHECKING([whether the linker accepts $flag])
    LDFLAGS="$LDFLAGS $flag"
    AC_LINK_IFELSE([AC_LANG_PROGRAM()],
        [AC_MSG_RESULT([yes])
         SUPPORTED_LDFLAGS="$SUPPORTED_LDFLAGS $flag"],
        [AC_MSG_RESULT([no])
         LDFLAGS="$save_LDFLAGS"])
done

# Apply supported flags
CFLAGS="$CFLAGS $SUPPORTED_CFLAGS"
LDFLAGS="$LDFLAGS $SUPPORTED_LDFLAGS"

if test "$CC" = "tcc"; then
    AC_DEFINE([__VERSION__], [1], [TinyCC])
    AC_DEFINE([__TIMESTAMP__], [1], [unknown])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

