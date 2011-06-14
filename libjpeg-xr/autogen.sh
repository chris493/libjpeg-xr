#!/bin/sh
set -x
rm aclocal.m4
libtoolize --force --copy
aclocal -I ./m4
autoheader
automake --foreign --add-missing --force-missing --copy
autoconf

