#include "config.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Item.H>

#ifdef WIN32
#  include "kcatrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

#include <FL/filename.H>
#ifdef __MINGW32__
#  undef dirent
#endif

#include <dirent.h>

#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>

#include "kcat_icon.cxx"

#include "config.h"
#include "support.h"
#include "kcat.h"
#include "debug.h"
#include "gettext.h"
#include "font_browser.h"

int parse_args(int argc, char **argv, int& idx);

pthread_t *watchdog_thread = 0;
pthread_t *serial_thread = 0;
pthread_t *telemetry_thread = 0;
pthread_t *xmlrpc_thread = 0;
pthread_t *cw_thread = 0;

pthread_mutex_t mutex_watchdog = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_serial = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_telemetry = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_xmlrpc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cw = PTHREAD_MUTEX_INITIALIZER;

bool testing = false;

Fl_Double_Window *window;

extern Fl_Double_Window *dlgFreqCalib;
extern Fl_Double_Window *dlgAntPorts;
extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgCommsConfig;
extern Fl_Double_Window *dlgNRAM;
extern Font_Browser     *fntbrowser;

string homedir = "";
char defFileName[200];

// set to true for test file output by executing at kcat TEST
bool test = false;

#if defined(__WIN32__) && defined(PTW32_STATIC_LIB)
static void ptw32_cleanup(void)
{
	(void)pthread_win32_process_detach_np();
}

void ptw32_init(void)
{
	(void)pthread_win32_process_attach_np();
	atexit(ptw32_cleanup);
}
#endif // __WIN32__

#define KNAME "kcat"
#if !defined(__WIN32__) && !defined(__APPLE__)
Pixmap	kcat_icon_pixmap;

void make_pixmap(Pixmap *xpm, const char **data)
{
	Fl_Window w(0,0, PACKAGE_NAME);
	w.xclass(PACKAGE_NAME);
	w.show();
	w.make_current();
	Fl_Pixmap icon(data);
	int maxd = (icon.w() > icon.h()) ? icon.w() : icon.h();
	*xpm = fl_create_offscreen(maxd, maxd);
	fl_begin_offscreen(*xpm);
	fl_color(FL_BACKGROUND_COLOR);
	fl_rectf(0, 0, maxd, maxd);
	icon.draw(maxd - icon.w(), maxd - icon.h());
	fl_end_offscreen();
}

#endif

void visit_URL(void* arg)
{
	const char* url = reinterpret_cast<const char *>(arg);
#ifndef __WOE32__
	const char* browsers[] = {
#  ifdef __APPLE__
		getenv("FLDIGI_BROWSER"), // valid for any OS - set by user
		"open"                    // OS X
#  else
		"fl-xdg-open",            // Puppy Linux
		"xdg-open",               // other Unix-Linux distros
		getenv("FLDIGI_BROWSER"), // force use of spec'd browser
		getenv("BROWSER"),        // most Linux distributions
		"sensible-browser",
		"firefox",
		"mozilla"                 // must be something out there!
#  endif
	};
	switch (fork()) {
	case 0:
#  ifndef NDEBUG
		unsetenv("MALLOC_CHECK_");
		unsetenv("MALLOC_PERTURB_");
#  endif
		for (size_t i = 0; i < sizeof(browsers)/sizeof(browsers[0]); i++)
			if (browsers[i])
				execlp(browsers[i], browsers[i], url, (char*)0);
		exit(EXIT_FAILURE);
	case -1:
		fl_alert(_("Could not run a web browser:\n%s\n\n"
			 "Open this URL manually:\n%s"),
			 strerror(errno), url);
	}
#else
	// gurgle... gurgle... HOWL
	// "The return value is cast as an HINSTANCE for backward
	// compatibility with 16-bit Windows applications. It is
	// not a true HINSTANCE, however. The only thing that can
	// be done with the returned HINSTANCE is to cast it to an
	// int and compare it with the value 32 or one of the error
	// codes below." (Error codes omitted to preserve sanity).
	if ((int)ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL) <= 32)
		fl_alert(_("Could not open url:\n%s\n"), url);
