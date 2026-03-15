# SPDX-License-Identifier: MIT
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([ruri],[3.12],[dabao1955@163.com])
AC_PROG_CC
AM_INIT_AUTOMAKE
AC_C_BIGENDIAN
AM_MAINTAINER_MODE
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(off_t)
AC_C_CONST
AC_C_INLINE
AC_FUNC_STAT
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_CHECK_SIZEOF(off_t, 4)
AC_PROG_INSTALL
AC_C_PROTOTYPES
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

dnl Give error and exit if we dont have pkgconfig
if test "x$PKG_CONFIG" = "x"; then
  AC_MSG_ERROR([you need to have pkgconfig installed!])
fi
# REQUIRE a standard (ANSI/ISO) C compiler
if test "$ac_cv_prog_cc_stdc" = no; then
	AC_MSG_ERROR([A standard (ANSI/ISO C89) C compiler is required.])
fi

AC_MSG_CHECKING([for commit id])
RURI_COMMIT_ID=`git rev-parse --short HEAD 2>/dev/null`
if test "$RURI_COMMIT_ID" = ""; then
    AC_MSG_RESULT([no])
    RURI_COMMIT_ID="null"
else
    AC_MSG_RESULT([$RURI_COMMIT_ID])
fi

AC_DEFINE_UNQUOTED([RURI_COMMIT_ID], ["$RURI_COMMIT_ID"], [The Git commit ID])

gl_cv_func_printf_directive_n=yes
gl_cv_func_printf_infinite_long_double=yes
gl_cv_func_printf_long_double=yes
gl_cv_func_snprintf_directive_n=yes
gl_cv_glob_lists_symlinks=yes

AC_CONFIG_SRCDIR([src/ruri.c])
AC_CONFIG_HEADERS([config.h])

# Check for programs
AC_PROG_MAKE_SET
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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

AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [Enable debug logging (defines RURI_DEBUG)])],
    [enable_debug=${enableval}],
    [enable_debug=no])

# Set default CFLAGS before AC_PROG_CC (autoconf.texi recommended idiom).
# Only applies when user has not pre-set CFLAGS via environment or command line.
AS_IF([test "x${enable_debug}" = "xyes"],
    [: ${CFLAGS="-O0 -g3"}],
    [: ${CFLAGS="-O2 -g0"}])

AC_PROG_CC
# PKG_PROG_PKG_CONFIG
AC_PROG_INSTALL

AC_CANONICAL_HOST
AS_CASE([${host_os}],
    [linux*], [],
    [AC_MSG_ERROR([ruri only supports Linux. Detected: ${host_os}])])

# coreonly: Highest priority, disable libcap/libseccomp/rurienv
AC_ARG_ENABLE([coreonly],
    [AS_HELP_STRING([--enable-coreonly],
        [Core only: auto-disables libcap, libseccomp, and rurienv])],
    [enable_coreonly=${enableval}],
    [enable_coreonly=no])

AC_ARG_ENABLE([libcap],
    [AS_HELP_STRING([--disable-libcap],
        [Disable libcap support (capability management)])],
    [enable_libcap=${enableval}],
    [enable_libcap=yes])

AC_ARG_ENABLE([libseccomp],
    [AS_HELP_STRING([--disable-libseccomp],
        [Disable libseccomp support (syscall filtering)])],
    [enable_libseccomp=${enableval}],
    [enable_libseccomp=yes])

AC_ARG_ENABLE([rurienv],
    [AS_HELP_STRING([--disable-rurienv],
        [Disable .rurienv config file support])],
    [enable_rurienv=${enableval}],
    [enable_rurienv=yes])

AS_IF([test "x${enable_coreonly}" = "xyes"], [
    enable_libcap=no
    enable_libseccomp=no
    enable_rurienv=no
])

AC_ARG_ENABLE([static],
    [AS_HELP_STRING([--enable-static],
        [Enable static linking (The corresponding static library is required.)])],
    [enable_static=${enableval}],
    [enable_static=no])

AC_ARG_ENABLE([static-pie],
    [AS_HELP_STRING([--enable-static-pie],
        [Enable static PIE linking (mutually exclusive with --enable-static)])],
    [enable_static_pie=${enableval}],
    [enable_static_pie=no])

