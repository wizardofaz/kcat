#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "support.h"
#include "kcat.h"
#include "kcat_io.h"
#include "util.h"
#include "config.h"
#include "xml_io.h"
#include "status.h"
#include "debug.h"
#include "threads.h"

using namespace std;

extern bool test;

extern void resetWatchDog();

const char *szmodes[] = {"LSB", "USB", "CW", "AM", "FM", NULL};
const char modetype[] = {'L', 'U', 'L', 'U', 'U'};

const char *szBW[] =  {
"100", "200", "500", "1000", "1700", "2100", "2400", "2700", "3500","DataM", "DataH", NULL};

int iBW[] = {
0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x0b, 0x0a };

const char *szNotch[] = {"Wide", "Med", "Nar", NULL};

FREQMODE vfoA = {14070000, USB, 7, UI };
FREQMODE vfoB = { 3580000, USB, 7, UI };
FREQMODE xmlvfo = vfoA;

char szVfoB[12];

FREQMODE oplist[LISTSIZE];
int  numinlist = 0;

stANTPORT antports[LISTSIZE] = {
	{1800, 0, 0},
	{3500, 0, 0},
	{7000, 0, 0},
	{10100, 0, 0},
	{14000, 0, 0},
	{18068, 0, 0},
	{21000, 0, 0},
	{24890, 0, 0},
	{28000, 0, 0} };
int numantports = 9;

CSerialComm kcatSerial;

bool rx_on_a = true;
bool tx_on_a = true;

char *print(FREQMODE data)
{
	static char str[100];
	snprintf(str, sizeof(str), "%3s,%10ld,%4s,%5s",
		data.src == XML ? "xml" : "ui",
		data.freq,
		szmodes[data.imode],
		szBW[data.iBW]);
	return str;
}

void initkcat()
{
	if (!dlgAntPorts)
		dlgAntPorts  = FreqRangesDialog();

	GetKachinaVersion();
	initXcvrState();
	cbRxA_TxA();
}

void initOptionMenus()
{
	const char **p;
	p = szmodes;
	while (*p) 
		opMODE->add(*p++);
	opMODE->value(vfoA.imode);
	p = szBW;
	while (*p) 
		opBW->add(*p++);
	opBW->value(vfoA.iBW);
	p = szNotch;
	while (*p)
		opNOTCH->add(*p++);
	opNOTCH->value(3);
}

// enables or disables controls based on mode selected
// see command inhibit table in IOspec.cpp

void setInhibits() {
	switch (opMODE->value()) {
	case AM:
	case FM:
		btnPTT->activate();
		opBW->deactivate();
		btnIFsh->deactivate();
		sldrNOTCH->deactivate();
		sldrDepth->deactivate();
		btnNotch->deactivate();
		opNOTCH->deactivate();
		btnNR->deactivate();
		sldrNR->deactivate();
		sldrAgcSpeed->deactivate();
		sldrAgcAction->deactivate();
		break;
	case CW:
		btnPTT->deactivate();
		opBW->activate();
		btnIFsh->activate();
		sldrNOTCH->activate();
		sldrDepth->activate();
		btnNotch->activate();
		opNOTCH->activate();
		btnNR->activate();
		sldrNR->activate();
		sldrAgcSpeed->activate();
		sldrAgcAction->activate();
		break;
	case USB:
	case LSB:
		btnPTT->activate();
		opBW->activate();
		btnIFsh->activate();
		sldrNOTCH->activate();
		sldrDepth->activate();
		btnNotch->activate();
		opNOTCH->activate();
		btnNR->activate();
		sldrNR->activate();
		sldrAgcSpeed->activate();
		sldrAgcAction->activate();
		break;
	default: ;
	}
	cbIFsh();
	cbbtnNR();

}

void setFocus()
{
//	if (dlgAntPorts != NULL && dlgAntPorts->visible())
//		return;
	if (rx_on_a) Fl::focus(FreqDisp);
	else         Fl::focus(FreqDispB);
}

void setBW() {
	if (tx_on_a) {
		vfoA.iBW = opBW->value();
		setXcvrBW (vfoA.iBW);
		send_new_bandwidth(vfoA.iBW);
	} else {
		vfoB.iBW = opBW->value();
		setXcvrBW (vfoB.iBW);
		send_new_bandwidth(vfoB.iBW);
	}
	setIFshift();
}

MODES current_mode = AM;