#endif
}

static void checkdirectories(void)
{
	struct {
		string& dir;
		const char* suffix;
		void (*new_dir_func)(void);
	} dirs[] = {
		{ homedir, 0, 0 }
	};

	int r;
	for (size_t i = 0; i < sizeof(dirs)/sizeof(*dirs); i++) {
		if (dirs[i].suffix)
			dirs[i].dir.assign(homedir).append(dirs[i].suffix).append("/");

		if ((r = mkdir(dirs[i].dir.c_str(), 0777)) == -1 && errno != EEXIST) {
			cerr << _("Could not make directory") << ' ' << dirs[i].dir
				 << ": " << strerror(errno) << '\n';
			exit(EXIT_FAILURE);
		}
		else if (r == 0 && dirs[i].new_dir_func)
			dirs[i].new_dir_func();
	}
}

void exit_main(Fl_Widget *w)
{
	if (Fl::event_key() == FL_Escape)
		return;
	cbExit();
}

void kcat_terminate() {
	std::cerr << "terminating" << std::endl;
	fl_message("Unrecoverable error\nTerminating kcat");
	cbExit();
}

int main (int argc, char *argv[])
{
	int arg_idx;

	std::set_terminate(kcat_terminate);

	Fl::args(argc, argv, arg_idx, parse_args);

	window = kcat_window();
	window->label(PACKAGE_NAME);
	window->callback(exit_main);

	fntbrowser = new Font_Browser;
	dlgFreqCalib = FreqCalibDialog();
	dlgAntPorts = FreqRangesDialog();
	dlgDisplayConfig = DisplayDialog();
	dlgCommsConfig = CommsDialog();

	char dirbuf[FL_PATH_MAX + 1];
#ifdef __WIN32__
	fl_filename_expand(dirbuf, sizeof(dirbuf) - 1, "$USERPROFILE/kcat.files/");
#else
	fl_filename_expand(dirbuf, sizeof(dirbuf) - 1, "$HOME/.kcat/");
#endif
	if (homedir.empty()) homedir = dirbuf;
	checkdirectories();

	try {
		debug::start(string(homedir).append("debug_log.txt").c_str());
		time_t t = time(NULL);
		LOG(debug::WARN_LEVEL, debug::LOG_OTHER, _("%s log started on %s"), PACKAGE_STRING, ctime(&t));
	}
	catch (const char* error) {
		cerr << error << '\n';
		debug::stop();
		exit(1);
	}

	Fl::lock();

#if defined(__WIN32__) && defined(PTW32_STATIC_LIB)
	ptw32_init();
#endif

	buildlist();
	initOptionMenus();

	loadState();

	if (xcvrState.ttyport.empty() || xcvrState.ttyport == "TEST") {
		testing = true;
	} else
		testing = false;

	window->xclass(KNAME);

#if defined(__WOE32__)
#  ifndef IDI_ICON
#    define IDI_ICON 101
#  endif
	window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON)));
	window->show (argc, argv);
#elif !defined(__APPLE__)
	make_pixmap(&kcat_icon_pixmap, kcat_icon);
	window->icon((char *)kcat_icon_pixmap);
	window->show(argc, argv);
#else
	window->show(argc, argv);
#endif

	Fl::add_timeout(0.05, startProcessing);

    return Fl::run();
}

int parse_args(int argc, char **argv, int& idx)
{
	if (strcasecmp("--help", argv[idx]) == 0) {
		printf("Usage: \n\
  --help this help text\n\
  --config-dir <DIR>\n\
  --version\n");
		exit(0);
	} 
	if (strcasecmp("--version", argv[idx]) == 0) {
		printf("Version: "VERSION"\n");
		exit (0);
	}
	if (strcasecmp("--config-dir", argv[idx]) == 0) {
		homedir = argv[idx + 1];
		if (homedir[homedir.length()-1] != '/')
			homedir += '/';
		idx += 2;
		return 1;
	}
	return 0;
}