AS_IF([test "x${enable_static}" = "xyes" && test "x${enable_static_pie}" = "xyes"], [
    AC_MSG_ERROR([--enable-static and --enable-static-pie are mutually exclusive])])

AS_IF([test "x${enable_static}" = "xyes" || test "x${enable_static_pie}" = "xyes"],
    [enable_dynamic=no],
    [enable_dynamic=yes])

AC_ARG_ENABLE([dev],
    [AS_HELP_STRING([--enable-dev],
        [Enable dev build: strict warnings + defines RURI_DEV])],
    [enable_dev=${enableval}],
    [enable_dev=no])

#  Flags/Link
AC_DEFUN([RURI_CHECK_CFLAG], [dnl
    AC_MSG_CHECKING([whether $CC accepts $1])
    save_CFLAGS="$CFLAGS"; CFLAGS="$CFLAGS $1"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes]); CFLAGS="$save_CFLAGS"; SECURITY_CFLAGS="$SECURITY_CFLAGS $1"],
        [AC_MSG_RESULT([no]);  CFLAGS="$save_CFLAGS"])])

AC_DEFUN([RURI_CHECK_CFLAG_LINK], [dnl
    AC_MSG_CHECKING([whether $CC accepts and links $1])
    save_CFLAGS="$CFLAGS"; CFLAGS="$CFLAGS $1"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes]); CFLAGS="$save_CFLAGS"; SECURITY_CFLAGS="$SECURITY_CFLAGS $1"],
        [AC_MSG_RESULT([no]);  CFLAGS="$save_CFLAGS"])])

AC_DEFUN([RURI_CHECK_LDFLAG], [dnl
    AC_MSG_CHECKING([whether linker accepts $1])
    save_LDFLAGS="$LDFLAGS"; LDFLAGS="$LDFLAGS $1"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes]); LDFLAGS="$save_LDFLAGS"; SECURITY_LDFLAGS="$SECURITY_LDFLAGS $1"],
        [AC_MSG_RESULT([no]);  LDFLAGS="$save_LDFLAGS"])])

AC_DEFUN([RURI_CHECK_PIE], [dnl
    AC_MSG_CHECKING([whether $CC and linker support -fPIE/-pie together])
    save_CFLAGS="$CFLAGS"; save_LDFLAGS="$LDFLAGS"
    CFLAGS="$CFLAGS -fPIE"; LDFLAGS="$LDFLAGS -pie"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
        [AC_MSG_RESULT([yes])
        CFLAGS="$save_CFLAGS"; LDFLAGS="$save_LDFLAGS"
        SECURITY_CFLAGS="$SECURITY_CFLAGS -fPIE"
        SECURITY_LDFLAGS="$SECURITY_LDFLAGS -pie"],
        [AC_MSG_RESULT([no])
        CFLAGS="$save_CFLAGS"; LDFLAGS="$save_LDFLAGS"])])

SECURITY_CFLAGS=""
SECURITY_LDFLAGS=""
RURI_CHECK_LDFLAG([-Wl,--build-id=sha1])
RURI_CHECK_LDFLAG([-Wl,-z,noexecstack])
RURI_CHECK_CFLAG([-pipe])

AS_IF([test "x${enable_debug}" = "xyes"], [
    RURI_CHECK_CFLAG([-Wall])
    RURI_CHECK_CFLAG([-Wextra])
    RURI_CHECK_CFLAG([-pedantic])
    RURI_CHECK_CFLAG([-Wstrict-prototypes])
    RURI_CHECK_CFLAG([-Wextra-semi-stmt])
    RURI_CHECK_CFLAG([-Wconversion])
    RURI_CHECK_CFLAG([-Wno-gnu-zero-variadic-macro-arguments])
    RURI_CHECK_CFLAG_LINK([-fsanitize=address,undefined])
    RURI_CHECK_CFLAG([-fno-sanitize-recover=all])
    RURI_CHECK_CFLAG([-fno-omit-frame-pointer])
    RURI_CHECK_CFLAG([-fno-stack-protector])
])

