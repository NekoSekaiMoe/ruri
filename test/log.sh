#!/bin/bash
PS4='+ [DEBUG] ${BASH_SOURCE}:${LINENO}: ' && export PS4
set -e

strace -f -e trace=write bash -x test-root.sh | tee -a log.txt
