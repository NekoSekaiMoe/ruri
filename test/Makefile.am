# SPDX-License-Identifier: MIT
# Automake file for ruri
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = ruri

# Source files
ruri_SOURCES = src/caplist.c src/chroot.c src/cprintf.c \
                src/info.c src/env.c src/seccomp.c \
                src/signal.c src/umount.c src/unshare.c \
                src/mount.c src/k2v.c src/elf-magic.c src/config.c \
                src/cgroup.c src/passwd.c src/ps.c  src/ruri.c

# Compiler and linker flags
AM_CFLAGS = \
    $(CFLAGS)				\
    -I$(top_builddir)/src/include           \
    -I$(top_srcdir)/src/include

TIDY_FLAGS = --checks=*,-clang-analyzer-security.insecureAPI.strcpy,-altera-unroll-loops,-cert-err33-c,-concurrency-mt-unsafe,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-readability-function-cognitive-complexity,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-bugprone-easily-swappable-parameters,-cert-err34-c,-misc-include-cleaner,-readability-identifier-length,-bugprone-signal-handler,-cert-msc54-cpp,-cert-sig30-c,-altera-id-dependent-backward-branch,-bugprone-suspicious-realloc-usage,-hicpp-signed-bitwise,-clang-analyzer-security.insecureAPI.UncheckedReturn --list-checks ${SOURCES} -- -lpthread -lseccomp -lcap -Wall -Wextra

DISTCLEANFILES = src/include/version.h
if !HAVE_LIBCAP
AM_CFLAGS += -DDISABLE_LIBCAP
endif
if !HAVE_LIBSECCOMP
AM_CFLAGS += -DDISABLE_LIBSECCOMP
endif
if !ENABLE_RURIENV
AM_CFLAGS += -DDISABLE_RURIENV
endif
if ENABLE_COREONLY
AM_CFLAGS += -DRURI_CORE_ONLY
endif
if ENABLE_DEBUG
AM_CFLAGS += -DRURI_DEBUG
endif
if !ENABLE_DEBUG
AM_CFLAGS += -D_FORTIFY_SOURCE=2
install: strip_target upx_target
endif
if ENABLE_DEV
AM_CFLAGS += -DRURI_DEV
endif
AM_CFLAGS += $(COMMIT_CFLAGS)

ruri_CFLAGS = -std=gnu11 -D_FILE_OFFSET_BITS=64
if !ENABLE_DEBUG
ruri_CFLAGS += -Wno-unused-result
endif
ruri_CFLAGS += $(SECURITY_CFLAGS)

ruri_LDFLAGS =
if ENABLE_STATIC
ruri_LDFLAGS += -static
endif
ruri_LDFLAGS += $(SECURITY_LDFLAGS)

# link library
# pthread Hard dependency (easteregg/nekofeng.c is unconditionally used)
ruri_LDADD = $(LIBS)
if HAVE_LIBCAP
ruri_LDADD += $(CAP_LIBS)
endif
if HAVE_LIBSECCOMP
ruri_LDADD += $(SECCOMP_LIBS)
endif

if ENABLE_STRIP
strip_target:
	$(STRIP) $(bin_PROGRAMS)
else
strip_target:
	@echo "strip not found, skipping stripping."
endif

if ENABLE_UPX
upx_target:
	$(UPX) --best --lzma $(bin_PROGRAMS)
else
upx_target:
	$(AM_V)echo "UPX not found, skipping compression."
endif

# Custom commands
.PHONY: ruritest check

check:
	$(AM_V)clang-tidy $(TIDY_FLAGS) $(ruri_SOURCES) || true

format:
	shfmt -i 4 -w test/*.sh
	shfmt -i 4 -w test/root/*.sh
	shfmt -i 4 -w configure
	shfmt -i 4 -w get-ruri.sh
	chmod 777 test/*.sh
	chmod 777 test/root/*
	sed -i "s/U '/U'/g" src/easteregg/*.c
	sed -i "s/U \"/U\"/g" src/easteregg/*.c

testuite:
	$(AM_V)$(MAKE) -C test -f Makefile.test test
