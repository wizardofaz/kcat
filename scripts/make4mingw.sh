#!/bin/sh

### Script to build a win32 installation

make
i586-mingw32msvc-strip src/kcat.exe
make nsisinst
mv src/*setup*exe .