void setMode() 
{
	MODES new_mode = (MODES)opMODE->value();
	setXcvrMode (new_mode);
	if (tx_on_a && (vfoA.imode != new_mode)) {
		switch (opMODE->value()) {
			case AM: opBW->value(8); setBW(); break;
			case FM: opBW->value(8); setBW(); break;
			case CW: opBW->value(2); setBW(); break;
			default: opBW->value(7); setBW(); break;
		}
		setInhibits();
		vfoA.imode = new_mode;
		send_new_mode(vfoA.imode);
	} else if (!tx_on_a && (vfoB.imode != new_mode)) {
		switch (opMODE->value()) {
			case AM: opBW->value(8); setBW(); break;
			case FM: opBW->value(8); setBW(); break;
			case CW: opBW->value(2); setBW(); break;
			default: opBW->value(7); setBW(); break;
		}
		setInhibits();
		vfoB.imode = new_mode;
		send_new_mode(vfoB.imode);
	} else
		setBW();
}

void sortList() {
	if (!numinlist) return;
	FREQMODE temp;
	for (int i = 0; i < numinlist - 1; i++)
		for (int j = i + 1; j < numinlist; j++)
			if (oplist[i].freq > oplist[j].freq) {
					temp = oplist[i];
					oplist[i] = oplist[j];
					oplist[j] = temp;
			}
}

void clearList() {
	if (!numinlist) return;
	for (int i = 0; i < LISTSIZE; i++) {
		oplist[i].freq = 0;
		oplist[i].imode = USB;
		oplist[i].iBW = 7;
	}
	FreqSelect->clear();
	numinlist = 0;
}

int coltabs[] = {10, 3, 0};
void updateSelect() {
	coltabs[0] = 0.45 * FreqSelect->w();
	coltabs[1] = 0.1 * FreqSelect->w();
	FreqSelect->column_widths((const int *)coltabs);
	char szFREQMODE[20];
	if (!numinlist) return;
	sortList();
	FreqSelect->clear();
	for (int n = 0; n < numinlist; n++) {
		snprintf(szFREQMODE, sizeof(szFREQMODE),
			"%.2f\t%c\t%-s", 
			oplist[n].freq / 1000.0, 
			szmodes[oplist[n].imode][0], 
			szBW[oplist[n].iBW]);
		FreqSelect->add (szFREQMODE);
	}
}

void addtoList(long val, int mode, int bw) {
	oplist[numinlist].freq = val;
	oplist[numinlist].imode = mode;
	oplist[numinlist++].iBW = bw;
}

void readFile() {
	ifstream iList(defFileName);
	if (!iList) {
		fl_message ("Could not open %s", defFileName);
		return;
	}
	clearList();
	int i = 0, mode = 0, bw = 0;
	long freq;
	while (!iList.eof()) {
		freq = 0L; mode = -1;
		iList >> freq >> mode >> bw;
	    if (freq && (mode > -1)) {
			oplist[i].freq = freq;
			oplist[i].imode = mode;
			oplist[i].iBW = bw;
			i++;
		}
	}
	iList.close();
	numinlist = i;
	updateSelect();
}

void buildlist() {
	
	strcpy (defFileName, homedir.c_str());
	strcat (defFileName, "default.arv");
	FILE *fh = fopen(defFileName, "r");
	if (fh != NULL) {
		fclose (fh);
		readFile();
		return;
	}
	
	for (int n = 0; n < LISTSIZE; n++) {oplist[n].freq = 0;}
	addtoList (10130000L, USB, 7);
	addtoList (3580000L, USB, 7);
	addtoList (14070000L, USB, 7);
	addtoList (21000000L, USB, 7);
	addtoList (7255000L, LSB, 7);
	addtoList (7070000L, USB, 7);
	addtoList (14000000L, CW, 2);
	addtoList (28000000L, USB, 7);
	addtoList (7000000L, CW, 2);
	addtoList (3500000L, CW, 2);
	addtoList (10100000L, CW, 2);
	addtoList (7030000L, USB, 7);
	updateSelect();
}

int movFreq()
{
	Fl_Color bgclr = fl_rgb_color(xcvrState.bg_red, xcvrState.bg_green, xcvrState.bg_blue);
	Fl_Color fgclr = fl_rgb_color(xcvrState.fg_red, xcvrState.fg_green, xcvrState.fg_blue);
	vfoA.freq = FreqDisp->value();
	if (rx_on_a) {
		setXcvrRcvFreq(vfoA.freq);
		send_xml_freq(vfoA.freq);
		FreqDisp->SetONOFFCOLOR( fgclr, bgclr);
		FreqDispB->SetONOFFCOLOR( fgclr, fl_color_average(bgclr, FL_BLACK, 0.87));
	}
	if (tx_on_a) {
		setXcvrXmtFreq(vfoA.freq);
	}
	setXcvrSplit();
	return 0;
}

