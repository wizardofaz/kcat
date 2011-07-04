#ifndef _KACHINA_H
#define _KACHINA_H

//#define DEBUG 1

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Enumerations.H>
#include <sys/types.h>
#include <pthread.h>

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_draw.H>

#include "FreqControl.h"
#include "KachinaPanel.h"
#include "Kachina_io.h"
#include "serialcomm.h"

extern pthread_t *watchdog_thread;
extern pthread_t *serial_thread;
extern pthread_t *telemetry_thread;
extern pthread_t *xmlrpc_thread;

extern pthread_mutex_t mutex_watchdog;
extern pthread_mutex_t mutex_serial;
extern pthread_mutex_t mutex_telemetry;
extern pthread_mutex_t mutex_xmlrpc;

extern Fl_Double_Window *window;
extern char homedir[120];
extern char defFileName[200];

extern long freqlist[100];
extern int  numinlist;

extern void selectFreq();
extern void clearList();
extern void buildlist();
extern void addtoList(long);
extern void updateSelect();
extern void addtoSelect(long);
extern void sortList();

extern void addFreq();
extern void delFreq();
extern void setMode();
extern void setBW();
extern int  movFreq();
extern void startProcessing(void *);

extern void configColor();
extern void openFreqList();
extern void saveFreqList();
extern void about();

extern void initOptionMenus();

#endif
