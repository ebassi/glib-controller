#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME=GLib-Controller
TEST_TYPE=-d
FILE=glib-controller

test ${TEST_TYPE} ${FILE} || {
        echo "You must run this script in the top-level ${PKG_NAME} directory"
        exit 1
}

which gnome-autogen.sh || {
        echo "*** You need to install gnome-common from GNOME Git:"
        echo "***  git clone git://git.gnome.org/gnome-common"
        exit 1
}

USE_GNOME2_MACROS=1 . gnome-autogen.sh