AS_IF([test "x${enable_debug}" = "xno"], [
    RURI_CHECK_CFLAG([-flto=auto])
    RURI_CHECK_CFLAG([-ftrivial-auto-var-init=pattern])
    RURI_CHECK_CFLAG([-fstack-protector-strong])
    RURI_CHECK_CFLAG([-fstack-clash-protection])
    RURI_CHECK_CFLAG([-fcf-protection=full])
    RURI_CHECK_CFLAG([-ffunction-sections])
    RURI_CHECK_CFLAG([-fdata-sections])
    RURI_CHECK_LDFLAG([-Wl,--gc-sections])
    RURI_CHECK_LDFLAG([-Wl,--strip-all])
    AS_IF([test "x${enable_dynamic}" = "xyes"], [
        RURI_CHECK_LDFLAG([-Wl,-z,relro])
        RURI_CHECK_LDFLAG([-Wl,-z,now])
        RURI_CHECK_PIE
    ])
    AS_IF([test "x${enable_static_pie}" = "xyes"], [
        RURI_CHECK_CFLAG([-fPIE])
        RURI_CHECK_LDFLAG([-static-pie])
    ])
    # RURI_CHECK_CFLAG([-Wno-unused-command-line-argument])
])
AS_IF([test "x${enable_dynamic}" = "xyes"], [
    RURI_CHECK_LDFLAG([-Wl,--disable-new-dtags])
])
AS_CASE([${host_cpu}],
    [x86_64 | i[[3-6]]86 | amd64], [
        RURI_CHECK_CFLAG([-mshstk])
    ],
    [aarch64 | arm64], [
        RURI_CHECK_CFLAG([-mbranch-protection=standard])
    ])

AC_SUBST([SECURITY_CFLAGS])
AC_SUBST([SECURITY_LDFLAGS])

AC_CHECK_HEADERS([pthread.h])
AC_SEARCH_LIBS([pthread_create], [pthread],
    [],
    [AC_MSG_ERROR([libpthread is required but not found])],
    [-pthread])

# libcap (optional)
AS_IF([test "x${enable_libcap}" = "xyes"], [
    AC_CHECK_LIB([cap], [cap_init], [
        AC_CHECK_HEADER([sys/capability.h], [
            have_libcap=yes
            CAP_LIBS="-lcap"
        ], [AC_MSG_ERROR([libcap found, but <sys/capability.h> header not found])])
    ], [AC_MSG_ERROR([libcap library not found])])
], [have_libcap=no])

# libseccomp (optional)
AS_IF([test "x${enable_libseccomp}" = "xyes"], [
    AC_CHECK_LIB([seccomp], [seccomp_init], [
        AC_CHECK_HEADER([seccomp.h], [
            have_libseccomp=yes
            SECCOMP_LIBS="-lseccomp"
        ], [AC_MSG_ERROR([libseccomp found, but <seccomp.h> header not found])])
    ], [AC_MSG_ERROR([libseccomp library not found])])
], [have_libseccomp=no])

AC_SUBST([CAP_LIBS])
AC_SUBST([SECCOMP_LIBS])

AM_CONDITIONAL([HAVE_LIBCAP],       [test "x${have_libcap}"       = "xyes"])
AM_CONDITIONAL([HAVE_LIBSECCOMP],   [test "x${have_libseccomp}"   = "xyes"])
AM_CONDITIONAL([ENABLE_COREONLY],   [test "x${enable_coreonly}"   = "xyes"])
AM_CONDITIONAL([ENABLE_RURIENV],    [test "x${enable_rurienv}"    = "xyes"])
AM_CONDITIONAL([ENABLE_STATIC],     [test "x${enable_static}"     = "xyes"])
AM_CONDITIONAL([ENABLE_DEBUG],      [test "x${enable_debug}"      = "xyes"])
AM_CONDITIONAL([ENABLE_DEV],        [test "x${enable_dev}"        = "xyes"])

if test "$CC" = "tcc"; then
    AC_DEFINE([__VERSION__], ["TinyCC"], ["1"])
    AC_DEFINE([__TIMESTAMP__], ["unknown"], ["1"])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
