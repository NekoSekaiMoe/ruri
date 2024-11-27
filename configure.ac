# SPDX-License-Identifier: MIT
# Process this file with autoconf to produce a configure script.

AC_INIT([ruri], [3.7], [moe-hacker@crack.moe])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Check for programs
AC_PROG_CC
AC_PROG_MAKE_SET
AC_CHECK_TOOL([STRIP], [strip], [:])

# Check for required headers
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h sys/mount.h sys/socket.h unistd.h])
AC_CHECK_HEADERS([sys/capability.h seccomp.h pthread.h], [], AC_MSG_ERROR([Missing required headers]))

# Check for libraries
AC_SEARCH_LIBS([cap_init], [cap], [], AC_MSG_ERROR([libcap is required]))
AC_SEARCH_LIBS([seccomp_init], [seccomp], [], AC_MSG_ERROR([libseccomp is required]))
AC_SEARCH_LIBS([pthread_create], [pthread], [], AC_MSG_ERROR([pthread is required]))

# Enable optional flags
CFLAGS="$CFLAGS -ftrivial-auto-var-init=pattern -fcf-protection=full -flto=auto -fPIE -pie -fstack-protector-all"
CFLAGS="$CFLAGS -fstack-clash-protection -mshstk -Wno-unused-result -O2"
CFLAGS="$CFLAGS -ffunction-sections -fdata-sections -Wl,--gc-sections"
LDFLAGS="$LDFLAGS -Wl,-z,relro -Wl,-z,noexecstack -Wl,-z,now"

# Handle optional flags for debug and static builds
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable debug build]),
    [if test "x$enable_debug" = "xyes"; then CFLAGS="$CFLAGS -g -O0 -DDEBUG_BUILD"; fi])
AC_ARG_ENABLE([static],
    AS_HELP_STRING([--enable-static], [Enable static linking]),
    [if test "x$enable_static" = "xyes"; then LDFLAGS="$LDFLAGS -static"; fi])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

