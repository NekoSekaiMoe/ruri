# SPDX-License-Identifier: MIT
# Automake file for ruri
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = ruri

# Source files
ruri_SOURCES = src/main.c src/caplist.c src/chroot.c src/cprintf.c \
                src/info.c src/rurienv.c src/rurifetch.c src/seccomp.c \
                src/signal.c src/umount.c src/unshare.c src/rootless.c \
                src/mount.c src/k2v.c src/elf-magic.c src/config.c \
                src/cgroup.c src/passwd.c src/ps.c src/easteregg/action.c \
                src/easteregg/nekofeng.c src/easteregg/layer.c \
                src/easteregg/typewriter.c

# Compiler and linker flags
AM_CFLAGS = -Wall -Wextra -pedantic -Wconversion -Wno-unused-result -ffunction-sections -fdata-sections -fstack-protector-all  -D_FORTIFY_SOURCE=3
# Include additional directories
AM_CFLAGS = -I$(srcdir)/src/include -I$(srcdir)/src/easteregg/include

AM_LDFLAGS = -Wl,-z,relro -Wl,-z,noexecstack -Wl,-z,now
AM_LDFLAGS += -Wl,--gc-sections -lcap -lseccomp -lpthread

# Custom commands
format:
	clang-format -i $(ruri_SOURCES)

check:
	clang-tidy $(ruri_SOURCES)

