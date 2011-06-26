# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)

bin_PROGRAMS = KC505

KC505_CPPFLAGS = @KC505_BUILD_CPPFLAGS@
KC505_CXXFLAGS = @KC505_BUILD_CXXFLAGS@
KC505_CFLAGS = $(KC505_CXXFLAGS)
KC505_LDFLAGS = @KC505_BUILD_LDFLAGS@
KC505_LDADD = @KC505_BUILD_LDADD@

MINGW32_SRC = \
	Kachina-rc.rc \
	include/Kachinarc.h \
	mingw.c \
	include/compat.h \
	include/compat-mingw.h


# We distribute these but do not always compile them
EXTRA_KC505_SOURCES = $(KC505_WIN32_RES_SRC) $(MINGW32_SRC)

KC505_SOURCES =

if MINGW32
if HAVE_WINDRES
  KC505_SOURCES += $(MINGW32_SRC)
endif
endif

########################################################################


KC505_VERSION_MAJOR = @KC505_VERSION_MAJOR@
KC505_VERSION_MINOR = @KC505_VERSION_MINOR@
KC505_VERSION_PATCH = @KC505_VERSION_PATCH@
KC505_VERSION = @KC505_VERSION@

.EXPORT_ALL_VARIABLES: nsisinst appbundle

# Sources that are generated,
BUILT_SOURCES =
# not distributed,
nodist_KC505_SOURCES = $(BUILT_SOURCES)
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
	if test -f $(srcdir)/../data/KC505.xpm; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/pixmaps; \
	    $(INSTALL_DATA) $(srcdir)/../data/KC505.xpm $(DESTDIR)/$(datadir)/pixmaps; \
	fi
	if test -f $(srcdir)/../data/KC505.desktop; then \
	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/applications; \
	    $(INSTALL_DATA) $(srcdir)/../data/KC505.desktop $(DESTDIR)/$(datadir)/applications; \
	fi

uninstall-local:
	rm -f $(DESTDIR)/$(datadir)/pixmaps/KC505.xpm
	rm -f $(DESTDIR)/$(datadir)/applications/KC505.desktop

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


INSTALLER_FILE = KC505-$(KC505_VERSION)_setup.exe
APPBUNDLE=KC505-$(KC505_VERSION)
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
KC505_SOURCES += \
	cstack.cpp \
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
	telemetry.cpp

# Additional source files that are distributed
EXTRA_DIST = \
	KachinaPanel.fl \
	kachina.rc \
	config.h \
	cstack.h \
	dialogs.h \
	Fl_SigBar.h \
	FreqControl.h \
	images.h \
	includes.txt \
	IOspec.h \
	Kachina.h \
	Kachina_io.h \
	KachinaPanel.h \
	kachinarc.h \
	linserial.h \
	serialcomm.h \
	support.h \
	test.h \
	version.h \
	winserial.h
	kachina.xpm \
	mvolts.xbm \
	p120.xbm \
	p150.xbm \
	p15.xbm \
	p30.xbm \
	p45.xbm \
	p60.xbm \
	p75.xbm \
	p90.xbm \
	rev.xbm \
	smeter.xbm \
	swr.xbm

# Additional non-source files that are distributed
# Additional source files that support non-Linux cross compilation
EXTRA_DIST += 
#	$(srcdir)/../data/KC505.desktop \
#	$(srcdir)/../data/KC505.xpm \
#	$(srcdir)/../data/win32/fl_app.nsi \
#	$(srcdir)/../data/win32/KC505.ico \
#	$(srcdir)/../scripts/mknsisinst.sh \
#	$(srcdir)/../scripts/build4mingw.sh \
#	$(srcdir)/../data/mac/Info.plist.in \
#	$(srcdir)/../data/mac/KC505.icns \
#	$(srcdir)/../scripts/mkappbundle.sh \
#	$(KC505_WIN32_SRC) \
#	$(KC505_FL_SRC)