int movFreqB()
{
	Fl_Color bgclr = fl_rgb_color(xcvrState.bg_red, xcvrState.bg_green, xcvrState.bg_blue);
	Fl_Color fgclr = fl_rgb_color(xcvrState.fg_red, xcvrState.fg_green, xcvrState.fg_blue);
	vfoB.freq = FreqDispB->value();
	if (!rx_on_a) {
		setXcvrRcvFreq(vfoB.freq);
		send_xml_freq(vfoB.freq);
		FreqDispB->SetONOFFCOLOR( fgclr, bgclr);
		FreqDisp->SetONOFFCOLOR( fgclr, fl_color_average(bgclr, FL_BLACK, 0.87));
	}
	if (!tx_on_a) {
		setXcvrXmtFreq(vfoB.freq);
	}
	setXcvrSplit();
	return 0;
}

void cbRxA()
{
	rx_on_a = true;
	if (tx_on_a) {
		opMODE->value(vfoA.imode);
		opBW->value(vfoA.iBW);
		setXcvrMode(vfoA.imode);
		setXcvrBW(vfoA.iBW);
		send_new_mode(vfoA.imode);
		send_new_bandwidth(vfoA.iBW);
	} else {
		opMODE->value(vfoB.imode);
		opBW->value(vfoB.iBW);
		setXcvrMode(vfoB.imode);
		setXcvrBW(vfoB.iBW);
		send_new_mode(vfoB.imode);
		send_new_bandwidth(vfoB.iBW);
	}
	movFreq();
	movFreqB();
}

void cbTxA()
{
	tx_on_a = true;
	opMODE->value(vfoA.imode);
	opBW->value(vfoA.iBW);
	setXcvrMode(vfoA.imode);
	setXcvrBW(vfoA.iBW);
	send_new_mode(vfoA.imode);
	send_new_bandwidth(vfoA.iBW);
	movFreq();
	movFreqB();
}

void cbRxB()
{
	rx_on_a = false;
	if (!tx_on_a) {
		opMODE->value(vfoB.imode);
		opBW->value(vfoB.iBW);
		setXcvrMode(vfoB.imode);
		setXcvrBW(vfoB.iBW);
		send_new_mode(vfoB.imode);
		send_new_bandwidth(vfoB.iBW);
	} else {
		opMODE->value(vfoA.imode);
		opBW->value(vfoA.iBW);
		setXcvrMode(vfoA.imode);
		setXcvrBW(vfoA.iBW);
		send_new_mode(vfoA.imode);
		send_new_bandwidth(vfoA.iBW);
	}
	movFreq();
	movFreqB();
}

void cbTxB()
{
	tx_on_a = false;
	opMODE->value(vfoB.imode);
	opBW->value(vfoB.iBW);
	setXcvrMode(vfoB.imode);
	setXcvrBW(vfoB.iBW);
	send_new_mode(vfoB.imode);
	send_new_bandwidth(vfoB.iBW);
	movFreq();
	movFreqB();
}

void cbRxA_TxA()
{
	cbRxA();
	cbTxA();
	btnRxA_TxA->color((Fl_Color)215);
	btnRxA_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxA->redraw();
	btnRxA_TxB->redraw();
	btnRxB_TxA->redraw();
	btnRxB_TxB->redraw();
}

void cbRxA_TxB()
{
	cbRxA();
	cbTxB();
	btnRxA_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxB->color((Fl_Color)215);
	btnRxB_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxA->redraw();
	btnRxA_TxB->redraw();
	btnRxB_TxA->redraw();
	btnRxB_TxB->redraw();
}

void cbRxB_TxA()
{
	cbRxB();
	cbTxA();
	btnRxA_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxA->color((Fl_Color)215);
	btnRxB_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxA->redraw();
	btnRxA_TxB->redraw();
	btnRxB_TxA->redraw();
	btnRxB_TxB->redraw();
}

void cbRxB_TxB()
{
	cbRxB();
	cbTxB();
	btnRxA_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxA_TxB->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxA->color((Fl_Color)FL_BACKGROUND_COLOR);
	btnRxB_TxB->color((Fl_Color)215);
	btnRxA_TxA->redraw();
	btnRxA_TxB->redraw();
	btnRxB_TxA->redraw();
	btnRxB_TxB->redraw();
}

