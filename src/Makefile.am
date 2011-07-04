# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)

bin_PROGRAMS = kcat

kcat_CPPFLAGS = @kcat_BUILD_CPPFLAGS@
kcat_CXXFLAGS = @kcat_BUILD_CXXFLAGS@
kcat_CFLAGS = $(kcat_CXXFLAGS)
kcat_LDFLAGS = @kcat_BUILD_LDFLAGS@
kcat_LDADD = @kcat_BUILD_LDADD@

MINGW32_SRC = \
	kachina.rc \
	include/kachinarc.h \
	mingw.c \
	include/compat.h \
	include/compat-mingw.h


# We distribute these but do not always compile them
EXTRA_kcat_SOURCES = $(kcat_WIN32_RES_SRC) $(MINGW32_SRC)

kcat_SOURCES =

if MINGW32
if HAVE_WINDRES
  kcat_SOURCES += $(MINGW32_SRC)
endif
endif

########################################################################


kcat_VERSION_MAJOR = @kcat_VERSION_MAJOR@
kcat_VERSION_MINOR = @kcat_VERSION_MINOR@
kcat_VERSION_PATCH = @kcat_VERSION_PATCH@
kcat_VERSION = @kcat_VERSION@

.EXPORT_ALL_VARIABLES: nsisinst appbundle

# Sources that are generated,
BUILT_SOURCES =
# not distributed,
nodist_kcat_SOURCES = $(BUILT_SOURCES)
# and deleted by the clean targets
CLEANFILES = $(BUILT_SOURCES)
CLEAN_LOCAL =

if MINGW32
if HAVE_WINDRES
.rc.o:
	$(WINDRES) -I$(srcdir)/include -I$(srcdir)/../data/win32 $< -O coff $@
endif
endif

install-data-local:
	if test -f $(srcdir)/../data/kcat.xpm; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/pixmaps; \
	    $(INSTALL_DATA) $(srcdir)/../data/kcat.xpm $(DESTDIR)/$(datadir)/pixmaps; \
	fi
	if test -f $(srcdir)/../data/kcat.desktop; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/applications; \
	    $(INSTALL_DATA) $(srcdir)/../data/kcat.desktop $(DESTDIR)/$(datadir)/applications; \
	fi

uninstall-local:
	rm -f $(DESTDIR)/$(datadir)/pixmaps/kcat.xpm
	rm -f $(DESTDIR)/$(datadir)/applications/kcat.desktop

FL_SRC = KachinaPanel.fl

