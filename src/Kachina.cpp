#include <sys/stat.h>

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

#include "support.h"
#include "Kachina.h"

Fl_Double_Window *window;
char homedir[120] = "";
char defFileName[200];

// set to true for test file output by executing at Kachina TEST
bool test = false;

#ifndef WIN32
Pixmap	kachina_icon_pixmap;
#define KNAME "Kachina CAT"

void make_pixmap(Pixmap *xpm, const char **data)
{
	Fl_Window w(0,0, KNAME);
	w.xclass(KNAME);
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
#ifndef WIN32	
	fl_filename_expand(homedir, 119, "$HOME/.kachina/");
	int fd = open(homedir, O_RDONLY);
	if (fd == -1)
		mkdir( homedir, S_IRUSR | S_IWUSR | S_IXUSR);
	else
		close(fd);
	make_pixmap( &kachina_icon_pixmap, kachina_xpm);
	window->icon((char*)kachina_icon_pixmap);
	window->xclass(KNAME);
#endif
	if (argc == 2) {
		if (strcmp(argv[1], "TEST") == 0)
			test = true;
	}
	
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