void cbA2B()
{
	vfoB.freq = vfoA.freq;
	vfoB.imode = vfoA.imode;
	vfoB.iBW = vfoA.iBW;
	FreqDispB->value(vfoB.freq);
	movFreq();
	movFreqB();
}

void cbB2A()
{
	vfoA.freq = vfoB.freq;
	vfoA.imode = vfoB.imode;
	vfoA.iBW = vfoB.iBW;
	FreqDisp->value(vfoA.freq);
	movFreq();
	movFreqB();
}

void selectFreq() {
	int n = FreqSelect->value();
	if (!n) return;
	n--;
	if (rx_on_a) {
		vfoA = oplist[n];
		FreqDisp->value(vfoA.freq);
		opMODE->value(vfoA.imode);
		opBW->value(vfoA.iBW);
		cbRxA_TxA();
	} else {
		vfoB = oplist[n];
		FreqDispB->value(vfoB.freq);
		opMODE->value(vfoB.imode);
		opBW->value(vfoB.iBW);
		cbRxB_TxB();
	}
}

void delFreq() {
	if (FreqSelect->value()) {
		int n = FreqSelect->value() - 1;
		for (int i = n; i < numinlist; i ++)
			oplist[i] = oplist[i+1];
		oplist[numinlist - 1].imode = USB;
		oplist[numinlist - 1].freq = 0;
		numinlist--;
		updateSelect();
	}
}

void addFreq() {
FREQMODE vfo;
	if (rx_on_a) vfo = vfoA;
	else         vfo = vfoB;
	for (int n = 0; n < numinlist; n++) 
		if (vfo.freq == oplist[n].freq && 
			vfo.imode == oplist[n].imode &&
			vfo.iBW == oplist[n].iBW ) return;
	addtoList(vfo.freq, vfo.imode, vfo.iBW);
	updateSelect();
}

void cbAttenuator()
{
	if (btnAttenuator->value() == 1) {
		setXcvrAttControl(1);
		btnPreamp->value(0);
		setXcvrPreamp(0);
	} else
		setXcvrAttControl(0);
}

void cbPreamp()
{
	if (btnPreamp->value() == 1) {
		setXcvrPreamp(1);
		btnAttenuator->value(0);
		setXcvrAttControl(0);
	} else
		setXcvrPreamp(0);
}

void cbbtnNR()
{
	if (btnNR->value() == 0) {
		sldrNR->deactivate();
		setXcvrNR(false);
		cbsldrAgcSpeed();
	}
	else {
		sldrNR->activate();
		setXcvrNR(true);
		setXcvrNRlevel(sldrNR->value());
		cbsldrAgcSpeed();
	}
}

void cbNR()
{
	setXcvrNRlevel(sldrNR->value());
}


void cbRIT()
{
	setXcvrRITfreq(sldrRIT->value());
}

void cbbtnRIT()
{
	if (btnRIT->value() == 0) {
		setXcvrRITfreq(0.0);
		sldrRIT->value(0.0);
		sldrRIT->deactivate();
		xcvrState.RIT = 1;
	} else {
		sldrRIT->activate();
		xcvrState.RIT = 0;
	}
}

void cbbtnNotch()
{
	if (btnNotch->value() == 1) { // Auto notch
		sldrNOTCH->hide(); // NOTCH freq adjust hidden
		sldrDepth->show(); // AUTO depth shown
		sldrNR->deactivate(); // NR controls deactivated
		btnNR->deactivate();
		setXcvrNotchWidth(3); // Put into AUTO notch mode
		setXcvrNotchDepth(sldrDepth->value()); // With last position of slider control
	} else { // Manual notch
		sldrDepth->hide(); // Depth control hidden
		sldrNOTCH->show(); // NOTCH freq adjust shown
		sldrNR->activate(); // NR controls activated
		btnNR->activate();
		setXcvrNotchWidth(opNOTCH->value()); // Notch width restored
		setXcvrNotch(sldrNOTCH->value()); // Notch frequency restored
		cbbtnNR();  // NR level restored as need
	}
}

void cbDepth()
{
	setXcvrNotchDepth(sldrDepth->value());
}

void setNotch()
{
	setXcvrNotch(sldrNOTCH->value());
}

void setNotchWidth()
{
	setXcvrNotchWidth(opNOTCH->value());
}

