#!/bin/bash
PS4='+ [DEBUG] ${BASH_SOURCE}:${LINENO}: ' && export PS4
set -e

bash test-root.sh | tee -a log.txt
