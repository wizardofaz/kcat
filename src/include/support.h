#ifndef SUPPORT_H
#define SUPPORT_H

#include <fstream>

#include <math.h>
#ifndef WIN32
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#endif

#include "Kachina.h"
#include "version.h"
#include "IOspec.h"

#include "cstack.h"
#include "Kachina_io.h"

#include "images.h"

#include "serialcomm.h"
#include <FL/fl_show_colormap.H>
#include <FL/fl_ask.H>

#define LISTSIZE 200

struct FREQMODE {
	long freq;
	int  imode;
	int  iBW;
	int  src;
};

enum {UI, XML};
enum MODES {LSB, USB, CW, AM, FM};

extern const char *szmodes[];
extern const char modetype[];
extern const char *szBW[];
extern int iBW[];
extern FREQMODE vfoA;
extern FREQMODE vfoB;
extern FREQMODE xmlvfo;

extern void cbExit();

extern void setMode();
extern void setBW();
extern void addFreq();
extern void delFreq();
extern void clearList();
extern int  movFreq();
extern int  movFreqB();
extern void adjustFreqs();
extern void cbABsplit();
extern void cbABactive();
extern void cbA2B();
extern void setFocus();
extern void show_controls();

extern void setNotch();

extern void setInhibits();

extern void set_xml_values(void *);

extern void cbAttenuator();
extern void cbPreamp();
extern void cbbtnNR();
extern void cbNR();
extern void cbbtnNotch();
extern void cbDepth();
extern void setNotch();
extern void setNotchWidth();
extern void setIFshift();
extern void cbIFsh();
extern void setVolume();
extern void setMicGain();
extern void setPower();
extern void setPTT(void *);
extern void cbWPM();
extern void cbRxAnt();
extern void cbTxAnt();
extern void cbTune();
extern void cbPTT();
extern void cbCarrier();
extern void cbRIT();
extern void cbbtnRIT();
extern void cbSmeter();
extern void cbSWR();
extern void cbPWR();
extern void cbMute();
extern void cbTemp();
extern void GetKachinaVersion();
extern void OpenTestLog();
extern void CloseTestLog();
extern void writeTestLog( char *str);

extern void cb_vfo_adj();

extern void loadConfig();
extern void saveConfig();
extern void loadState();
extern void saveState();
extern void initKachina();

extern void openRcvConfigDialog();
extern void cbsldrAgcAction();
extern void cbsldrAgcSpeed();
extern void cbSqlLevel();
extern void cbSQLtype();
extern void closeRcvDialog();

extern void openXmtConfigDialog();
extern void cbbtnSpchProc();
extern void cbSpchMon();
extern void cbsldrCompression();
extern void cbSidetone();
extern void cbVoxOnOff();
extern void cbsldrVoxLevel();
extern void cbsldrAntiVox();
extern void cbsldrVoxDelay();
extern void cbsldrXmtEqualizer();
extern void cbbtnAmpOnOff();
extern void closeXmtDialog();


extern void cbOkCommsDialog();
extern void initCommPortTable ();
extern void setCommsPort();
extern void setDisplayColors();

extern void cbPrefBackground();
extern void cbPrefForeground();
extern void cbSelectColor();
extern void cbSmeterColor();
extern void cbPWRcolor();
extern void cbSWRcolor();
extern void cbOkDisplayDialog();

// CW Dialog
extern void cbCWattack();
extern void cbCWweight();
extern void cbCWmode();
extern void cbCWoffset();
extern void cbCWdefFilter();
extern void cbQSKonoff();
extern void cbCWspot();

// Log Viewer
extern void openLogViewer();
extern void closeLogViewer();
extern void cbViewLog();

// Antenna Port Dialog
struct stANTPORT {
	int freq;
	int rcv;
	int xmt;
};

extern void cbClearAntData();

// NRAM Dialog
extern void cbNRAM();
extern void cbNRAMok();
extern void cbNRAMAntImp();
extern void cbNRAMsmeter();
extern void cbNRAMFreqRef();
extern void cbNRAMPhase();
extern void cbNRAMCarrier();
extern void cbNRAMClearText();
extern void cbNRAMAll();
extern void cbNRAMSave();
extern void cbNRAMRestore();

extern stANTPORT antports[];
extern int numantports;

extern void  cbbrwsAntRanges();
extern void  cbAddAntRange();
extern void  cbDeleteAntRange();
extern void  cbAntRangeDialogOK();
extern void  cbmnuAntPorts();

extern void setPowerImage(double);

// Kachina parameters & state variables
struct XCVRSTATE {
	int VFO; // SIMPLEX
	int NOTCHWIDTH;
	int ATTEN;
	int ANTIVOX;
	int VOXDELAY; //5
	
	int QSK;
	int CWATTACK;
	int CWWEIGHT;
	int CWMODE; // left handed 10
	
	int SQUELCH; // level sensitive
	int AMP; // off
	int SPEECHPROC; // off
	int SPEECHCOMP; // min
	int ANTTUNE; // on 15

	int CWOFFSET; // 700 Hz
	int CWFILTER; // narrow
	int EQUALIZER; // flat
	int PREAMP; // off
	int SQLEVEL; //20
	
	int SPCHMONITOR; // off
	int NR; // off
	int AGCSPEED; // fast
	int AGCACTION; //
	int RIT; //25

	int MODE; // USB
	int BW; // 2.7 kHz
	int VOXLEVEL;

	long   FREQ;
	
	double VOL;
	double NOTCHFREQ;
	double MAXPWR;
	double MICGAIN;
	double CWMON;
	double CWSPEED;

	double NR_LEVEL; // min
	double RITFREQ;
	double IFSHIFT;
	double NOTCHDEPTH;
	char   vers[10];
	int	   mainX;
	int    mainY;
	int	   TxOffset;

	double VFOADJ;
};

extern struct XCVRSTATE xcvrState;

extern Fl_Double_Window *dlgFreqCalib;
extern Fl_Double_Window *dlgRcvConfig;
extern Fl_Double_Window *dlgXmtConfig;
extern Fl_Double_Window *dlgCwParams;
extern Fl_Double_Window *dlgAntPorts;
extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgCommsConfig;
extern Fl_Double_Window *dlgViewLog;
extern Fl_Double_Window *dlgNRAM;

extern CSerialComm KachinaSerial;
extern char szttyport[];
extern int  baudttyport;

// used for reference freq calibration run
extern int	avgrcvsig ;
extern int avgcnt;
extern bool computeavg;

extern int checkCalibrate(long int);
extern void Calibrate();

#endif
