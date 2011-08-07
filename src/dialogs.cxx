#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/fl_draw.H>

#include <string>

#include <iostream>

using namespace std;

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#endif

#ifdef __APPLE__
#  include <glob.h>
#endif

#include "dialogs.h"
#include "debug.h"
#include "font_browser.h"
#include "kcat_panel.h"

Fl_Double_Window *dlgFreqCalib = NULL;
Fl_Double_Window *dlgAntPorts  = NULL;
Fl_Double_Window *dlgDisplayConfig = NULL;
Fl_Double_Window *dlgCommsConfig = NULL;
Fl_Double_Window *dlgNRAM = NULL;
Fl_Double_Window *dlgScanner = NULL;

Font_Browser     *fntbrowser = NULL;

Fl_Color flrig_def_color(int);

//-----------Frequency Calibration dialog
void openFreqCalibDialog()
{
	if (!dlgFreqCalib)
		dlgFreqCalib = FreqCalibDialog();
	dlgFreqCalib->show();
}

void closeFreqCalibDialog()
{
	dlgFreqCalib->hide();
}

void cbCalibrate()
{
	long refstd = (long)(cntrFreqStd->value() * 1000000.0);
	checkCalibrate(refstd);
}

void cbCalFinished()
{
	closeFreqCalibDialog();
	initXcvrState();
}

//-----------Receiver settings dialog

void cbsldrAgcAction()
{
	xcvrState.AGCACTION = (int)sldrAgcAction->value();
	setXcvrAGCaction(xcvrState.AGCACTION);
}

void cbsldrAgcSpeed()
{
	xcvrState.AGCSPEED = (int)sldrAgcSpeed->value();
	setXcvrAgcSpeed(xcvrState.AGCSPEED);
}

void cbSqlLevel()
{
	xcvrState.SQLEVEL = (int)sldrSqlLevel->value();
	setXcvrSqlLevel(xcvrState.SQLEVEL);
}

void cbSQLtype()
{
	if (btnSQLtype[1]->value() == 1)
		xcvrState.SQUELCH = 1;
	else
		xcvrState.SQUELCH = 0;
	setXcvrSqlType(xcvrState.SQUELCH);
}

//-----------Transmit settings dialog

void cbbtnSpchProc()
{
	xcvrState.SPEECHPROC = btnSpchProc->value();
	setXcvrSpeechProcessor (xcvrState.SPEECHPROC);
}

void cbSpchMon()
{
	xcvrState.SPCHMONITOR = btnSpchMon->value();
	setXcvrSpchMon(xcvrState.SPCHMONITOR);
}

void cbsldrCompression()
{
	xcvrState.SPEECHCOMP = (int)sldrCompression->value();
	setXcvrCompression(xcvrState.SPEECHCOMP);
}

void cbSidetone()
{
	setXcvrCWMON (sldrSideTone->value());
}

void cbVoxOnOff()
{
	setXcvrVoxOnOff(btnVoxOnOff->value());
}

void cbsldrVoxLevel()
{
	xcvrState.VOXLEVEL = (int)sldrVoxLevel->value();
	if (btnVoxOnOff->value()) setXcvrVoxOnOff(1);
}

void cbsldrAntiVox()
{
	xcvrState.ANTIVOX = (int)sldrAntiVox->value();
	setXcvrAntiVox(xcvrState.ANTIVOX);
}

void cbsldrVoxDelay()
{
	xcvrState.VOXDELAY = (int)sldrVoxDelay->value();
	setXcvrVoxDelay(xcvrState.VOXDELAY);
}

void cbsldrXmtEqualizer()
{
	xcvrState.EQUALIZER = (int)sldrXmtEqualizer->value();
	setXcvrEqualizer(xcvrState.EQUALIZER);
}

void cbbtnAmpOnOff()
{
	xcvrState.AMP = btnAmpOnOff->value();
	setXcvrAmpOnOff(xcvrState.AMP);
}


// CW parameters

void cbCWattack()
{
	xcvrState.CWATTACK = (int)sldrCWattack->value();
	setXcvrCWattack(xcvrState.CWATTACK);
}

void cbCWweight()
{
	xcvrState.CWWEIGHT = (int)sldrCWweight->value();
	setXcvrCWweight(xcvrState.CWWEIGHT);
}

void cbCWmode()
{
	xcvrState.CWMODE = mnuCWmode->value();
	setXcvrCWmode(xcvrState.CWMODE);
}

void cbCWoffset()
{
	xcvrState.CWOFFSET = mnuCWoffset->value();
	setXcvrCWoffset(xcvrState.CWOFFSET);
}

void cbCWdefFilter()
{
	xcvrState.CWFILTER = mnuCWdefFilter->value();
	setXcvrCWdefFilter(xcvrState.CWFILTER);
}

void cbQSKonoff()
{
	xcvrState.QSK = btnQSKonoff->value();
	setXcvrQSKonoff(xcvrState.QSK);
}

// Antenna Port Dialog

void sortAntPortList()
{
	if (!numantports) return;
	stANTPORT temp;
	for (int i = 0; i < numantports - 1; i++)
		for (int j = i + 1; j < numantports; j++)
			if (antports[i].freq > antports[j].freq) {
					temp = antports[i];
					antports[i] = antports[j];
					antports[j] = temp;
			}
}

void clearAntPortList()
{
	if (!numantports) return;
	for (int i = 0; i < LISTSIZE; i++) {
		antports[i].freq = 0;
		antports[i].rcv = 0;
		antports[i].xmt = 0;
	}
	brwsAntRanges->clear();
	numantports = 0;
}

void  cbbrwsAntRanges()
{
	int n = brwsAntRanges->value();
	if (!n) return;
	n--;
	char szFreq[10];
	sprintf(szFreq, "%d", antports[n].freq);
	txtFreqRange->value(szFreq);
	if (antports[n].rcv == 0) {
		btnRcvAnt->label("A");
		btnRcvAnt->value(0);
	}
	else {
		btnRcvAnt->label("B");
		btnRcvAnt->value(1);
	}
	if (antports[n].xmt == 0) {
		btnXmtAnt->label("A");
		btnXmtAnt->value(0);
	}
	else {
		btnXmtAnt->label("B");
		btnXmtAnt->value(1);
	}
}

void updateAntRanges()
{
	char szListEntry[30];
	brwsAntRanges->clear();
	for (int n = 0; n < numantports; n++) {
		sprintf(szListEntry,"%5d %c %c",
			antports[n].freq,
			antports[n].rcv == 0 ? 'A' : 'B',
			antports[n].xmt == 0 ? 'A' : 'B');
		brwsAntRanges->add(szListEntry);
	}
}

