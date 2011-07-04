#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "support.h"
#include "Kachina.h"
#include "Kachina_io.h"
#include "util.h"
#include "config.h"
#include "xml_io.h"

using namespace std;

extern bool test;

extern void resetWatchDog();

const char *szmodes[] = {"LSB", "USB", "CW", "AM", "FM", NULL};
const char modetype[] = {'L', 'U', 'L', 'U', 'U'};

const char *szBW[] =  {
"100", "200", "500", "1000", "1700", "2100", "2400", "2700", "3500","DATmed", "DAThi", NULL};
int iBW[] = {
0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x0b, 0x0a };

const char *szNotch[] = {"Wide", "Med", "Nar", NULL};

FREQMODE vfoA = {1407000, USB, 7, UI };
FREQMODE vfoB = {3500000, USB, 7, UI };
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

CSerialComm KachinaSerial;
#ifdef WIN32
	int baudttyport = CBR_9600;
#else
	int baudttyport = B9600;
#endif

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

void initKachina()
{
	dlgRcvConfig = RcvParamDialog();
	dlgXmtConfig = XmtParamDialog();
	dlgCwParams  = CwParamDialog();
	dlgAntPorts  = FreqRangesDialog();
    FreqDisp->setMaxVal(30000000L);
    FreqDisp->setMinVal(30000L);

	if (test) {
		mnuViewLog->show();
		openLogViewer();
		OpenTestLog();
	} else
		mnuViewLog->hide();
	GetKachinaVersion();
	initXcvrState();
	cbA2B();
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
	opNOTCH->value(0);
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

static bool nofocus = false;

void setFocus()
{
	if (dlgAntPorts != NULL && dlgAntPorts->visible())
		return;
	if (nofocus) return;
	if (Fl::focus() != FreqDisp)
		Fl::focus(FreqDisp);
}

void setBW() {
	vfoA.iBW = opBW->value();
	setXcvrBW (vfoA.iBW);
	send_new_bandwidth(vfoA.iBW);
	setIFshift();
}

MODES current_mode = AM;

void setMode() 
{
	MODES new_mode = (MODES)opMODE->value();
	setXcvrMode (new_mode);
	if (vfoA.imode != new_mode) {
		switch (opMODE->value()) {
			case AM: opBW->value(8); setBW(); break;
			case FM: opBW->value(8); setBW(); break;
			case CW: opBW->value(2); setBW(); break;
			default: opBW->value(7); setBW(); break;
		}
		setInhibits();
		vfoA.imode = new_mode;
		send_new_mode(vfoA.imode);
	} else setBW();
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

void updateSelect() {
	char szFREQMODE[20];
	if (!numinlist) return;
	sortList();
	FreqSelect->clear();
	for (int n = 0; n < numinlist; n++) {
		sprintf(szFREQMODE, "%9.3f%4s", oplist[n].freq / 1000.0, szmodes[oplist[n].imode]);
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
	
	strcpy (defFileName, homedir);
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

int movFreq() {
	int ret;
	vfoA.freq = FreqDisp->value();
	setXcvrXmtFreq (vfoA.freq, xcvrState.TxOffset);
	send_xml_freq(vfoA.freq);
	ret = setXcvrRcvFreq (FreqDisp->value(), 0);
	return ret;
}

	
void cbABsplit()
{
	if (btnSplit->value() == 1) {
		setXcvrSplit();
		setXcvrXmtFreq (vfoB.freq, xcvrState.TxOffset);
		setXcvrListenOnReceive();
		setXcvrRcvFreq (vfoA.freq, 0);
	} else {
//		setXcvrSimplex();
		setXcvrSplit();
		setXcvrXmtFreq (vfoA.freq, xcvrState.TxOffset);
		setXcvrListenOnReceive();
		setXcvrRcvFreq (vfoA.freq, 0);
	}
}

void cbABactive()
{
	if (!vfoB.freq) return;
	FREQMODE temp = vfoA;
	vfoA = vfoB;
	vfoB = temp;
	FreqDisp->value(vfoA.freq);
	opMODE->value(vfoA.imode);
	opBW->value(vfoA.iBW);
	sprintf(szVfoB,"%8ld", vfoB.freq);
	txtInactive->value(szVfoB);
	btnSplit->value(0);
	cbABsplit();
	send_xml_freq(vfoA.freq);
	send_new_mode(vfoA.imode);
	send_new_bandwidth(vfoA.iBW);
}

void cbA2B()
{
	vfoB.freq = vfoA.freq;
	vfoB.imode = vfoA.imode;
	vfoB.iBW = vfoA.iBW;
	sprintf(szVfoB,"%8ld", vfoB.freq);
	txtInactive->value(szVfoB);
	btnSplit->value(0);
	cbABsplit();
}


void selectFreq() {
	if (FreqDisp->enable() == false) return;
	int n = FreqSelect->value();
	if (!n) return;
	n--;
	vfoA = oplist[n];
	FreqDisp->value(vfoA.freq);
	opMODE->value(vfoA.imode);
	opBW->value(vfoA.iBW);
	setXcvrRcvFreq (vfoA.freq, 0);
	setXcvrXmtFreq (vfoA.freq,  0);
	setXcvrMode(vfoA.imode);
	setXcvrBW(vfoA.iBW);
	send_xml_freq(vfoA.freq);
	send_new_mode(vfoA.imode);
	send_new_bandwidth(vfoA.iBW);
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
	for (int n = 0; n < numinlist; n++) 
		if (vfoA.freq == oplist[n].freq && vfoA.imode == oplist[n].imode) return;
	addtoList(vfoA.freq, vfoA.imode, vfoA.iBW);
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
	if (btnNotch->value() == 1) {
		sldrNOTCH->hide(); // NOTCH freq adjust hidden
		sldrDepth->show(); // AUTO depth shown
		sldrNR->deactivate(); // NR controls deactivated
		btnNR->deactivate();
		setXcvrNotchWidth(3); // Put into AUTO notch mode
		setXcvrNotchDepth(sldrDepth->value()); // With last postion of slider control
	} else {
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
	if (btnIFsh->value() == 1) {
		sldrIFSHIFT->activate();
		sldrIFSHIFT->value(0);
	} else {
		sldrIFSHIFT->value(0);
		sldrIFSHIFT->deactivate();
	}
	setIFshift();
}

void setVolume()
{
	setXcvrVolume(sldrVOLUME->value());
}

void cbMute()
{
	if (btnMute->value() == 1) {
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
	if (img != &image_alc)
		setPowerImage(pwr);
}

void cbTune()
{
	setXcvrTune(2);
}

void setTx(bool on)
{
	if (on) {
		if (btnSplit->value()) {
			setXcvrXmtFreq(vfoB.freq, xcvrState.TxOffset);
			setXcvrRcvFreq (FreqDisp->value(), 0);
		} else {
			setXcvrXmtFreq(FreqDisp->value(), xcvrState.TxOffset);
			setXcvrRcvFreq(FreqDisp->value(), 0);
		}
		btnTune->deactivate();
		btnCarrier->deactivate();
		setXcvrPTT(1);
	} else {
		setXcvrPTT(0);
		if (btnSplit->value()) {
			setXcvrXmtFreq(vfoB.freq, 0);
			setXcvrRcvFreq (FreqDisp->value(), 0);
		} else {
			setXcvrRcvFreq(FreqDisp->value(), 0);
		}
		btnTune->activate();
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

void adjustFreqs()
{
	if (btnSplit->value()) {
		setXcvrXmtFreq(vfoB.freq, xcvrState.TxOffset);
		setXcvrRcvFreq (FreqDisp->value(), 0);
	} else {
		setXcvrXmtFreq(FreqDisp->value(), xcvrState.TxOffset);
		setXcvrRcvFreq(FreqDisp->value(), 0);
	}
}

void cbCarrier()
{
	if (btnCarrier->value() == 1) {
		btnPTT->deactivate();
		btnTune->deactivate();
		setXcvrCarrier(1); // carrier on
		cmdK_cmdR[2] = 0; // Equalizer set to 0
		sendCommand(cmdK_cmdR);
		cmdK_SPP0[2] = 0; // Speech processor OFF
		sendCommand(cmdK_SPP0);
		setXcvrPTT(1);
		
	} else {
		btnPTT->activate();
		btnTune->activate();
		setXcvrPTT(0);
		setXcvrCarrier(0); // carrier OFF
		cmdK_cmdR[2] = xcvrState.EQUALIZER;
		sendCommand(cmdK_cmdR);
		cmdK_SPP0[2] = xcvrState.SPEECHPROC;
		sendCommand(cmdK_SPP0);
		setMode();
	}
}

void cbWPM()
{
	setXcvrWPM(cntrWPM->value());
}

bool bRxAnt = false;
bool bTxAnt = false;

void cbRxAnt()
{
	if (bRxAnt == true) {
		bRxAnt = false;
		btnRxAnt->label("Rx-A");
	} else {
		bRxAnt = true;
		btnRxAnt->label("Rx-B");
	}
	btnRxAnt->redraw_label();
	if (btnSelAnt->value() == true)
		movFreq();
}

void cbTxAnt()
{
	if (bTxAnt == true) {
		bTxAnt = false;
		btnTxAnt->label("Tx-A");
	} else {
		bTxAnt = true;
		btnTxAnt->label("Tx-B");
	}
	btnTxAnt->redraw_label();
	if (btnSelAnt->value() == true)
		movFreq();
}

void updateSquelch( int data)
{
	if (data == 128)
		boxSquelch->color(FL_GREEN);
	else
		boxSquelch->color(FL_LIGHT1);
	boxSquelch->redraw();
}

void updateALC(int data)
{
	Fl_Image *img = btnPower->image();
	if (img == &image_alc) {
		sldrFwdPwr->value(data * 1.0);
	}
	sldrFwdPwr->redraw();
}

float fp_ = 0.0, rp_ = 0.0;

void updateFwdPwr(int data)
{
	fp_ = 2 * data;
	double power = xcvrState.MAXPWR * fp_ / 99.0;
	Fl_Image *img = btnPower->image();
	if (img != &image_alc) {
		sldrFwdPwr->value(power);
		sldrFwdPwr->redraw();
	}
}

void updateRefPwr(int data)
{
	rp_ = 2 * data;
	sldrRefPwr->value(rp_); // 0 - 50 scale;
	sldrRefPwr->redraw();
}

void zeroSmeter()
{
	sldrRcvSignal->value(-128);
	sldrRcvSignal->redraw();
}

void zeroXmtMeters()
{
	updateFwdPwr(0);
	updateALC(0);
	updateRefPwr(0);
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
	sldrRcvSignal->value(-data);
	zeroXmtMeters();
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
    char *p = fl_file_chooser("Open freq list", "*.arv", homedir);
    if (p) {
		strcpy (defFileName, p);
		readFile();
	}
}

void saveFreqList()
{
    if (!numinlist) return;
	nofocus = true;
    char *p = fl_file_chooser("Save freq list", "*.arv", defFileName);
	nofocus = false;
    if (p) {
		ofstream oList(p);
		if (!oList) {
			fl_message ("Could not write to %s", p);
			return;
		}
		for (int i = 0; i < numinlist; i++)
			oList << oplist[i].freq << " " << oplist[i].imode << endl;
		oList.close();
		strcpy(defFileName, p);
	}
}

void loadConfig()
{
	char fname[200];
	int red, green, blue, baud;
	int clr1, clr2, clr3;
	int freq, rcv, xmt;
	strcpy(fname, homedir);
	strcat(fname, "kachina.ini");
	
	ifstream inCfg(fname);
	if (!inCfg)
		setCommsPort();
	else {
		inCfg >> szttyport >> baud;
		inCfg >> red >> green >> blue;
		FreqDisp->SetONCOLOR (red, green, blue);
		inCfg >> red >> green >> blue;
		FreqDisp->SetOFFCOLOR (red, green, blue);
		inCfg >> red >> green >> blue;
		FreqDisp->SetSELCOLOR (red, green, blue);
		if (!inCfg.eof()) {
			inCfg >> clr1 >> clr2 >> clr3; 
			btnSWR->color(clr1); btnSWR->redraw();
			btnPower->color(clr2); btnPower->redraw();
			btnSmeter->color(clr3); btnSmeter->redraw();
		}
		int ports = 0;
		while (!inCfg.eof()) {
			freq = 0;
			inCfg >> freq >> rcv >> xmt;
			if (freq) {
				if (!ports) { ports = 1; numantports = 0;}
				antports[numantports].freq = freq;
				antports[numantports].rcv = rcv;
				antports[numantports].xmt = xmt;
				numantports++;
			}
		}
		inCfg.close();
	}
}

void saveConfig()
{
	char fname[200];
	uchar red, green, blue;
	int iRed, iGreen, iBlue;
	strcpy(fname, homedir);
	strcat(fname, "kachina.ini");

	ofstream outCfg(fname);
	if (!outCfg) return;
// ttyport & baud rate
	outCfg << szttyport << " " << 9600 << endl;
// digit color
	FreqDisp->GetONCOLOR(red,green,blue);
	iRed = red; iGreen = green; iBlue = blue;
	outCfg << iRed << " " << iGreen << " " << iBlue << endl;
// digit background
	FreqDisp->GetOFFCOLOR(red,green,blue);
	iRed = red; iGreen = green; iBlue = blue;
	outCfg << iRed << " " << iGreen << " " << iBlue << endl;
// digit select color
	FreqDisp->GetSELCOLOR(red,green,blue);
	iRed = red; iGreen = green; iBlue = blue;
	outCfg << iRed << " " << iGreen << " " << iBlue << endl;
// save mode
	outCfg << btnSWR->color() << " ";
	outCfg << btnPower->color() << " ";
	outCfg << btnSmeter->color() << endl;
	for (int i = 0; i < numantports; i++) {
		outCfg << antports[i].freq << " ";
		outCfg << antports[i].rcv << " ";
		outCfg << antports[i].xmt << endl;
	}
	outCfg.close();
}


void loadState()
{
	char fname[200];
	XCVRSTATE ondisk;
	strcpy(fname, homedir);
	strcat(fname, "kachina.sta");
	ifstream inCfg(fname);
	if (inCfg) {
		inCfg.read((char *)&ondisk, sizeof(ondisk));
		inCfg.close();
		if (strcmp(ondisk.vers, VERSION) == 0)
			xcvrState = ondisk;
		else
			fl_message("State file not current version.");
		window->resize(xcvrState.mainX, xcvrState.mainY, 500, 315);
	}
}

void saveState()
{
	char fname[200];
	strcpy(fname, homedir);
	strcat(fname, "kachina.sta");

	ofstream outCfg(fname);
	if (!outCfg) return;
	xcvrState.mainX = window->x();
	xcvrState.mainY = window->y();
	outCfg.write((char *)&xcvrState, sizeof(xcvrState));
	outCfg.close();
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
}

void cbSWR()
{
	Fl_Image *img = btnSWR->image();
	if (img == &image_swr)
		btnSWR->image(image_rev);
	else
		btnSWR->image(image_swr);
	btnSWR->redraw();
}

void cbPWR()
{
	Fl_Image *img = btnPower->image();
	if (img != &image_alc) {
		btnPower->image(image_alc);
		sldrFwdPwr->minimum(10.0f);
	}
	else
		setPowerImage(sldrPOWER->value());
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
	setXcvrRcvFreq (vfoA.freq, 0);
	setXcvrXmtFreq (vfoA.freq,  0);
	setXcvrMode (vfoA.imode);
	setXcvrBW (vfoA.iBW);
}

long RigSerNbr = 0;
int  RigFirm[2] = {0,0};
int  RigHard[2] = {0,0};
void GetKachinaVersion()
{
	unsigned char buffer[10];
	RequestData (cmdK_RSER, buffer, 10);
	RigSerNbr = (((buffer[0]*256 + buffer[1])*256) + buffer[2])*256 + buffer[3];
	RigFirm[0] = buffer[6]; RigFirm[1] = buffer[7];
	RigHard[0] = buffer[4]; RigHard[1] = buffer[5];
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
FILE *fTestLog = 0;
const char *fTestLogName = "KachinaTest.log";

void OpenTestLog()
{
	fTestLog = fopen(fTestLogName,"w");
}

void CloseTestLog()
{
	if (fTestLog) fclose(fTestLog);
}

void writeTestLog( char *str)
{
	if (fTestLog)
		fprintf(fTestLog,"%s", str);
	if (dlgViewLog) {
		txtViewLog->insert(str);
		txtViewLog->show_insert_position();
	}
}

// Utilities

void cbClearAntData()
{
	sendCommand(cmdK_ANTA0);
	sendCommand(cmdK_ANTB0);
}

//======================================================================
// Radio sends telemetry data at a 50 msec rate
//======================================================================

bool exit_telemetry = false;

void parseTelemetry(void *val)
{
	int data = (int)(reinterpret_cast<long> (val));
	if (data > 139 && data < 215)
		FreqDisp->enable(false); // transmitting do not allow changes in vfo
	else
		FreqDisp->enable(true);
	
	if (data < 128)
		updateRcvSignal(data);
	else if (data < 130)
		updateSquelch(data);
	else if (data < 140)
		updateALC(data - 130);
	else if (data < 190) {
		sldrRcvSignal->value(-128);
		updateFwdPwr(data - 140);
	}
	else if (data < 215) {
		sldrRcvSignal->value(-128);
		updateRefPwr(data - 190);
	}
	else if (data == 215)
		setOverTempAlarm();
	else if (data > 219 && data < 250)
		updateTempDisplay(data);
	Fl::flush();
	setFocus();
}

void * telemetry_thread_loop(void *d)
{
	unsigned char buff;
	for (;;) {
		MilliSleep(50);
		if (exit_telemetry) break;
		while (commstack.pop(buff))
			Fl::awake(parseTelemetry, (void *) buff);
		if (!serial_busy)
			if (KachinaSerial.ReadBuffer (&buff, 1) == 1)
				commstack.push(buff);
	}
	return NULL;
}

//======================================================================
// watchdog timer sends a NOOP to xcvr every 15 seconds
//======================================================================
bool exit_watchdog = false;

void * watchdog_thread_loop(void *d)
{
	static int count = 1500;
	for (;;) {
		if (exit_watchdog) break;
		MilliSleep(10);
		if (--count == 0) {
			pthread_mutex_lock(&mutex_watchdog);
			setXcvrNOOP();
			pthread_mutex_unlock(&mutex_watchdog);
			count = 1500;
		}
	}
	return NULL;
}

//======================================================================

void startProcessing(void *d)
{
	if (startComms(szttyport, baudttyport) == 0) {
		fl_message("%s not available", szttyport);
		exit(1);
	}

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

	open_rig_xmlrpc();
	xmlrpc_thread = new pthread_t;
	if (pthread_create(xmlrpc_thread, NULL, xmlrpc_thread_loop, NULL)) {
		perror("pthread_create telemetry");
		exit(EXIT_FAILURE);
	}

	initKachina();
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

// close xmlrpc
	pthread_mutex_lock(&mutex_xmlrpc);
	exit_telemetry = true;
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_join(*telemetry_thread, NULL);

	send_no_rig();

	KachinaSerial.ClosePort();
	saveConfig();
	saveState();
	if (test)
		CloseTestLog();
	exit(0);
}

