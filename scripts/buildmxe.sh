#!/bin/sh

### Script to build a win32 installation

./configure \
  $PKGCFG \
  --host=i686-w64-mingw32.static \
  --with-ptw32=$PREFIX/i686-w64-mingw32.static \
  --with-libiconv-prefix=$PREFIX/iconv \
  --enable-static \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre" \
  FLTK_CONFIG=$PREFIX/i686-w64-mingw32.static/bin/fltk-config

make -j 8

$PREFIX/bin/i686-w64-mingw32.static-strip src/kcat.exe
make nsisinst
mv src/*setup*exe .