void  cbAddAntRange()
{
	int freq = 0;
	if (!sscanf(txtFreqRange->value(),"%d",&freq));
	if (!freq)
		return;
	int n = 0;
	while (n < LISTSIZE) {
		if (freq == antports[n].freq)
			break;
		if (antports[n].freq == 0)
			break;
		n++;
	}
    antports[n].freq = freq;
	antports[n].rcv = btnRcvAnt->value();
	antports[n].xmt = btnXmtAnt->value();
	if (n == numantports) numantports++;
	if (numantports == LISTSIZE) numantports --;
	sortAntPortList();
	updateAntRanges();
}

void  cbDeleteAntRange()
{
	if (brwsAntRanges->value()) {
		int n = brwsAntRanges->value() - 1;
		for (int i = n; i < numantports; i ++)
			antports[i] = antports[i+1];
		antports[numantports - 1].freq = 0;
		antports[numantports - 1].rcv = 0;
		antports[numantports - 1].xmt = 0;
		numantports--;
		updateAntRanges();
	}
}

void  cbAntRangeDialogOK()
{
	dlgAntPorts->hide();
}

void  cbmnuAntPorts()
{
	if (!dlgAntPorts)
		dlgAntPorts = FreqRangesDialog();
	updateAntRanges();
	dlgAntPorts->show();
}


//======================================================================
// Comm port dialog
//======================================================================
#define COMMPORT_TABLESIZE 20

string ttyport;
string commPortTable[COMMPORT_TABLESIZE];
string sCommPorts;

int  commportnbr = 0;
int  iNbrCommPorts  = 0;
char szttyport[20] = "";
bool commport_table_empty = true;

bool waitfordialog = false;

void clear_combos()
{
	for (int i = 0; i < COMMPORT_TABLESIZE; i++)
		commPortTable[i].clear();
	commPortTable[0] = "TEST";
	sCommPorts = "TEST";
	iNbrCommPorts = 0;
}

void add_combos(char *port)
{
	iNbrCommPorts++;
	if (iNbrCommPorts >= COMMPORT_TABLESIZE) return;

	commPortTable[iNbrCommPorts] = port;
	sCommPorts.append("|").append(port);
}

//======================================================================
// WIN32 init_port_combos
//======================================================================

#ifdef __WIN32__
static bool open_serial(const char* dev)
{
	bool ret = false;
	HANDLE fd = CreateFile(dev, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (fd != INVALID_HANDLE_VALUE) {
		CloseHandle(fd);
		ret = true;
	}
	return ret;
}

#  define TTY_MAX 255
void initCommPortTable()
{
	clear_combos();

	char ttyname[21];
	const char tty_fmt[] = "//./COM%u";

	for (unsigned j = 0; j < TTY_MAX; j++) {
		snprintf(ttyname, sizeof(ttyname), tty_fmt, j);
		if (!open_serial(ttyname))
			continue;
		snprintf(ttyname, sizeof(ttyname), "COM%u", j);
		LOG_WARN("Found serial port %s", ttyname);
		add_combos(ttyname);
	}
}
#endif //__WIN32__

//======================================================================
// Linux init_port_combos
//======================================================================

#ifdef __linux__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif
#  define TTY_MAX 8

void initCommPortTable()
{
	struct stat st;
	char ttyname[PATH_MAX + 1];

	clear_combos();

	const char* tty_fmt[] = {
		"/dev/ttyS%u",
		"/dev/ttyUSB%u",
		"/dev/usb/ttyUSB%u"
	};
	LOG_WARN("%s", "Serial port discovery via 'stat'");
	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;

			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}

}
#endif // __linux__

//======================================================================
// APPLE init_port_combos
//======================================================================

#ifdef __APPLE__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif

#  include <glob.h>

void initCommPortTable()
{
	clear_combos();

	struct stat st;

	const char* tty_fmt[] = {
		"/dev/cu.*",
		"/dev/tty.*"
	};

	glob_t gbuf;

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		glob(tty_fmt[i], 0, NULL, &gbuf);
		for (size_t j = 0; j < gbuf.gl_pathc; j++) {
			if ( !(stat(gbuf.gl_pathv[j], &st) == 0 && S_ISCHR(st.st_mode)) ||
			     strstr(gbuf.gl_pathv[j], "modem") )
				continue;
			LOG_WARN("Found serial port %s", gbuf.gl_pathv[j]);
			add_combos(gbuf.gl_pathv[j]);
		}
		globfree(&gbuf);
	}
}
#endif //__APPLE__

//======================================================================
// FreeBSD init_port_combos
//======================================================================

#ifdef __FreeBSD__
#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif
#  define TTY_MAX 8

void initCommPortTable()
{
	int retval;
	struct stat st;
	char ttyname[PATH_MAX + 1];
	const char* tty_fmt[] = {
		"/dev/ttyd%u"
	};

	clear_combos();

	for (size_t i = 0; i < sizeof(tty_fmt)/sizeof(*tty_fmt); i++) {
		for (unsigned j = 0; j < TTY_MAX; j++) {
			snprintf(ttyname, sizeof(ttyname), tty_fmt[i], j);
			if ( !(stat(ttyname, &st) == 0 && S_ISCHR(st.st_mode)) )
				continue;
			LOG_WARN("Found serial port %s", ttyname);
			add_combos(ttyname);
		}
	}
}
#endif //__FreeBSD__

void cbOkCommsDialog()
{
	dlgCommsConfig->hide();
	waitfordialog = false;
	commportnbr = selectCommPort->value();
	xcvrState.ttyport =  commPortTable[commportnbr];
	if (xcvrState.ttyport == "TEST") {
		testing = true;
		kcatSerial.ClosePort();
	} else {
		testing = false;
		if (startComms(xcvrState.ttyport.c_str(), 9600) == 0) {
			fl_message("%s not available", xcvrState.ttyport.c_str());
			testing = true;
		} else
			initkcat();
	}
}

void setCommsPort()
{
	if (dlgCommsConfig == NULL)
		dlgCommsConfig = CommsDialog();
	if (commport_table_empty) {
		initCommPortTable();
		selectCommPort->add(sCommPorts.c_str());
		commport_table_empty = false;
	}
	commportnbr = 0;
	selectCommPort->value(commportnbr);
	waitfordialog = true;
	dlgCommsConfig->show();
	while (waitfordialog) Fl::wait();
}

// NRAM Dialog
static Fl_Text_Buffer txtDataBuffer;

void cbNRAM()
{
	if (!dlgNRAM) {
		dlgNRAM = NRAMdataDialog();
		txtDataDisp->buffer(&txtDataBuffer);
		txtDataDisp->textfont(FL_SCREEN);
	}
	dlgNRAM->show();
}

void cbNRAMok()
{
	dlgNRAM->hide();
}

int IntFreq(unsigned int a)
{
	int low = a >> 8;
	int high = (a & 0x3F) << 8;
	return high + low;
}

double AntImpFreq(unsigned int a)
{
	double f = IntFreq(a) << 16;
	if (f == 0.0) return f;
	f /= 2.2369621333;
	f -= 75000000.0;
	return f;
}

