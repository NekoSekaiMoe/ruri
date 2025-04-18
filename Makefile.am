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
AM_CFLAGS = $(CFLAGS) -I.
AM_LDFLAGS = $(LINKER_FLAGS)

TIDY_FLAGS = --checks=*,-clang-analyzer-security.insecureAPI.strcpy,-altera-unroll-loops,-cert-err33-c,-concurrency-mt-unsafe,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-readability-function-cognitive-complexity,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-bugprone-easily-swappable-parameters,-cert-err34-c,-misc-include-cleaner,-readability-identifier-length,-bugprone-signal-handler,-cert-msc54-cpp,-cert-sig30-c,-altera-id-dependent-backward-branch,-bugprone-suspicious-realloc-usage,-hicpp-signed-bitwise,-clang-analyzer-security.insecureAPI.UncheckedReturn --list-checks ${SOURCES} -- -lpthread -lseccomp -lcap -Wall -Wextra

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

if !ENABLE_DEBUG
install-data-hook: strip_target upx_target
endif

#Check VERBOSE vaule
AM_V = $(if $(VERBOSE),, @)

AM_V_CC = $(AM_V)echo "  CC       " $@;

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