void setIFshift()
{
	setXcvrIFshift(sldrIFSHIFT->value());
}

void cbIFsh()
{
	if (btnIFsh->value() == 0)
		sldrIFSHIFT->value(0);
	setIFshift();
}

void setVolume()
{
	if (btnVol->value() == 1)
		setXcvrVolume(sldrVOLUME->value());
}

void cbVol()
{
	if (btnVol->value() == 0) {
		sldrVOLUME->deactivate();
		setXcvrVolume(0.0);
	} else {
		sldrVOLUME->activate();
		setXcvrVolume(sldrVOLUME->value());
	}
}

void setMicGain()
{
	setXcvrMicGain(sldrMICGAIN->value());
}

void setPowerImage(double pwr)
{
	if (pwr < 15.0) {
		btnPower->image(image_p15);
		sldrFwdPwr->minimum(15.0f);
	}
	else if (pwr < 30.0) {
		btnPower->image(image_p30);
		sldrFwdPwr->minimum(30.0f);
	}
	else if (pwr < 45.0) {
		btnPower->image(image_p45);
		sldrFwdPwr->minimum(45.0f);
	}
	else if (pwr < 60.0) {
		btnPower->image(image_p60);
		sldrFwdPwr->minimum(60.0f);
	}
	else if (pwr < 75.0) {
		btnPower->image(image_p75);
		sldrFwdPwr->minimum(75.0f);
	}
	else if (pwr < 90.0) {
		btnPower->image(image_p90);
		sldrFwdPwr->minimum(90.0f);
	}
	else {
		btnPower->image(image_p120);
		sldrFwdPwr->minimum(120.0f);
	}
	btnPower->redraw();
	return;
}

void setPower()
{
	double pwr = sldrPOWER->value();
	Fl_Image *img = btnPower->image();

	setXcvrPower(pwr);
	if ((img != &image_alc))
		setPowerImage(pwr);
}

void cb_autotune(int v)
{
	if (v) setXcvrTune(1);
	else setXcvrTune(0);
}

void cbTune()
{
	setXcvrTune(2);
}

void setTx(bool on)
{
	if (on) {
		FreqDisp->deactivate();
		FreqDispB->deactivate();
		btn_tune->deactivate();
		btnCarrier->deactivate();
		setXcvrPTT(1);
	} else {
		setXcvrPTT(0);
		FreqDisp->activate();
		FreqDispB->activate();
		btn_tune->activate();
		btnCarrier->activate();
	}
}

void cbPTT()
{
	if (btnPTT->value() == 1) {
		setTx(true);
	} else {
		setTx(false);
	}
}

void cbCarrier()
{
	if (btnCarrier->value() == 1) {
		btnPTT->deactivate();
		btn_tune->deactivate();
		setXcvrCarrier(1); // carrier on
		setXcvrPTT(1);
	} else {
		btnPTT->activate();
		btn_tune->activate();
		setXcvrCarrier(0); // carrier OFF
		setXcvrPTT(0);
	}
}

void cbWPM()
{
	setXcvrWPM(cntrWPM->value());
	cntrFARNSWORTH->maximum(cntrWPM->value());
	if (cntrFARNSWORTH->value() > cntrWPM->value()) {
		cntrFARNSWORTH->value(cntrWPM->value());
		xcvrState.FARNSWORTH_WPM = cntrWPM->value();
	}
}

void cbSPOT()
{
	setXcvrSPOT(btnSPOT->value());
}

int iAntSel = 0;
void cbAntSel()
{
	iAntSel = antSelect->value();
	movFreq();
}

void updateSquelch( int data)
{
	if (data == 128 && xcvrState.SQLEVEL > -127)
		boxSquelch->color(fl_rgb_color(xcvrState.smeterRed, xcvrState.smeterGreen, xcvrState.smeterBlue));
	else
		boxSquelch->color(fl_rgb_color(xcvrState.bg_red, xcvrState.bg_green, xcvrState.bg_blue));
	boxSquelch->redraw();
	LOG_DEBUG("%d", data);
}

void updateALC(int data)
{
	Fl_Image *img = btnPower->image();
	if (img == &image_alc) {
		sldrFwdPwr->value(data * 2.0);//1.0);
		sldrFwdPwr->redraw();
	}
	sldrFwdPwr->redraw();
	LOG_DEBUG("%d", data);
}

float fp_ = 0.0, rp_ = 0.0;