struct ANTIMP {
	unsigned short int freq;
	uchar cap;
	uchar ind;
};

char *szBinary(int n)
{
	static char bin[9];
	for (int i = 0; i < 8; i ++) {
		if (n & 1)
			bin[7-i] = '1';
		else
			bin[7-i] = '0';
		n /= 2;
	}
	return bin;
}

int binCaps(int n)
{
	static int caps[] = {20, 39, 82, 160, 330, 680, 1500};
	int val = 0;
	for (int i = 0; i < 7; i++) {
		if (n & 1) val += caps[i];
		n /= 2;
	}
	return val;
}


void cbNRAMAntImp()
{
	union {
		unsigned char data[258];
		struct ANTIMP antimp[64];
	}antdata;

	char line[256];
	int chksum, aport;
	ANTIMP temp;

	setXcvrVolume(0.0);

	RequestData (cmdK_RIMP, antdata.data, 258);
	chksum = 0;
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 4; j++)
			chksum += antdata.data[4*i+j];
	if (chksum == 256*antdata.data[256] + antdata.data[257]) {
// sort values with 0 0 0 0 entries last
		int f1, f2;
		for (int i = 0; i < 63; i++) {
			f1 = IntFreq(antdata.antimp[i].freq);
			for (int j = i+1; j < 64; j++) {
				f2 = IntFreq(antdata.antimp[j].freq);
				if (f2 != 0)
					if (f1 == 0  || f1 > f2 ) {
						f1 = f2;
						temp = antdata.antimp[i];
						antdata.antimp[i] = antdata.antimp[j];
						antdata.antimp[j] = temp;
					}
			}
		}
		txtDataDisp->insert("Antenna Impedance:\n");
		for (int i = 0; i < 64; i++) {
			int f = IntFreq(antdata.antimp[i].freq);
			if (f) {
				sprintf(line, "%10.0f ",
						AntImpFreq(antdata.antimp[i].freq));
				txtDataDisp->insert(line);
				aport = antdata.data[4*i] >> 6;
				if (aport == 1 || aport == 2)
					sprintf(line, "%c CL %5d pF %c, %4.1f uH\n",
							aport == 1 ? 'A' : 'B',
							binCaps(antdata.data[4*i+2]),
							antdata.data[4*i+2] > 127 ? 'S' : 'L',
							11.0 * antdata.data[4*i+3] / 128.0);
				else
					sprintf(line, "%c rp   %4.3f rho,   %3.0f phi\n",
							aport == 0 ? 'A' : 'B',
							antdata.data[4*i+2] / 256.0,
							360.0 * antdata.data[4*i+3] / 256.0);
				txtDataDisp->insert(line);
			}
		}
	}
	else {
		sprintf(line,"Ant Impedance: Check sum Error\n");
		txtDataDisp->insert(line);
	}

	setVolume();

}

void cbNRAMsmeter()
{
	unsigned char data[18];
	char line[256];
	int chksum;

	setXcvrVolume(0.0);

	RequestData (cmdK_RSMTR, data, 18);
	txtDataDisp->insert("S meter calibration table");
	chksum = 0;
	for (int i = 0; i < 16; i++)
			chksum += data[i];
	if (chksum == 256*data[16]+data[17]) {
		for (int j = 0; j < 16; j++) {
			if (j % 8 == 0)
				txtDataDisp->insert("\n");
			sprintf(line, "%02x ", data[j]);
			txtDataDisp->insert(line);
		}
		txtDataDisp->insert("\n");
	}
	else {
		sprintf(line,"S meter cal. table: Check sum Error\n");
		txtDataDisp->insert(line);
	}

	setVolume();

}

void cbNRAMFreqRef()
{
	unsigned char data[34];
	char line[256];
	int chksum;

	setXcvrVolume(0.0);

	RequestData (cmdK_RFCAL, data, 34);
	txtDataDisp->insert("Freq. Ref. Cal. table");
	chksum = 0;
	for (int i = 0; i < 32; i++)
			chksum += data[i];
	if (chksum == 256*data[32]+data[33]) {
		for (int j = 0; j < 32; j++) {
			if (j % 8 == 0)
				txtDataDisp->insert("\n");
			sprintf(line, "%02x ", data[j]);
			txtDataDisp->insert(line);
		}
		txtDataDisp->insert("\n");
	}
	else {
		sprintf(line,"Freq. Ref. Cal. Check sum Error\n");
		txtDataDisp->insert(line);
	}

	setVolume();
}

void cbNRAMPhase()
{
	unsigned char data[18];
	char line[256];
	int chksum;

	setXcvrVolume(0.0);

	RequestData (cmdK_RPCAL, data, 18);
	txtDataDisp->insert("Phase Cal. table");
	chksum = 0;
	for (int i = 0; i < 16; i++)
			chksum += data[i];
	if (chksum == 256*data[16]+data[17]) {
		for (int j = 0; j < 16; j++) {
			if (j % 8 == 0)
				txtDataDisp->insert("\n");
			sprintf(line, "%02x ", data[j]);
			txtDataDisp->insert(line);
		}
		txtDataDisp->insert("\n");
	}
	else {
		sprintf(line,"Phase Cal. Check sum Error\n");
		txtDataDisp->insert(line);
	}

	setVolume();
}

void cbNRAMCarrier()
{
	unsigned char data[2];
	char line[256];

	setXcvrVolume(0.0);

	RequestData (cmdK_RCBAL, data, 2);
	txtDataDisp->insert("Carrier Balance value\n");
	sprintf(line, "%02x %02x", data[0], data[1]);
	txtDataDisp->insert(line);

	setVolume();
}

void cbNRAMClearText()
{
	txtDataBuffer.remove(0,txtDataBuffer.length());
	txtDataDisp->redraw();
}

void cbNRAMAll()
{
	cbNRAMAntImp();
	txtDataDisp->insert("\n");
	cbNRAMsmeter();
	txtDataDisp->insert("\n");
	cbNRAMFreqRef();
	txtDataDisp->insert("\n");
	cbNRAMPhase();
	txtDataDisp->insert("\n");
	cbNRAMCarrier();
}

void cbNRAMSave()
{
	txtDataBuffer.savefile("NRAMdata.txt");
}

void cbNRAMRestore()
{
}

void cb_vfo_adj()
{
	xcvrState.VFOADJ = ctr_vfo_adj->value();
	movFreq();
	movFreqB();
}

void show_controls()
{
	if (tabs->visible()) {
		tabs->hide();
		btn_show_controls->label("@-22->");
		btn_show_controls->redraw_label();
		window->size( window->w(), window->h() - tabs->h());
	} else {
		tabs->show();
		btn_show_controls->label("@-28->");
		btn_show_controls->redraw_label();
		window->size( window->w(), window->h() + tabs->h());
	}
	window->redraw();
	setFocus();
}

