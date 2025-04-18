#!/bin/sh

PS4='[$(date)] [DEBUG] ${BASH_SOURCE}:${LINENO}: ' && export PS4

set -eu

rm -rfd configure configure~ config.* Makefile Makefile.in compile* missing* ltmain.sh* install-sh* 

echo
echo Running autogen script...
echo

## Check all dependencies are present
MISSING=""

# Check for aclocal
env aclocal --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
  ACLOCAL=aclocal
else
  MISSING="$MISSING aclocal"
fi

# Check for autoconf
env autoconf --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
  AUTOCONF=autoconf
else
  MISSING="$MISSING autoconf"
fi

# Check for autoheader
env autoheader --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
  AUTOHEADER=autoheader
else
  MISSING="$MISSING autoheader"
fi

# Check for automake
env automake --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
  AUTOMAKE=automake
else
  MISSING="$MISSING automake"
fi

# Check for libtoolize or glibtoolize
env libtoolize --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
  # libtoolize was found, so use it
  TOOL=libtoolize
else
  # libtoolize wasn't found, so check for glibtoolize
  env glibtoolize --version > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    TOOL=glibtoolize
  else
    MISSING="$MISSING libtoolize/glibtoolize"
  fi
fi

# Check for tar
env tar -cf /dev/null /dev/null > /dev/null 2>&1
if [ $? -ne 0 ]; then
  MISSING="$MISSING tar"
fi

env perl -V > /dev/null 2>&1
if [ $? -eq 0 ]; then
  PERL=perl
else
  MISSING="$MISSING perl"
fi

## If dependencies are missing, warn the user and abort
if [ "x$MISSING" != "x" ]; then
  echo "Aborting."
  echo
  echo "The following build tools are missing:"
  echo
  for pkg in $MISSING; do
    echo "  * $pkg"
  done
  echo
  echo "Please install them and try again."
  echo
  exit 1
fi

## Do the autogeneration
echo Running ${ACLOCAL}...
$ACLOCAL 
echo Running ${AUTOHEADER}...
$AUTOHEADER
echo Running ${TOOL}...
$TOOL --automake --copy --force
echo Running ${AUTOCONF}...
$AUTOCONF
echo Running ${AUTOMAKE}...
$AUTOMAKE --add-missing --force-missing --copy --foreign

perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" configure
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" missing
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" config.sub
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" config.guess
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" ltmain.sh
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" depcomp
perl -pi -e "s/^PS4='\+ '\$/PS4='[\\\$\(date \"+%Y年 %m月 %d日 %A %H:%M:%S %Z\"\)] [DEBUG] \\\${BASH_SOURCE}:\\\${LINENO}: '/" compile

# Instruct user on next steps
echo
echo "Generuated configure."
rm -rf autom4te.cache
