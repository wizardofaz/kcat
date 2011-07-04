#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#ifdef WIN32
	#include "kachinarc.h"
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <FL/Fl_Pixmap.H>
	#include <FL/Fl_Image.H>
	#include "kachina.xpm"
#endif

#include <FL/x.H>

#include "config.h"
#include "support.h"
#include "Kachina.h"

pthread_t *watchdog_thread = 0;
pthread_t *serial_thread = 0;
pthread_t *telemetry_thread = 0;
pthread_t *xmlrpc_thread = 0;

pthread_mutex_t mutex_watchdog = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_serial = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_telemetry = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_xmlrpc = PTHREAD_MUTEX_INITIALIZER;

Fl_Double_Window *window;
char homedir[120] = "";
char defFileName[200];

// set to true for test file output by executing at Kachina TEST
bool test = false;

#ifndef WIN32
Pixmap	kachina_icon_pixmap;

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

int main (int argc, char *argv[])
{
	window = Kachina_window();
	window->label(PACKAGE_NAME);
#ifndef WIN32	
	fl_filename_expand(homedir, 119, "$HOME/.kachina/");
	int fd = open(homedir, O_RDONLY);
	if (fd == -1)
		mkdir( homedir, S_IRUSR | S_IWUSR | S_IXUSR);
	else
		close(fd);
	make_pixmap( &kachina_icon_pixmap, kachina_xpm);
	window->icon((char*)kachina_icon_pixmap);
	window->xclass(PACKAGE_NAME);
#endif
	if (argc == 2) {
		if (strcmp(argv[1], "TEST") == 0)
			test = true;
	}

	Fl::lock();

#if defined(__WIN32__) && defined(PTW32_STATIC_LIB)
	ptw32_init();
#endif

	buildlist();
	initOptionMenus();

	loadConfig();
	loadState();

#ifdef WIN32
	window->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON)));
	window->show (argc, argv);
#else
	window->show();
#endif

	Fl::add_timeout(0.05, startProcessing);

    return Fl::run();
}