void cbEventLog()
{
	debug::show();
	setFocus();
}

// Frequency display font / colors
Fl_Font selfont;

void cbFreqControlFontBrowser(Fl_Widget*, void*) {
	selfont = fntbrowser->fontNumber();
	lblTest->labelfont(selfont);
	dlgDisplayConfig->redraw();
	FreqDisp->font(selfont);
	FreqDispB->font(selfont);
	fntbrowser->hide();
}

void cbPrefFont()
{
	if (!fntbrowser) fntbrowser = new Font_Browser;

	fntbrowser->fontNumber(xcvrState.fontnbr);
	fntbrowser->fontFilter(Font_Browser::FIXED_WIDTH);
	fntbrowser->fontFilter(Font_Browser::ALL_TYPES);
	fntbrowser->callback(cbFreqControlFontBrowser);
	fntbrowser->show();
}

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar smeterRed, smeterGreen, smeterBlue;
uchar peakRed, peakGreen, peakBlue;
uchar pwrRed, pwrGreen, pwrBlue;
uchar swrRed, swrGreen, swrBlue;

Fl_Color bgclr;
Fl_Color fgclr;

Fl_Color fgsys;
static uchar fg_sys_red, fg_sys_green, fg_sys_blue;

Fl_Color bgsys;
static uchar bg_sys_red, bg_sys_green, bg_sys_blue;

Fl_Color bg2sys;
static uchar bg2_sys_red, bg2_sys_green, bg2_sys_blue;

Fl_Color bg_slider;
static uchar bg_slider_red, bg_slider_green, bg_slider_blue;

Fl_Color btn_slider;
static uchar btn_slider_red, btn_slider_green, btn_slider_blue;

Fl_Color btn_lt_color;
static uchar btn_lt_color_red, btn_lt_color_green, btn_lt_color_blue;

void cb_lighted_button()
{
	uchar r = btn_lt_color_red, g = btn_lt_color_green, b = btn_lt_color_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		btn_lt_color_red = r; btn_lt_color_green = g; btn_lt_color_blue = b;
		btn_lt_color = fl_rgb_color(r, g, b);
		btn_lighted->selection_color(btn_lt_color);
		btn_lighted->value(1);
		btn_lighted->redraw();
	}
}

void cb_lighted_default()
{
	btn_lt_color = flrig_def_color(FL_YELLOW);
	btn_lt_color_red = ((btn_lt_color >> 24) & 0xFF);
	btn_lt_color_green = ((btn_lt_color >> 16) & 0xFF);
	btn_lt_color_blue = ((btn_lt_color >> 8) & 0xFF);
	btn_lighted->selection_color(btn_lt_color);
	btn_lighted->value(1);
	btn_lighted->redraw();
}

void cb_slider_defaults()
{
	bg_slider_red = 232;
	bg_slider_green = 255;
	bg_slider_blue = 232;

	btn_slider_red = 0;
	btn_slider_green = 0;
	btn_slider_blue = 128;

	bg_slider = fl_rgb_color( 232, 255, 232);
	btn_slider = fl_rgb_color( 0, 0, 128);

	sldrColors->color(bg_slider);
	sldrColors->selection_color(btn_slider);
	sldrColors->redraw();
}

void cb_slider_background()
{
	uchar r = bg_slider_red, g = bg_slider_green, b = bg_slider_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		bg_slider_red = r; bg_slider_green = g; bg_slider_blue = b;
		bg_slider = fl_rgb_color(r, g, b);
		sldrColors->color(bg_slider);
		sldrColors->selection_color(btn_slider);
		sldrColors->redraw();
	}
}

void cb_slider_select()
{
	uchar r = btn_slider_red, g = btn_slider_green, b = btn_slider_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		btn_slider_red = r; btn_slider_green = g; btn_slider_blue = b;
		btn_slider = fl_rgb_color(r, g, b);
		sldrColors->color(bg_slider);
		sldrColors->selection_color(btn_slider);
		sldrColors->redraw();
	}
}

void cb_sys_defaults()
{
	bgsys = flrig_def_color(FL_BACKGROUND_COLOR);
	bg_sys_red = ((bgsys >> 24) & 0xFF);
	bg_sys_green = ((bgsys >> 16) & 0xFF);
	bg_sys_blue = ((bgsys >> 8) & 0xFF);

	bg2sys = flrig_def_color(FL_BACKGROUND2_COLOR);
	bg2_sys_red = ((bg2sys) >> 24 & 0xFF);
	bg2_sys_green = ((bg2sys) >> 16 & 0xFF);
	bg2_sys_blue = ((bg2sys) >> 8 & 0xFF);

	fgsys = flrig_def_color(FL_FOREGROUND_COLOR);
	fg_sys_red = (fgsys >> 24) & 0xFF;
	fg_sys_green = (fgsys >> 16) & 0xFF;
	fg_sys_blue = (fgsys >> 8) & 0xFF;

	Fl::background(bg_sys_red, bg_sys_green, bg_sys_blue);
	Fl::background2(bg2_sys_red, bg2_sys_green, bg2_sys_blue);
	Fl::foreground(fg_sys_red, fg_sys_green, fg_sys_blue);

	dlgDisplayConfig->redraw();
	window->redraw();
}

void cb_sys_foreground()
{
	uchar r = fg_sys_red, g = fg_sys_green, b = fg_sys_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		fg_sys_red = r; fg_sys_green = g; fg_sys_blue = b;
		fgsys = fl_rgb_color(r, g, b);
		Fl::foreground(r, g, b);
		dlgDisplayConfig->redraw();
		window->redraw();
	}
}

void cb_sys_background()
{
	uchar r = bg_sys_red, g = bg_sys_green, b = bg_sys_blue;
	if (fl_color_chooser("Background color", r, g, b)) {
		bg_sys_red = r; bg_sys_green = g; bg_sys_blue = b;
		bgsys = fl_rgb_color(r, g, b);
		Fl::background(r, g, b);
		dlgDisplayConfig->redraw();
		window->redraw();
	}
}

void cb_sys_background2()
{
	uchar r = bg2_sys_red, g = bg2_sys_green, b = bg2_sys_blue;
	if (fl_color_chooser("Background2 color", r, g, b)) {
		bg2_sys_red = r; bg2_sys_green = g; bg2_sys_blue = b;
		bg2sys = fl_rgb_color(r, g, b);
		Fl::background2(r, g, b);
		dlgDisplayConfig->redraw();
		window->redraw();
	}
}

void cbBacklightColor()
{
	uchar r = bg_red, g = bg_green, b = bg_blue;
	if (fl_color_chooser("Background color", r, g, b)) {
		bg_red = r; bg_green = g; bg_blue = b;
		bgclr = fl_rgb_color(r, g, b);
		lblTest->color(bgclr);
		sldrRcvSignaldisp->color( fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr );
		sldrFwdPwrdisp->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
		sldrRefPwrdisp->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
		btnSmeterdisp->color(bgclr);
		btnPowerdisp->color(bgclr);
		btnSWRdisp->color(bgclr);
		grpMeter1disp->color(bgclr);
		grpMeter2disp->color(bgclr);
		dlgDisplayConfig->redraw();
	}
}

