#!/bin/sh

### Script to build a win32 installation

./configure \
  $PKGCFG \
  $CROSSCFG \
  --with-ptw32=/opt/mxe/usr/i686-pc-mingw32 \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre" \
  XMLRPC_C_CONFIG=$PREFIX/bin/xmlrpc-c-config \
  FLTK_CONFIG=$PREFIX/bin/i686-pc-mingw32-fltk-config

make

$PREFIX/bin/i686-pc-mingw32-strip src/kcat.exe
make nsisinst
mv src/*setup*exe .


