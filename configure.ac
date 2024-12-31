# SPDX-License-Identifier: MIT
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([ruri], [3.8], [moe-hacker@crack.moe])
AC_PROG_CC
AM_INIT_AUTOMAKE

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
AC_PROG_RANLIB
AM_PROG_AR
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
AC_CHECK_HEADERS([time.h grp.h fcntl.h sys/ioctl.h sys/mount.h sys/socket.h linux/fs.h linux/version.h linux/sched.h])
AC_CHECK_HEADERS([sys/capability.h seccomp.h pthread.h], [], AC_MSG_ERROR([Missing required headers]))

# Check for libraries
AC_SEARCH_LIBS([cap_init], [cap], [], AC_MSG_ERROR([libcap is required]))
AC_SEARCH_LIBS([seccomp_init], [seccomp], [], AC_MSG_ERROR([libseccomp is required]))
AC_SEARCH_LIBS([pthread_create], [pthread], [], AC_MSG_ERROR([pthread is required]))

# Enable optional flags
CFLAGS_LIST="-flto=auto -fPIE -pie -fstack-protector-all -fdata-sections -fno-omit-frame-pointer -fno-stack-protector -ftrivial-auto-var-init=pattern -fstack-clash-protection -Wno-unused-result -mshstk -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all -Wl,--disable-new-dtags -Wl,--build-id=sha1 -Wl,-z,norelro -Wl,-z,execstack -Wall -Wextra -Wconversion -pedantic -pipe"
LDFLAGS="$LDFLAGS -Wl,-z,relro -Wl,-z,noexecstack -Wl,-z,now"

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
for flag in $CFLAGS_LIST; do
    AC_CHECK_CFLAGS([$flag])
done

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