void cbPrefForeground()
{
	uchar r = fg_red, g = fg_green, b = fg_blue;
	if (fl_color_chooser("Foreground color", r, g, b)) {
		fg_red = r; fg_green = g; fg_blue = b;
		fgclr = fl_rgb_color(r, g, b);
		lblTest->labelcolor(fgclr);
		btnSmeterdisp->labelcolor(fgclr);
		sldrFwdPwrdisp->labelcolor(fgclr);
		sldrRefPwrdisp->labelcolor(fgclr);
		btnSmeterdisp->labelcolor(fgclr);
		btnPowerdisp->labelcolor(fgclr);
		btnSWRdisp->labelcolor(fgclr);
		grpMeter1disp->labelcolor(fgclr);
		grpMeter2disp->labelcolor(fgclr);
		dlgDisplayConfig->redraw();
	}
}

void default_meters()
{
	Fl_Color c;
	bg_red = 232; bg_green = 255; bg_blue = 232;
	bgclr = fl_rgb_color( bg_red, bg_green, bg_blue);
		lblTest->color(bgclr);
		sldrRcvSignaldisp->color( fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr );
		sldrFwdPwrdisp->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
		sldrRefPwrdisp->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
		btnSmeterdisp->color(bgclr, bgclr);
		btnPowerdisp->color(bgclr, bgclr);
		btnSWRdisp->color(bgclr, bgclr);
		grpMeter1disp->color(bgclr);
		grpMeter2disp->color(bgclr);
	fg_red = 0; fg_green = 0; fg_blue = 0;
	fgclr = (Fl_Color)0;
		lblTest->labelcolor(fgclr);
		btnSmeterdisp->labelcolor(fgclr);
		btnPowerdisp->labelcolor(fgclr);
		btnSWRdisp->labelcolor(fgclr);
		grpMeter1disp->labelcolor(fgclr);
		grpMeter2disp->labelcolor(fgclr);
	smeterRed = 0; smeterGreen = 180; smeterBlue = 0;
		c = fl_rgb_color (smeterRed, smeterGreen, smeterBlue);
		sldrRcvSignaldisp->color(c, bgclr );
	peakRed = 255; peakGreen = 0; peakBlue = 0;
		c = fl_rgb_color( peakRed, peakGreen, peakBlue );
		sldrRcvSignaldisp->PeakColor(c);
		sldrFwdPwrdisp->PeakColor(c);
		sldrRefPwrdisp->PeakColor(c);
	pwrRed = 180; pwrGreen = 0; pwrBlue = 0;
		c = fl_rgb_color( pwrRed, pwrGreen, pwrBlue );
		sldrFwdPwrdisp->color(c, bgclr);
	swrRed = 148; swrGreen = 0; swrBlue = 148;
		c = fl_rgb_color(swrRed, swrGreen, swrBlue);
		sldrRefPwrdisp->color(c, bgclr);
	dlgDisplayConfig->redraw();
}