if HAVE_FLUID
flgen: $(FL_SRC)
	(cd $(srcdir)/include; \
	for f in $(FL_SRC); do \
	    c=$${f%.fl}.cxx; \
		h=$${f%.fl}.h; \
		h=$${h##*/}; \
	    $(FLUID) -c -o ../$$c -h $$h ../$$f; \
	done)
endif


INSTALLER_FILE = kcat-$(kcat_VERSION)_setup.exe
APPBUNDLE=kcat-$(kcat_VERSION)
APPBUNDLE_NOLIBS=$(APPBUNDLE)-nolibs

if HAVE_NSIS
nsisinst: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mknsisinst.sh "$(srcdir)/../data" .
    CLEANFILES += $(INSTALLER_FILE)
endif

if DARWIN
appbundle: $(bin_PROGRAMS)
	sh $(srcdir)/../scripts/mkappbundle.sh "$(srcdir)/../data" .
    CLEAN_LOCAL += $(APPBUNDLE_NOLIBS) $(APPBUNDLE) $(APPBUNDLE)*.dmg
endif

#if HAVE_ASCIIDOC
#$(builddir)/../doc/guide.html: $(builddir)/../doc/guide.txt
#	$(MAKE) -C $(builddir)/../doc $(AM_MAKEFLAGS) guide.html

#$(builddir)/dialogs/guide.cxx: $(builddir)/../doc/guide.html
#	mkdir -p $(builddir)/dialogs
#	sed 's/"/\\"/g; s/$$/\\n\\/g; 1 s/.*/const char* szBeginner = "&/; $$ s/.*/&n";/' $< > $@
#dialogs/htmlstrings.cxx: $(builddir)/dialogs/guide.cxx

#distclean-local:
#	@if test "$(builddir)" != "$(srcdir)"; then \
#	    rm -f $(builddir)/dialogs/guide.cxx; \
#        fi
#endif

clean-local:
	-rm -rf $(CLEAN_LOCAL)

# Sources that we build. It is OK to have headers here.
kcat_SOURCES += \
	dialogs.cpp \
	Kachina.cpp \
	Fl_SigBar.cpp \
	FreqControl.cpp \
	images.cpp \
	IOspec.cpp \
	Kachina_io.cpp \
	KachinaPanel.cxx \
	serialcomm.cpp \
	support.cpp \
	telemetry.cpp \
	cstack.cpp \
	debug.cxx \
	icons.cxx \
	pixmaps.cxx \
	util.cxx \
	include/cstack.h \
	include/debug.h \
	include/dialogs.h \
	include/gettext.h \
	include/icons.h \
	include/pixmaps.h \
	include/Fl_SigBar.h \
	include/FreqControl.h \
	include/images.h \
	include/IOspec.h \
	include/Kachina.h \
	include/Kachina_io.h \
	include/KachinaPanel.h \
	include/kachinarc.h \
	include/linserial.h \
	include/serialcomm.h \
	include/support.h \
	include/test.h \
	include/util.h \
	include/version.h \
	include/winserial.h \
	xml_io.cxx \
	include/xml_io.h \
	xmlrpcpp/XmlRpc.h \
	xmlrpcpp/XmlRpcClient.cpp \
	xmlrpcpp/XmlRpcClient.h \
	xmlrpcpp/XmlRpcDispatch.cpp \
	xmlrpcpp/XmlRpcDispatch.h \
	xmlrpcpp/XmlRpcException.h \
	xmlrpcpp/XmlRpcMutex.cpp \
	xmlrpcpp/XmlRpcMutex.h \
	xmlrpcpp/XmlRpcServer.cpp \
	xmlrpcpp/XmlRpcServer.h \
	xmlrpcpp/XmlRpcServerConnection.cpp \
	xmlrpcpp/XmlRpcServerConnection.h \
	xmlrpcpp/XmlRpcServerMethod.cpp \
	xmlrpcpp/XmlRpcServerMethod.h \
	xmlrpcpp/XmlRpcSocket.cpp \
	xmlrpcpp/XmlRpcSocket.h \
	xmlrpcpp/XmlRpcSource.cpp \
	xmlrpcpp/XmlRpcSource.h \
	xmlrpcpp/XmlRpcThread.cpp \
	xmlrpcpp/XmlRpcThread.h \
	xmlrpcpp/XmlRpcThreadedServer.cpp \
	xmlrpcpp/XmlRpcThreadedServer.h \
	xmlrpcpp/XmlRpcUtil.cpp \
	xmlrpcpp/XmlRpcUtil.h \
	xmlrpcpp/XmlRpcValue.cpp \
	xmlrpcpp/XmlRpcValue.h \
	xmlrpcpp/base64.h


# Additional source files that are distributed
EXTRA_DIST = \
	kachina.rc \
	images/kachina.xpm \
	images/mvolts.xbm \
	images/p120.xbm \
	images/p150.xbm \
	images/p15.xbm \
	images/p30.xbm \
	images/p45.xbm \
	images/p60.xbm \
	images/p75.xbm \
	images/p90.xbm \
	images/rev.xbm \
	images/smeter.xbm \
	images/swr.xbm \
	images/dbm.xbm \
	images/alc.xbm

# Additional non-source files that are distributed
# Additional source files that support non-Linux cross compilation
EXTRA_DIST += \
	$(srcdir)/../data/kcat.desktop \
	$(srcdir)/../data/kcat.xpm \
	$(srcdir)/../data/win32/fl_app.nsi \
	$(srcdir)/../data/win32/kcat.ico \
	$(srcdir)/../scripts/mknsisinst.sh \
	$(srcdir)/../scripts/build4mingw.sh \
	$(srcdir)/../data/mac/Info.plist.in \
	$(srcdir)/../data/mac/kcat.icns \
	$(srcdir)/../scripts/mkappbundle.sh \
	$(kcat_WIN32_SRC) \
	$(kcat_FL_SRC)