void updateFwdPwr(int data)
{
	fp_ = 2 * data;
	float power = xcvrState.MAXPWR * fp_ / 100.0;
	Fl_Image *img = btnPower->image();
	if (img != &image_alc) {
		sldrFwdPwr->value(power);
		sldrFwdPwr->redraw();
	}
	send_pwrmeter_val((int)(power));
}

void updateSWR(int data)
{
	float rho = 0;
	float vswr = 1.0;
	rp_ = 2 * data;
	rho = 1.0;

	if (rp_ == fp_ && fp_ > 0) vswr = 5.0;
	else if (fp_ > 0) {
		rho = sqrtf(rp_ / fp_);
		vswr = (1 + rho) / (1 - rho);
		if (vswr > 5) vswr = 5;
	}
	sldrRefPwr->value(50.0 * (vswr - 1.0) / 4.0);
	sldrRefPwr->redraw();
	if (fp_ > 0)
		LOG_INFO("fwd %3.1f, ref %3.1f, VSWR %3.1f", 
				 xcvrState.MAXPWR * fp_ / 100.0, 
				 xcvrState.MAXPWR * rp_ / 100.0, 
				 vswr);
}

void zeroSmeter()
{
	sldrRcvSignal->clear(-127);
	sldrRcvSignal->redraw();
}

void zeroXmtMeters()
{
	updateFwdPwr(0);
	updateALC(0);
	updateSWR(0);
}

int	avgrcvsig = 0;
int avgcnt = 0;
bool computeavg = 0;

void updateRcvSignal( int data)
{
	if (computeavg) {
		avgrcvsig += data;
		avgcnt++;
	}
	if (computeavg && (avgcnt == 32)) {
		avgrcvsig /= -avgcnt;
		if (avgrcvsig < -110) {
			fl_message("Signal strength %d dB too low!", avgrcvsig);
		} else if (avgrcvsig < -101) {
			if (fl_choice("Weak signal, proceed", "No", "Yes", NULL) == 1) {
				Calibrate();
				fl_message("Calibration completed.");
			}
		} else {
			Calibrate();
			fl_message("Calibration completed.");
		}
		avgcnt = 0;
		computeavg = false;
		avgrcvsig = 0;
	}
	sldrRcvSignal->value(-data);
	update_scanner(-data);
	int ival = round((120 - data) / 0.9);
	if (ival < 0) ival = 0;
	if (ival > 100) ival = 100;
	send_smeter_val(ival);
	LOG_DEBUG("%d", data);
}

void setOverTempAlarm()
{
	txtTEMP->textcolor(FL_RED);
	txtTEMP->redraw();
}

#define NTEMPS 50
double temps[NTEMPS];
int Ntemps = 0;
int ntemp = 0;
bool dispCent = true;
char Centigrade[] = "C";
char Fahrenheit[] = "F";

void updateTempDisplay(int data)
{
	double temp = (data - 220)*2.5 + 17.5;
	temps[ntemp++] = temp;
	ntemp %= NTEMPS;
	if (++Ntemps > NTEMPS) Ntemps = NTEMPS;
	temp = 0.0;
	for (int i = 0; i < Ntemps; i++) temp += temps[i];
	temp /= Ntemps;
	char buff[5];
	if (!dispCent)
		temp = temp * 9 / 5 + 32.0;
	sprintf(buff,"%3.0f",temp);
	txtTEMP->value(buff);
	txtTEMP->redraw();
	LOG_DEBUG("%d", data);
}

void cbTemp()
{
	const char *lbl = txtTEMP->label();
	if (lbl[0] == 'C') {
		dispCent = false;
		txtTEMP->label(Fahrenheit);
	} else {
		dispCent = true;
		txtTEMP->label(Centigrade);
	}
	txtTEMP->redraw();
}

void openFreqList()
{
    char *p = fl_file_chooser("Open freq list", "*.arv", homedir.c_str());
    if (p) {
		strcpy (defFileName, p);
		readFile();
	}
}

void saveFreqList()
{
    if (!numinlist) return;
    char *p = fl_file_chooser("Save freq list", "*.arv", defFileName);
    if (p) {
		ofstream oList(p);
		if (!oList) {
			fl_message ("Could not write to %s", p);
			return;
		}
		for (int i = 0; i < numinlist; i++)
			oList << oplist[i].freq << " " << oplist[i].imode << " " << oplist[i].iBW << endl;
		oList.close();
		strcpy(defFileName, p);
	}
}

