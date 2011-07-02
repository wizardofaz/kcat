#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "support.h"
#include "config.h"

using namespace std;

extern bool test;

extern void resetWatchDog();

enum MODES {AM, CW, FM, USB, LSB};
const char *szmodes[] = {"AM", "CW", "FM", "USB", "LSB", 0};

const char *szBW[] =  {"3500","2700","2400","2100","1700","1000","500","200","100","DAThi","DATmed", 0};

const char *szNotch[] = {"Wide", "Med", "Nar", 0};

struct FREQMODE {
	long freq;
	union {MODES  mode; int imode;};
} freqmode;

FREQMODE vfoB = {3500000, {CW} };

char szVfoB[12];

struct ST_SHMEM {
	int  flag;
	long freq;
	long midfreq;
	char mode[4];
};

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
	p = szBW;
	while (*p) 
		opBW->add(*p++);
	p = szNotch;
	while (*p)
		opNOTCH->add(*p++);
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
	int sel = opBW->value();
	setXcvrBW (sel);
	setIFshift();
}

void setMode() 
{
	setXcvrMode ((MODES)opMODE->value());
	switch (opMODE->value()) {
	case AM: opBW->value(0); setBW(); break;
	case FM: opBW->value(1); setBW(); break;
	case CW: opBW->value(6); setBW(); break;
	default: opBW->value(2); setBW(); break;
	}
	setInhibits();
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
		oplist[i].mode = CW;
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
		sprintf(szFREQMODE, "%9.3f%4s", oplist[n].freq / 1000.0, szmodes[oplist[n].mode]);
		FreqSelect->add (szFREQMODE);
	}
}

void addtoList(long val, MODES mode) {
	oplist[numinlist].mode = mode;
	oplist[numinlist++].freq = val;
}

