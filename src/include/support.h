#ifndef SUPPORT_H
#define SUPPORT_H

#include <fstream>

#include <math.h>
#ifndef WIN32
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#endif

#include "kcat.h"
#include "version.h"
#include "IOspec.h"

#include "cstack.h"
#include "kcat_io.h"
#include "status.h"

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

extern bool testing;

extern bool rx_on_a;
extern bool tx_on_a;
extern int  iAntSel;

extern const char *szmodes[];
extern const char modetype[];
extern const char *szBW[];
extern int iBW[];
extern FREQMODE vfoA;
extern FREQMODE vfoB;
extern FREQMODE xmlvfo;

extern int watchdog_count;

extern void cbExit();

extern void setMode();
extern void setBW();
extern void addFreq();
extern void delFreq();
extern void clearList();
extern int  movFreq();
extern int  movFreqB();
//extern void cbVFOsel();
extern void cbA2B();
extern void cbB2A();
extern void cbRxA_TxA();
extern void cbRxA_TxB();
extern void cbRxB_TxA();
extern void cbRxB_TxB();

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
extern void cbAntSel();
extern void cb_autotune(int);
extern void cbTune();
extern void cbPTT();
extern void cbCarrier();
extern void cbRIT();
extern void cbbtnRIT();
extern void cbSmeter();
extern void cbSWR();
extern void cbPWR();
extern void cbVol();
extern void cbTemp();
extern void GetKachinaVersion();

extern void cb_vfo_adj();

extern void loadConfig();
extern void saveConfig();
extern void loadState();
extern void saveState();
extern void initkcat();

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

// CW Dialog
extern void cbCWattack();
extern void cbCWweight();
extern void cbCWmode();
extern void cbCWoffset();
extern void cbCWdefFilter();
extern void cbQSKonoff();
extern void cbSPOT();

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

extern Fl_Double_Window *dlgFreqCalib;
extern Fl_Double_Window *dlgRcvConfig;
extern Fl_Double_Window *dlgXmtConfig;
extern Fl_Double_Window *dlgCwParams;
extern Fl_Double_Window *dlgAntPorts;
extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgCommsConfig;
extern Fl_Double_Window *dlgViewLog;
extern Fl_Double_Window *dlgNRAM;
extern Fl_Double_Window *dlgScanner;
extern Fl_Double_Window *dlgCWkeyboard;

extern CSerialComm kcatSerial;
extern char szttyport[];
extern int  baudttyport;

// used for reference freq calibration run
extern int	avgrcvsig ;
extern int avgcnt;
extern bool computeavg;

extern int checkCalibrate(long);
extern void Calibrate();
extern int movFreq();
extern int movFreqB();

void cbEventLog();

extern void setDisplayColors();

extern void cbOkDisplayDialog();
extern void cbCancelDisplayDialog();
extern void cbPrefFont();
extern void cbPrefBackground();
extern void cbPrefForeground();

extern void cbSMeterColor();
extern void cbPwrMeterColor();
extern void cbSWRMeterColor();
extern void cbPeakMeterColor();
extern void cbBacklightColor();

extern void cb_sys_defaults();
extern void cb_sys_foreground();
extern void cb_sys_background();
extern void cb_sys_background2();

extern void cb_reset_display_dialog();
extern void cb_slider_background();
extern void cb_slider_select();
extern void cb_slider_defaults();

extern void cb_lighted_button();
extern void cb_lighted_default();

extern Fl_Color flrig_def_color(int);

extern void parseTelemetry(void *);

// scanner
extern void set_freq_range();
extern void start_scan();
extern void start_continuous_scan();
extern void stop_scan();
extern int  startFreq();
extern int  db_min_cb();
extern int  db_max_cb();
extern void update_scanner(int);
extern void open_scanner();

// CW
extern void open_keyboard();
extern void sendCQ();

extern void config_messages();

extern void exec_msg1();
extern void exec_msg2();
extern void exec_msg3();
extern void exec_msg4();
extern void exec_msg5();
extern void exec_msg6();
extern void exec_msg7();
extern void exec_msg8();
extern void exec_msg9();
extern void exec_msg10();
extern void exec_msg11();
extern void exec_msg12();

extern void change_txt_cll();
extern void change_txt_qth();
extern void change_txt_loc();
extern void change_txt_opr();

extern void check_call();
extern void send_message(string msg);
extern void sendChar(int c);
extern void do_config_messages(void *);
extern void ck_band();
extern void dups();
extern void zeros();
extern void time_span();
extern void serial_nbr();
extern void apply_edit();
extern void done_edit();
extern void cancel_edit();

extern void xml_get_record(const char *);
extern void xml_add_record();
extern void xml_dup_check();
extern void connect_to_server();

extern void cb_cancel_transmit();
extern void cb_send_button();
extern void close_cw_keyboard();

#endif