void loadState()
{
	xcvrState.loadLastState();
	tabs->hide();
	btn_show_controls->label("@-22->");
	btn_show_controls->redraw_label();
	window->resize(xcvrState.mainX, xcvrState.mainY, window->w(), window->h() - tabs->h());
}

void saveState()
{
	xcvrState.saveLastState();
}

void cbSmeter()
{
	Fl_Image *img = btnSmeter->image();
	if (img == &image_dbm)
		btnSmeter->image(image_mvolts);
	else if (img == &image_mvolts)
		btnSmeter->image(image_smeter);
	else
		btnSmeter->image(image_dbm);
	btnSmeter->redraw();
	setFocus();
}

void cbPWR()
{
	Fl_Image *img = btnPower->image();
	if (img == &image_alc) {
		setPowerImage(sldrPOWER->value());
	} else {
		btnPower->image(image_alc);
	}
	btnPower->redraw();
	setFocus();
}

void setPTT(void *data)
{
	bool on = (bool)(reinterpret_cast<long>(data));
	btnPTT->value(on);
	cbPTT();
}

void set_xml_values( void * d)
{
	vfoA = xmlvfo;
	FreqDisp->value(vfoA.freq);
	opMODE->value(vfoA.imode);
	opBW->value(vfoA.iBW);
	setXcvrRcvFreq (vfoA.freq);
	setXcvrXmtFreq (vfoA.freq);
	setXcvrMode (vfoA.imode);
	setXcvrBW (vfoA.iBW);
}

long RigSerNbr = 0;
int  RigFirm[2] = {0,0};
int  RigHard[2] = {0,0};
void GetKachinaVersion()
{
	unsigned char buffer[10];
	if (testing) {
		RigSerNbr = 0L;
		RigFirm[0] = 1;
		RigHard[0] = 'A';
		RigHard[1] = 1;
	} else {
		bool dataok = RequestData (cmdK_RSER, buffer, 10);
		if (!dataok) {
			const char errmsg[] = "KC505 not responding!";
			fl_message("%s", errmsg);
			testing = true;
			kcatSerial.ClosePort();
			return;
		}
		RigSerNbr = (((buffer[0]*256 + buffer[1])*256) + buffer[2])*256 + buffer[3];
		RigFirm[0] = buffer[6]; RigFirm[1] = buffer[7];
		RigHard[0] = buffer[4]; RigHard[1] = buffer[5];
	}
	LOG_WARN("Serial number %ld\nFirmware version %d.%d\nHardware version %c.%d", 
		RigSerNbr, 
		RigFirm[0], RigFirm[1],
		RigHard[0], RigHard[1]);
}

// cmdK_RTIME does not seem to respond with any data
/*
long time_on = 0;
void GetKachinaHours()
{
	unsigned char tbuffer[6];
	memset(tbuffer, 0, 6);
	RequestData (cmdK_RTIME, tbuffer, 6);
	time_on = (((tbuffer[0]*256 + tbuffer[1])*256) + tbuffer[2])*256 + tbuffer[3];
}
*/

void about()
{
	fl_message( "\
CAT for Kachina 505, copyright W1HKJ\n\
Version %s\n\
Serial #: %ld\n\
Firmware: %d.%d\n\
Hardware: %c.%d\n\n\
w1hkj@@w1hkj.com", 
VERSION, RigSerNbr, 
RigFirm[0], RigFirm[1],
RigHard[0], RigHard[1]);
}


// TEST results logger
// Utilities

void cbClearAntData()
{
	sendCmd(cmdK_ANTA0);
	sendCmd(cmdK_ANTB0);
}

//======================================================================
// Radio sends telemetry data at a 50 msec rate
//======================================================================

bool exit_telemetry = false;

void parseTelemetry(void *)
{
	unsigned char data;
	static bool smeter_active = true;
	static bool pwrmtr_active = false;

	while(commstack.pop(data)) {

//std::cout << (unsigned int)data << "\n";
		if (data < 130) { // receive telemetry
			if (data < 128) { // smeter
				updateRcvSignal(data);
				smeter_active = true;
				if (pwrmtr_active) {
					pwrmtr_active = false;
					zeroXmtMeters();
				}
			}
			else if (data == 128 || data == 129) // squelch
				updateSquelch(data);
		} else if (data < 215) { // transmit telemetry
			if (data < 140) // ALC
				updateALC(data - 130);
			else if (data < 190) { // forward power
				updateFwdPwr(data - 140);
			} else if (data < 215)
				updateSWR(data - 190);
		} else if (data == 215) // temperature alarm
			setOverTempAlarm();
		else if (data > 219 && data < 250) // temperature value
			updateTempDisplay(data);

		if (data > 129 && data < 215) {
			if (smeter_active) {
				zeroSmeter();
				smeter_active = false;
				pwrmtr_active = true;
			}
		}
	}
}