void cbSMeterColor()
{
	uchar r = smeterRed, g = smeterGreen, b = smeterBlue;
	if (fl_color_chooser("S Meter color", r, g, b)) {
		smeterRed = r; smeterGreen = g; smeterBlue = b;
		sldrRcvSignaldisp->color(
			fl_rgb_color (r, g, b),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cbPeakMeterColor()
{
	uchar r = peakRed, g = peakGreen, b = peakBlue;
	if (fl_color_chooser("Peak value color", r, g, b)) {
		peakRed = r; peakGreen = g; peakBlue = b;
		sldrRcvSignaldisp->PeakColor(fl_rgb_color (r, g, b));
		sldrFwdPwrdisp->PeakColor(fl_rgb_color (r, g, b));
		sldrRefPwrdisp->PeakColor(fl_rgb_color (r, g, b));
		dlgDisplayConfig->redraw();
	}
}

void cbPwrMeterColor()
{
	uchar r = pwrRed, g = pwrGreen, b = pwrBlue;
	if (fl_color_chooser("Power color", r, g, b)) {
		pwrRed = r; pwrGreen = g; pwrBlue = b;
		sldrFwdPwrdisp->color(
			fl_rgb_color (r, g, b),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void cbSWRMeterColor()
{
	uchar r = swrRed, g = swrGreen, b = swrBlue;
	if (fl_color_chooser("Power color", r, g, b)) {
		swrRed = r; swrGreen = g; swrBlue = b;
		sldrRefPwrdisp->color(
			fl_rgb_color (swrRed, swrGreen, swrBlue),
			bgclr );
		dlgDisplayConfig->redraw();
	}
}

void setColors()
{
	bgclr = fl_rgb_color(bg_red, bg_green, bg_blue);

	xcvrState.swrRed = swrRed;
	xcvrState.swrGreen = swrGreen;
	xcvrState.swrBlue = swrBlue;

	xcvrState.pwrRed = pwrRed;
	xcvrState.pwrGreen = pwrGreen;
	xcvrState.pwrBlue = pwrBlue;

	xcvrState.smeterRed = smeterRed;
	xcvrState.smeterGreen = smeterGreen;
	xcvrState.smeterBlue = smeterBlue;

	xcvrState.peakRed = peakRed;
	xcvrState.peakGreen = peakGreen;
	xcvrState.peakBlue = peakBlue;

	xcvrState.fg_red = fg_red;
	xcvrState.fg_green = fg_green;
	xcvrState.fg_blue = fg_blue;

	xcvrState.bg_red = bg_red;
	xcvrState.bg_green = bg_green;
	xcvrState.bg_blue = bg_blue;

	xcvrState.fontnbr = selfont;
	FreqDisp->font(selfont);
	FreqDispB->font(selfont);

	xcvrState.fg_sys_red = fg_sys_red;
	xcvrState.fg_sys_green = fg_sys_green;
	xcvrState.fg_sys_blue = fg_sys_blue;

	xcvrState.bg_sys_red = bg_sys_red;
	xcvrState.bg_sys_green = bg_sys_green;
	xcvrState.bg_sys_blue = bg_sys_blue;

	xcvrState.bg2_sys_red = bg2_sys_red;
	xcvrState.bg2_sys_green = bg2_sys_green;
	xcvrState.bg2_sys_blue = bg2_sys_blue;

	xcvrState.slider_red = bg_slider_red;
	xcvrState.slider_green = bg_slider_green;
	xcvrState.slider_blue = bg_slider_blue;

	xcvrState.slider_btn_red = btn_slider_red;
	xcvrState.slider_btn_green = btn_slider_green;
	xcvrState.slider_btn_blue = btn_slider_blue;

	xcvrState.lighted_btn_red = btn_lt_color_red;
	xcvrState.lighted_btn_green = btn_lt_color_green;
	xcvrState.lighted_btn_blue = btn_lt_color_blue;

	if (rx_on_a) {
		FreqDisp->SetONOFFCOLOR( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDispB->SetONOFFCOLOR(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	} else {
		FreqDispB->SetONOFFCOLOR( fl_rgb_color(fg_red, fg_green, fg_blue), bgclr);
		FreqDisp->SetONOFFCOLOR(
			fl_rgb_color(fg_red, fg_green, fg_blue),
			fl_color_average(bgclr, FL_BLACK, 0.87));
	}

	grpMeters->color(bgclr);
	grpMeters->labelcolor(fgclr);

	btnSmeter->color(bgclr, bgclr);
	btnSmeter->labelcolor(fgclr);

	btnPower->color(bgclr, bgclr);
	btnPower->labelcolor(fgclr);

	boxSWR->color(bgclr, bgclr);
	boxSWR->labelcolor(fgclr);

	sldrFwdPwr->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
	sldrFwdPwr->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrRcvSignal->color(fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr);
	sldrRcvSignal->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrRefPwr->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrRefPwr->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	grpMeters->redraw();

	btnVol->selection_color(btn_lt_color);
	btnNR->selection_color(btn_lt_color);
	btnIFsh->selection_color(btn_lt_color);
	btnNotch->selection_color(btn_lt_color);
	btnAttenuator->selection_color(btn_lt_color);
	btnPreamp->selection_color(btn_lt_color);
	btnNR->selection_color(btn_lt_color);
	btnNotch->selection_color(btn_lt_color);
	btnTune->selection_color(btn_lt_color);
	btnPTT->selection_color(btn_lt_color);
	btnSPOT->selection_color(btn_lt_color);
	btnVoxOnOff->selection_color(btn_lt_color);

	sldrVOLUME->color(bg_slider);
	sldrVOLUME->selection_color(btn_slider);
	sldrRIT->color(bg_slider);
	sldrRIT->selection_color(btn_slider);
	sldrIFSHIFT->color(bg_slider);
	sldrIFSHIFT->selection_color(btn_slider);
	sldrNR->color(bg_slider);
	sldrNR->selection_color(btn_slider);
	sldrMICGAIN->color(bg_slider);
	sldrMICGAIN->selection_color(btn_slider);
	sldrNOTCH->color(bg_slider);
	sldrNOTCH->selection_color(btn_slider);
	sldrDepth->color(bg_slider);
	sldrDepth->selection_color(btn_slider);
	sldrPOWER->color(bg_slider);
	sldrPOWER->selection_color(btn_slider);

	window->redraw();
}

void cb_reset_display_dialog()
{
	cb_sys_defaults();
	cb_lighted_default();
	cb_slider_defaults();
	default_meters();
	setColors();
}

void cbOkDisplayDialog()
{
	setColors();
	dlgDisplayConfig->hide();
}

void cbCancelDisplayDialog()
{
	dlgDisplayConfig->hide();
}

void setDisplayColors()
{
	if (dlgDisplayConfig == NULL)
		dlgDisplayConfig = DisplayDialog();

	swrRed = xcvrState.swrRed;
	swrGreen = xcvrState.swrGreen;
	swrBlue = xcvrState.swrBlue;

	pwrRed = xcvrState.pwrRed;
	pwrGreen = xcvrState.pwrGreen;
	pwrBlue = xcvrState.pwrBlue;

	smeterRed = xcvrState.smeterRed;
	smeterGreen = xcvrState.smeterGreen;
	smeterBlue = xcvrState.smeterBlue;

	peakRed = xcvrState.peakRed;
	peakGreen = xcvrState.peakGreen;
	peakBlue = xcvrState.peakBlue;

	fg_red = xcvrState.fg_red;
	fg_green = xcvrState.fg_green;
	fg_blue = xcvrState.fg_blue;

	bg_red = xcvrState.bg_red;
	bg_green = xcvrState.bg_green;
	bg_blue = xcvrState.bg_blue;

	bgclr = fl_rgb_color(bg_red, bg_green, bg_blue);
	fgclr = fl_rgb_color(fg_red, fg_green, fg_blue);

	fg_sys_red = xcvrState.fg_sys_red;
	fg_sys_green = xcvrState.fg_sys_green;
	fg_sys_blue = xcvrState.fg_sys_blue;

	bg_sys_red = xcvrState.bg_sys_red;
	bg_sys_green = xcvrState.bg_sys_green;
	bg_sys_blue = xcvrState.bg_sys_blue;

	bg2_sys_red = xcvrState.bg2_sys_red;
	bg2_sys_green = xcvrState.bg2_sys_green;
	bg2_sys_blue = xcvrState.bg2_sys_blue;

	bg_slider_red = xcvrState.slider_red;
	bg_slider_green = xcvrState.slider_green;
	bg_slider_blue = xcvrState.slider_blue;

	btn_slider_red = xcvrState.slider_btn_red;
	btn_slider_green = xcvrState.slider_btn_green;
	btn_slider_blue = xcvrState.slider_btn_blue;

	sldrColors->color(fl_rgb_color(bg_slider_red, bg_slider_green, bg_slider_blue));
	sldrColors->selection_color(fl_rgb_color(btn_slider_red, btn_slider_green, btn_slider_blue));

	btn_lt_color_red = xcvrState.lighted_btn_red;
	btn_lt_color_green = xcvrState.lighted_btn_green;
	btn_lt_color_blue = xcvrState.lighted_btn_blue;

	lblTest->labelcolor(fl_rgb_color(fg_red, fg_green, fg_blue));
	lblTest->color(bgclr);

	btnSmeterdisp->color(bgclr);
	btnSmeterdisp->labelcolor(fgclr);
	btnPowerdisp->color(bgclr);
	btnPowerdisp->labelcolor(fgclr);
	btnSWRdisp->color(bgclr);
	btnSWRdisp->labelcolor(fgclr);
	grpMeter1disp->color(bgclr);
	grpMeter1disp->labelcolor(fgclr);
	grpMeter2disp->color(bgclr);
	grpMeter2disp->labelcolor(fgclr);

	sldrRcvSignaldisp->color(
		fl_rgb_color (smeterRed, smeterGreen, smeterBlue),
		bgclr );

	sldrFwdPwrdisp->color(
		fl_rgb_color (pwrRed, pwrGreen, pwrBlue),
		bgclr );
	sldrRefPwrdisp->color(
		fl_rgb_color (swrRed, swrGreen, swrBlue),
		bgclr );

	sldrRcvSignaldisp->minimum(0);
	sldrRcvSignaldisp->maximum(100);
	sldrRcvSignaldisp->value(25);

	sldrFwdPwrdisp->minimum(0);
	sldrFwdPwrdisp->maximum(100);
	sldrFwdPwrdisp->value(25);

	sldrRefPwrdisp->minimum(0);
	sldrRefPwrdisp->maximum(100);
	sldrRefPwrdisp->value(25);

	btn_lt_color = fl_rgb_color( btn_lt_color_red, btn_lt_color_green, btn_lt_color_blue);
	btn_slider = fl_rgb_color( btn_slider_red, btn_slider_green, btn_slider_blue);
	bg_slider = fl_rgb_color(bg_slider_red, bg_slider_green, bg_slider_blue);

	btn_lighted->value(1);
	btn_lighted->selection_color(btn_lt_color);

	sldrColors->color(bg_slider);
	sldrColors->selection_color(btn_slider);

	selfont = xcvrState.fontnbr;

	mnuScheme->value(mnuScheme->find_item(xcvrState.ui_scheme.c_str()));

	dlgDisplayConfig->show();
}

// a replica of the default color map used by Fltk

static unsigned flrig_cmap[256] = {
	0x00000000,
	0xff000000,
	0x00ff0000,
	0xffff0000,
	0x0000ff00,
	0xff00ff00,
	0x00ffff00,
	0xffffff00,
	0x55555500,
	0xc6717100,
	0x71c67100,
	0x8e8e3800,
	0x7171c600,
	0x8e388e00,
	0x388e8e00,
	0x00008000,
	0xa8a89800,
	0xe8e8d800,
	0x68685800,
	0x98a8a800,
	0xd8e8e800,
	0x58686800,
	0x9c9ca800,
	0xdcdce800,
	0x5c5c6800,
	0x9ca89c00,
	0xdce8dc00,
	0x5c685c00,
	0x90909000,
	0xc0c0c000,
	0x50505000,
	0xa0a0a000,
	0x00000000,
	0x0d0d0d00,
	0x1a1a1a00,
	0x26262600,
	0x31313100,
	0x3d3d3d00,
	0x48484800,
	0x55555500,
	0x5f5f5f00,
	0x6a6a6a00,
	0x75757500,
	0x80808000,
	0x8a8a8a00,
	0x95959500,
	0xa0a0a000,
	0xaaaaaa00,
	0xb5b5b500,
	0xc0c0c000,
	0xcbcbcb00,
	0xd5d5d500,
	0xe0e0e000,
	0xeaeaea00,
	0xf5f5f500,
	0xffffff00,
	0x00000000,
	0x00240000,
	0x00480000,
	0x006d0000,
	0x00910000,
	0x00b60000,
	0x00da0000,
	0x00ff0000,
	0x3f000000,
	0x3f240000,
	0x3f480000,
	0x3f6d0000,
	0x3f910000,
	0x3fb60000,
	0x3fda0000,
	0x3fff0000,
	0x7f000000,
	0x7f240000,
	0x7f480000,
	0x7f6d0000,
	0x7f910000,
	0x7fb60000,
	0x7fda0000,
	0x7fff0000,
	0xbf000000,
	0xbf240000,
	0xbf480000,
	0xbf6d0000,
	0xbf910000,
	0xbfb60000,
	0xbfda0000,
	0xbfff0000,
	0xff000000,
	0xff240000,
	0xff480000,
	0xff6d0000,
	0xff910000,
	0xffb60000,
	0xffda0000,
	0xffff0000,
	0x00003f00,
	0x00243f00,
	0x00483f00,
	0x006d3f00,
	0x00913f00,
	0x00b63f00,
	0x00da3f00,
	0x00ff3f00,
	0x3f003f00,
	0x3f243f00,
	0x3f483f00,
	0x3f6d3f00,
	0x3f913f00,
	0x3fb63f00,
	0x3fda3f00,
	0x3fff3f00,
	0x7f003f00,
	0x7f243f00,
	0x7f483f00,
	0x7f6d3f00,
	0x7f913f00,
	0x7fb63f00,
	0x7fda3f00,
	0x7fff3f00,
	0xbf003f00,
	0xbf243f00,
	0xbf483f00,
	0xbf6d3f00,
	0xbf913f00,
	0xbfb63f00,
	0xbfda3f00,
	0xbfff3f00,
	0xff003f00,
	0xff243f00,
	0xff483f00,
	0xff6d3f00,
	0xff913f00,
	0xffb63f00,
	0xffda3f00,
	0xffff3f00,
	0x00007f00,
	0x00247f00,
	0x00487f00,
	0x006d7f00,
	0x00917f00,
	0x00b67f00,
	0x00da7f00,
	0x00ff7f00,
	0x3f007f00,
	0x3f247f00,
	0x3f487f00,
	0x3f6d7f00,
	0x3f917f00,
	0x3fb67f00,
	0x3fda7f00,
	0x3fff7f00,
	0x7f007f00,
	0x7f247f00,
	0x7f487f00,
	0x7f6d7f00,
	0x7f917f00,
	0x7fb67f00,
	0x7fda7f00,
	0x7fff7f00,
	0xbf007f00,
	0xbf247f00,
	0xbf487f00,
	0xbf6d7f00,
	0xbf917f00,
	0xbfb67f00,
	0xbfda7f00,
	0xbfff7f00,
	0xff007f00,
	0xff247f00,
	0xff487f00,
	0xff6d7f00,
	0xff917f00,
	0xffb67f00,
	0xffda7f00,
	0xffff7f00,
	0x0000bf00,
	0x0024bf00,
	0x0048bf00,
	0x006dbf00,
	0x0091bf00,
	0x00b6bf00,
	0x00dabf00,
	0x00ffbf00,
	0x3f00bf00,
	0x3f24bf00,
	0x3f48bf00,
	0x3f6dbf00,
	0x3f91bf00,
	0x3fb6bf00,
	0x3fdabf00,
	0x3fffbf00,
	0x7f00bf00,
	0x7f24bf00,
	0x7f48bf00,
	0x7f6dbf00,
	0x7f91bf00,
	0x7fb6bf00,
	0x7fdabf00,
	0x7fffbf00,
	0xbf00bf00,
	0xbf24bf00,
	0xbf48bf00,
	0xbf6dbf00,
	0xbf91bf00,
	0xbfb6bf00,
	0xbfdabf00,
	0xbfffbf00,
	0xff00bf00,
	0xff24bf00,
	0xff48bf00,
	0xff6dbf00,
	0xff91bf00,
	0xffb6bf00,
	0xffdabf00,
	0xffffbf00,
	0x0000ff00,
	0x0024ff00,
	0x0048ff00,
	0x006dff00,
	0x0091ff00,
	0x00b6ff00,
	0x00daff00,
	0x00ffff00,
	0x3f00ff00,
	0x3f24ff00,
	0x3f48ff00,
	0x3f6dff00,
	0x3f91ff00,
	0x3fb6ff00,
	0x3fdaff00,
	0x3fffff00,
	0x7f00ff00,
	0x7f24ff00,
	0x7f48ff00,
	0x7f6dff00,
	0x7f91ff00,
	0x7fb6ff00,
	0x7fdaff00,
	0x7fffff00,
	0xbf00ff00,
	0xbf24ff00,
	0xbf48ff00,
	0xbf6dff00,
	0xbf91ff00,
	0xbfb6ff00,
	0xbfdaff00,
	0xbfffff00,
	0xff00ff00,
	0xff24ff00,
	0xff48ff00,
	0xff6dff00,
	0xff91ff00,
	0xffb6ff00,
	0xffdaff00,
	0xffffff00
};

Fl_Color flrig_def_color(int n)
{
	if ( n > 255 ) n = 255;
	if (n < 0) n = 0;
	return (Fl_Color)flrig_cmap[n];
}

//======================================================================
// scanner
//======================================================================

extern int rxsignal;
int scanrange = 5000;
int range[] = {2500, 5000, 10000, 25000, 50000, 100000};
char szrange[] = "2500|5000|10000|25000|50000|10000";
int startfreq = 14070000;
int endfreq = 14070000;
int scanfreq = 14070000;
XYplot::line rxp = {0,0,0,0,FL_YELLOW};

double dbmin, dbmax;
static bool scanning = false;
static bool continuous_scan = false;

void set_freq_range()
{
	scanrange = range[scan_range->value()];
	spectrum_plot->clear();
}

void update_scanner(int d)
{
	if (!scanning) return;

	rxp.x2 = scanfreq; rxp.y2 = d;
	if (scanfreq == startfreq) { rxp.x1 = rxp.x2; rxp.y1 = rxp.y2;}
	startFreqDisp->value(scanfreq);
	spectrum_plot->add_line(rxp);
	rxp.x1 = rxp.x2; rxp.y1 = rxp.y2;
	spectrum_plot->redraw();
	if (!scanning) goto done;

	scanfreq += 50;
	if (scanfreq > endfreq) {
		if (!continuous_scan) goto done;
		scanfreq = startfreq;
	}
	setXcvrRcvFreq(scanfreq);
	return;

done:
	scanning = false;
	continuous_scan = false;
	startFreqDisp->value(startfreq);
	if (rx_on_a) {
		if (tx_on_a)
			cbRxA_TxA();
		else
			cbRxA_TxB();
	} else {
		if (tx_on_a)
			cbRxB_TxA();
		else
			cbRxB_TxB();
	}
}

void draw_axis()
{
	spectrum_plot->clear_axis();

	XYplot::line axis = {0,0,0,0,FL_GRAY};

	dbmin = -60.0 -10.0 * db_min->value();
	dbmax = -10.0 * db_max->value();
	spectrum_plot->xmin_max(startfreq, startfreq + scanrange);
	spectrum_plot->ymin_max(dbmin, dbmax);

	axis.x1 = axis.x2 = startfreq;
	axis.y1 = dbmin; axis.y2 = dbmax;
	for (int i = 1; i < 10; i++) {
		axis.x1 = (axis.x2 += scanrange/10.0);
		spectrum_plot->add_axis(axis);
	}
	axis.x1 = startfreq; axis.x2 = startfreq + scanrange;
	axis.y1 = axis.y2 = dbmax;
	while (axis.y1 > dbmin) {
		axis.y2 = (axis.y1 -= 10);
		spectrum_plot->add_axis(axis);
	}
	spectrum_plot->redraw();
}

void start_scan()
{
	if (scanning) return;

	startfreq = startFreqDisp->value();
	scanrange = range[scan_range->value()];
	endfreq = startfreq + scanrange;

	spectrum_plot->clear();

	XYplot::line axis = {0,0,0,0,FL_GRAY};

	dbmin = -60.0 -10.0 * db_min->value();
	dbmax = -10.0 * db_max->value();
	spectrum_plot->xmin_max(startfreq, startfreq + scanrange);
	spectrum_plot->ymin_max(dbmin, dbmax);

	axis.x1 = axis.x2 = startfreq;
	axis.y1 = dbmin; axis.y2 = dbmax;
	for (int i = 1; i < 10; i++) {
		axis.x1 = (axis.x2 += scanrange/10.0);
		spectrum_plot->add_axis(axis);
	}
	axis.x1 = startfreq; axis.x2 = startfreq + scanrange;
	axis.y1 = axis.y2 = dbmax;
	while (axis.y1 > dbmin) {
		axis.y2 = (axis.y1 -= 10);
		spectrum_plot->add_axis(axis);
	}
	spectrum_plot->redraw();

	scanfreq = startfreq;
	setXcvrRcvFreq(scanfreq);
	setXcvrMode(CW);
	setXcvrBW(0);
	scanning = true;
}

void start_continuous_scan()
{
	if (!scanning) {
		continuous_scan = true;
		start_scan();
	}
}

void stop_scan()
{
	scanning = false;
	continuous_scan = false;
	startFreqDisp->value(startfreq);
	if (rx_on_a) {
		if (tx_on_a)
			cbRxA_TxA();
		else
			cbRxA_TxB();
	} else {
		if (tx_on_a)
			cbRxB_TxA();
		else
			cbRxB_TxB();
	}
}

int startFreq()
{
	scanning = false;
	spectrum_plot->clear();
	startfreq = startFreqDisp->value();
	return 0;
}

int db_min_cb()
{
	draw_axis();
	return 0;
}

int db_max_cb()
{
	draw_axis();
	return 0;
}

int scanner_cb()
{
	int xpos = Fl::event_x();
	int xleft = spectrum_plot->x();
	int freq = ceil(((xpos - xleft) * scanrange / spectrum_plot->w())/100.0);
	if (rx_on_a) {
		FreqDisp->value(startfreq + freq * 100);
		movFreq();
	} else {
		FreqDispB->value(startfreq + freq * 100);
		movFreqB();
	}
	return 0;
}

char *format_tooltip(double x, double y)
{
	static char tip[80];
	snprintf(tip, sizeof(tip), "%.1f KHz\n%.0f dB", ceil(x/100.0)/10.0, y);
	return tip;
}

void open_scanner()
{
	if (!dlgScanner) {
		dlgScanner = scanner_window();
		startFreqDisp->value(startfreq);
		scan_range->clear();
		scan_range->add(szrange);
		scan_range->value(1);
		scanrange = range[1];
		db_min->value(7);
		db_max->value(0);
		dbmin = -60.0 -10.0 * db_min->value();
		dbmax = -10.0 * db_max->value();
		spectrum_plot->xmin_max(startfreq, startfreq + scanrange);
		spectrum_plot->ymin_max(dbmin, dbmax);
		spectrum_plot->setCallback(scanner_cb);
		spectrum_plot->enable_tooltip(true);
		spectrum_plot->set_tooltip(format_tooltip);
		draw_axis();
	}
	dlgScanner->show();
}