void readFile() {
	ifstream iList(defFileName);
	if (!iList) {
		fl_message ("Could not open %s", defFileName);
		return;
	}
	clearList();
	int i = 0, mode;
	long freq;
	while (!iList.eof()) {
		freq = 0L; mode = -1;
		iList >> freq >> mode;
	    if (freq && (mode > -1)) {
			oplist[i].freq = freq;
			oplist[i].imode = mode;
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
	addtoList (10130000L, USB);
	addtoList (3580000L, USB);
	addtoList (14070000L, USB);
	addtoList (21000000L, USB);
	addtoList (7255000L, LSB);
	addtoList (7070000L, USB);
	addtoList (14000000L, CW);
	addtoList (28000000L, USB);
	addtoList (7000000L, CW);
	addtoList (3500000L, CW);
	addtoList (10100000L, CW);
	addtoList (7030000L, USB);
	updateSelect();
}

int movFreq() {
	int ret;
	setXcvrXmtFreq (FreqDisp->value(), xcvrState.TxOffset);
	ret = setXcvrRcvFreq (FreqDisp->value(), 0);
	return ret;
}

	
void cbABsplit()
{
	if (btnSplit->value() == 1) {
		setXcvrSplit();
		setXcvrXmtFreq (vfoB.freq, xcvrState.TxOffset);
		setXcvrListenOnReceive();
		setXcvrRcvFreq (FreqDisp->value(), 0);
	} else {
//		setXcvrSimplex();
		setXcvrSplit();
		setXcvrXmtFreq (FreqDisp->value(),xcvrState.TxOffset);
		setXcvrListenOnReceive();
		setXcvrRcvFreq (FreqDisp->value(),0);
	}
}

void cbABactive()
{
	if (!vfoB.freq) return;
	FREQMODE temp;
	temp.freq = FreqDisp->value();
	temp.imode = opMODE->value();
	FreqDisp->value(vfoB.freq);
	opMODE->value(vfoB.imode);
	vfoB = temp;
	sprintf(szVfoB,"%8ld", vfoB.freq);
	txtInactive->value(szVfoB);
	btnSplit->value(0);
	cbABsplit();
	setMode();
}

void cbA2B()
{
	vfoB.freq = FreqDisp->value();
	vfoB.imode = opMODE->value();
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
	FreqDisp->value(oplist[n].freq);
	opMODE->value(oplist[n].mode);
	setXcvrRcvFreq (oplist[n].freq, 0);
	setXcvrXmtFreq (oplist[n].freq,  0);
	setMode();
}

void delFreq() {
	if (FreqSelect->value()) {
		int n = FreqSelect->value() - 1;
		for (int i = n; i < numinlist; i ++)
			oplist[i] = oplist[i+1];
		oplist[numinlist - 1].mode = USB;
		oplist[numinlist - 1].freq = 0;
		numinlist--;
		updateSelect();
	}
}

void addFreq() {
	long freq = FreqDisp->value();
	if (!freq) return;
	MODES mode = (MODES)opMODE->value();
	for (int n = 0; n < numinlist; n++) 
		if (freq == oplist[n].freq && mode == oplist[n].mode) return;
	addtoList(freq, mode);
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

int ndata = 0;
void parseTelemetry(unsigned char data)
{
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
}


// Radio sends telemetry data at a 50 msec rate

int telpass = 0;
bool teltimeroff = false,
	 wdtimeroff = false;

#ifdef DEBUG
#include "telemetry.cpp"
int loopcount = 5;
#endif

void telemetry(void *d)
{
#ifdef DEBUG
	if (--loopcount == 0) {
		loopcount = 5;
		parseTelemetry(teldata[telptr]);
		if (teldata[++telptr] == -1) telptr = 0;
	}
	setFocus();
	Fl::repeat_timeout(0.05, telemetry);
#else
	extern bool busy;
	unsigned char buff;
	while (commstack.pop(buff)) {
		parseTelemetry(buff);
	}
	setFocus();
	if (!busy) {
		if (KachinaSerial.ReadBuffer (&buff, 1) == 1)
			parseTelemetry(buff);
		Fl::repeat_timeout(0.01, telemetry);
	} else
		Fl::repeat_timeout(0.05, telemetry);
#endif
}

void watchDogTimer(void *d)
{
#ifndef DEBUG
	setXcvrNOOP();
#endif
	Fl::repeat_timeout(15.0, watchDogTimer);
}

void closeTimers()
{
	Fl::remove_timeout(watchDogTimer);
	Fl::remove_timeout(telemetry);
}

void startProcessing(void *d)
{
	if (startComms(szttyport, baudttyport) == 0) {
		fl_message("%s not available", szttyport);
		exit(1);
	}
	startSharedMemory();
	Fl::add_timeout(1.0, watchDogTimer);
	Fl::add_timeout(0.05, telemetry);
	initKachina();
	setInhibits();
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
			oList << oplist[i].freq << " " << oplist[i].mode << endl;
		oList.close();
		strcpy(defFileName, p);
	}
}


/* following code supports shared memory with gMFSK / fldigi application */

struct ST_SHMEM *fmode = (ST_SHMEM *)0;
struct ST_SHMEM sharedmem;

void *shared_memory = (void *)0;

int  shmid;
#define MIDFREQ 1000

int Kachina2Argo(int m)
{
	int n = 0x11;
	switch (m) {
		case 0: n = 0x10; break;
		case 1: n = 0x13; break;
		case 2: n = 0x14; break;
		case 3: n = 0x11; break;
		case 4: n = 0x12; break;
		default: break;
	}
	return n;
}

void sharedMemLoop(void *d)
{
	if (fmode == (ST_SHMEM *)0) return;

	if (fmode->flag == -1) { // gmfsk asking for new value
		fmode->freq  = FreqDisp->value();
		fmode->midfreq = MIDFREQ; // in Hz
		fmode->flag = Kachina2Argo(opMODE->value());
		strcpy(fmode->mode, szmodes[opMODE->value()]);
	} else if (fmode->flag == -2) { // gmfsk setting frequency value
		FreqDisp->value(fmode->freq);
		setXcvrRcvFreq(fmode->freq, 0);
		fmode->flag = Kachina2Argo(opMODE->value());
		if (strcmp(fmode->mode,"USB")==0) {
            opMODE->value(USB);
            setXcvrMode (USB);
            setInhibits();        
        }
        else {
            opMODE->value(LSB);
            setXcvrMode (LSB);
            setInhibits();        
        }
	}
	else if (fmode->flag == -3) { // ptt on
		btnPTT->value(1);
		cbPTT();
		fmode->flag = Kachina2Argo(opMODE->value());
	} else if (fmode->flag == -4) { // ptt off
		btnPTT->value(0);
		cbPTT();
		fmode->flag = Kachina2Argo(opMODE->value());
	}
	Fl::repeat_timeout (0.01, sharedMemLoop);
	return;	
}

void startSharedMemory(void)
{
	shmid = shmget ((key_t)1234, sizeof(ST_SHMEM), 0666 | IPC_CREAT);
	if (shmid < 0) {
		fl_message("shmid failed");
		exit(1);
	}
	shared_memory = shmat (shmid, (void *)0, 0);
 	if (shared_memory == (void *)-1) {
		fl_message("shmat failed");
		exit(1);
	}
	fmode = (ST_SHMEM *) shared_memory;
	Fl::add_timeout (0.05, sharedMemLoop);
}

void closeSharedMemory(void)
{
  Fl::remove_timeout(sharedMemLoop);
  shmctl(shmid, IPC_RMID, 0 );
  fmode = (ST_SHMEM *)0;
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


void cbExit()
{
	closeSharedMemory();
	closeTimers();
	KachinaSerial.ClosePort();
	saveConfig();
	saveState();
	if (test)
		CloseTestLog();
	exit(0);
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