void * telemetry_thread_loop(void *d)
{
	char buff[20];
	int num = 0;
	for (;;) {
		MilliSleep(10);
		if (exit_telemetry) break;
		{
			guard_lock serial_lock(&mutex_serial);
			num = kcatSerial.ReadBuffer (buff, 1);
		}
		if (num) commstack.push((unsigned char)buff[0]);
		if (!commstack.isEmpty()) Fl::awake(parseTelemetry);
	}
	return NULL;
}

//======================================================================
// watchdog timer sends a NOOP to xcvr every 15 seconds
//======================================================================
bool exit_watchdog = false;
int watchdog_count = 150;

void * watchdog_thread_loop(void *d)
{
	for (;;) {
		if (exit_watchdog) break;
		MilliSleep(100);
		if (--watchdog_count == 0) {
			{
				guard_lock dog_log(&mutex_watchdog);
				setXcvrNOOP();
			}
			watchdog_count = 150;
		}
	}
	return NULL;
}

//======================================================================
// cw thread monitors for cw keyboard activity
// loop timing is dependent on cw WPM setting
//======================================================================
bool exit_cw = false;
extern double char_duration;

void delete_char(void *)
{
	if (strlen(txt_to_send->value())) {
		string txt = txt_to_send->value();
		txt.erase(0,1);
		txt_to_send->value(txt.c_str());
	}
}

void * cw_thread_loop(void *d)
{
	int duration = 20;
	for (;;) {
		if (exit_cw) break;
		// wait for end of char
		while (duration > 0) {
			MilliSleep(1);
			duration--;
		}
		if (dlgCWkeyboard && dlgCWkeyboard->visible()) {
			if (btn_send->value() && strlen(txt_to_send->value())) {
				guard_lock cw_lock(&mutex_cw);
				sendChar(txt_to_send->value()[0]);
				Fl::awake(delete_char);
				duration = char_duration;
			} else
				duration = 20;
		}
	}
	return NULL;
}

//======================================================================

void startProcessing(void *d)
{
	watchdog_thread = new pthread_t;
	if (pthread_create(watchdog_thread, NULL, watchdog_thread_loop, NULL)) {
		perror("pthread_create watchdog");
		exit(EXIT_FAILURE);
	}

	telemetry_thread = new pthread_t;
	if (pthread_create(telemetry_thread, NULL, telemetry_thread_loop, NULL)) {
		perror("pthread_create telemetry");
		exit(EXIT_FAILURE);
	}

	xmlrpc_thread = new pthread_t;
	if (pthread_create(xmlrpc_thread, NULL, xmlrpc_thread_loop, NULL)) {
		perror("pthread_create telemetry");
		exit(EXIT_FAILURE);
	}

	cw_thread = new pthread_t;
	if (pthread_create(cw_thread, NULL, cw_thread_loop, NULL)) {
		perror("pthread create cw thread");
		exit(EXIT_FAILURE);
	}

	if (!testing) {
		if (startComms(xcvrState.ttyport.c_str(), 9600) == 0) {
			fl_message("%s not available", xcvrState.ttyport.c_str());
			testing = true;
		}
	} else
		fl_message("\
Executing in test mode\n\
Select serial port 'Config / Select port'");

	initkcat();
	setInhibits();
}

void cbExit()
{
// close watchdog
	pthread_mutex_lock(&mutex_watchdog);
	exit_watchdog = true;
	pthread_mutex_unlock(&mutex_watchdog);
	pthread_join(*watchdog_thread, NULL);

// close telemetry
	pthread_mutex_lock(&mutex_telemetry);
	exit_telemetry = true;
	pthread_mutex_unlock(&mutex_telemetry);
	pthread_join(*telemetry_thread, NULL);

// close cw
	pthread_mutex_lock(&mutex_cw);
	exit_cw = true;
	pthread_mutex_unlock(&mutex_cw);
	pthread_join(*cw_thread, NULL);

// close xmlrpc
	pthread_mutex_lock(&mutex_xmlrpc);
	exit_xmlrpc = true;
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_join(*telemetry_thread, NULL);

	send_no_rig();
	close_rig_xmlrpc();

	if (!testing)
		kcatSerial.ClosePort();

	saveState();

	exit(0);
}

