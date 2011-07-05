#include <math.h>
#include "Kachina.h"
#include "IOspec.h"
#include "test.h"
#include "support.h"
#include "cstack.h"
#include "Kachina_io.h"
#include "config.h"

extern bool test;

extern CSerialComm KachinaSerial;
// maximum number of retries on sending the command
// Rig may be serial_busy internally and not accept the command

#define ANTA 0x40  // 01000000
#define ANTB 0x80  // 10000000
#define ANTBA 0x00 // 00000000
#define ANTAB 0xC0 // 11000000

struct XCVRSTATE xcvrState = { 
	1, // VFO TYPE 
	0, // NOTCHWIDTH 
	0, // ATTEN
	0, // ANTIVOX
	80, // VOXDELAY
	0, // QSK
	128, // CWATTACK
	128, // CWWEIGHT
	1, // CWMODE
	0, // SQUELCH
	0, // AMP
	0, // SPEECHPROC
	0, // SPEECHCOMP
	1, // ANTTUNE
	4, // CWOFFSET = 700 Hz
	1, // CWFILTER
	0, // EQUALIZER
	0, // PREAMP
	-127, //SQLEVEL
	0, // SPCHMONITOR
	0, // NR
	128, // AGCSPEED
	128, // AGCACTION
	0, // RIT 
	1, // MODE
	6, // BW
	0, // VOXLEVEL
	7030000L, // FREQ
	0.4, // VOL
	0.0, // NOTCHFREQ
	20.0, // MAXPWR
	0.5, // MICGAIN
	0.05, //CWMON 
	20.0, // CWSPEED
	0.0, // NR_LEVEL
	0.0, // RITFREQ
	0.0, // IFSHIFT
	0.0,  // NOTCH DEPTH
	VERSION,
	0,		// MAIN_X
	0,		// MAIN_Y
	0,		// TXOFFSET
	0.0		// VFOADJ
};
	
#define MAXTRIES 2
#ifdef WIN32
#define LOOPS 20
#else
#define LOOPS 4
#endif

cStack commstack(1000);

unsigned char modes[5] = {'0','1','2','3','4'}; // AM, CW, LSB, USB, FM

bool startComms(char *szPort, int baudrate)
{
#ifdef DEBUG
    return true;
#else
	if (KachinaSerial.OpenPort(szPort, baudrate) == false)
		return false;
	KachinaSerial.Timeout(50); // msec timeout for read from rig
	return true;
#endif
}

bool serial_busy = false;

bool sendCommand (char *str)
{
#ifdef DEBUG
    return true;
#else
	int len = str[0];
	int nret, loopcnt;
	unsigned char *sendbuff = new unsigned char(len+2);
	unsigned char retbuff[3];
	char szVal[20];
	
	serial_busy = true;

// create command string	
	sendbuff[0] = STX;
	for (int n = 1; n <= len; n++) sendbuff[n] = str[n];
	sendbuff[len+1] = ETX;

	if (test && sendbuff[1] != 'd') {
		if (sendbuff[1] != 'R' && sendbuff[1] != 't' && sendbuff[1] != 'T') {
			sprintf(szVal,"%c ", sendbuff[1]);
			writeTestLog(szVal);
		}
			for (int i = 2; i <= len; i++) {
				sprintf(szVal,"%02X ", sendbuff[i]);
				writeTestLog(szVal);
			}
	}
	len += 2;
	for (int i = 0; i < MAXTRIES; i++) {
		nret = KachinaSerial.WriteBuffer (sendbuff, len);
		if (nret != len)
			continue; // write error, retry
		loopcnt = 0;
		do  {
			memset(retbuff, 0, 3);
			nret = KachinaSerial.ReadBuffer (retbuff, 1);
			if (retbuff[0] == 0xFF) { // Kachina accepted the command
				if (test && sendbuff[1] != 'd') {
					sprintf(szVal, "\n");
					writeTestLog(szVal);
				}
				serial_busy = false;
				delete [] sendbuff;
				return true;
			}
			if (retbuff[0] == 0xFE) { // Kachina rejected the command
				if (test && sendbuff[1] != 'd') {
					sprintf(szVal," R\n"); 
					writeTestLog(szVal);
				}
				break;
			}
			if (test) {
				sprintf(szVal,">"); 
				writeTestLog(szVal);
			}
			commstack.push(retbuff[0]); // telemetry data
		} while (++loopcnt < LOOPS);		
	}
	serial_busy = false;
	if (test && sendbuff[1] != 'd') {
		sprintf(szVal," F\n"); 
		writeTestLog(szVal);
	}
	delete [] sendbuff;
	return false;
#endif
}

bool RequestData (char *cmd, unsigned char *buff, int nbr)
{
#ifdef DEBUG
    return true;
#else
	int len = cmd[0];
	int nret, loopcnt;
	unsigned char *sendbuff = new unsigned char(len+2);
	unsigned char retbuff[3];
	char szVal[20];

	serial_busy = true;

// create command string	
	sendbuff[0] = STX;
	for (int n = 1; n <= len; n++) sendbuff[n] = cmd[n];
	sendbuff[len+1] = ETX;

	if (test) {
		sprintf(szVal,"%c ", sendbuff[1]);
		writeTestLog(szVal);
		for (int i = 2; i <= len; i++) {
			sprintf(szVal,"%02X ", sendbuff[i]);
			writeTestLog(szVal);
		}
	}
	len += 2;
	for (int i = 0; i < MAXTRIES; i++) {
		nret = KachinaSerial.WriteBuffer (sendbuff, len);
		if (nret != len)
			continue; // write error, retry
		loopcnt = 0;
		do  {
			memset(retbuff, 0, 3);
			nret = KachinaSerial.ReadBuffer (retbuff, 1);
			if (retbuff[0] == 0xFD) { // Kachina is sending the data
				if (test) {
					sprintf(szVal, " %02X", retbuff[0]);
					writeTestLog(szVal);
				}
				KachinaSerial.ReadBuffer (buff, nbr);
				for (int i = 0; i < nbr; i++) {
					sprintf(szVal, " %02X", buff[i]);
					writeTestLog(szVal);
				}
				sprintf(szVal, " OK\n");
				writeTestLog(szVal);
				serial_busy = false;
				delete [] sendbuff;
				return true;
			}
			if (retbuff[0] == 0xFE) { // Kachina rejected the command
				if (test) {
					sprintf(szVal, " %02X Rejected\n", retbuff[0]);
					writeTestLog(szVal);
				}
				break;
			}
			commstack.push(retbuff[0]); // telemetry data
		} while (++loopcnt < LOOPS);		
	}
	serial_busy = false;
	if (test) {
		sprintf(szVal," Failed\n");
		writeTestLog(szVal);
	}
	delete [] sendbuff;
	return false;
#endif
}

int aPort(int r, int x)
{
	int v = r*2 + x;
	switch (v) {
		case 0: return 0x40; // rx = A, tx = A
		case 1: return 0x00; // rx = A, tx = B
		case 2: return 0xC0; // rx = B, tx = A
		case 3: return 0x80; // rx = B, tx = B
	}
	return 0x40;
}

#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *btnSelAnt;
extern bool bRxAnt;
extern bool bTxAnt;

int antPort( long freq)
{
	if (numantports == 0) 
		return 0x40;  // use port A for receive & transmit
// if SelAnt is checked then override the table entries for antenna selection
	if (btnSelAnt->value() == 1)
		return aPort(bRxAnt, bTxAnt);
		
	int f = freq / 1000;
	for (int i = numantports - 1; i > -1; i--)
		if (f >= antports[i].freq) 
			return aPort(antports[i].rcv, antports[i].xmt);
	return 0x40;
}

void setvfo (char  *cmd, long freq, long offset)
{
	long val = 0;
	freq *= (1 + xcvrState.VFOADJ * 1e-6);
	val = (long)ceil(2.2369621333 * (75000000 + freq - offset));
	cmd[5] = val & 0xFF; val = val >> 8;
	cmd[4] = val & 0xFF; val = val >> 8;
	cmd[3] = val & 0xFF; val = val >> 8;
	cmd[2] = (val & 0x3F) | antPort(freq);
}

// freq in Hz
bool setXcvrRcvFreq (long freq, int offset) 
{
	xcvrState.FREQ = freq;
	setvfo (cmdK_RCVF, freq, offset);
	if (test) {
		char szVal[20];
		sprintf(szVal,"R %ld ", freq);
		writeTestLog(szVal);
	}
	return (!sendCommand (cmdK_RCVF));
}

bool setXcvrSplitFreq (long freq, int offset)
{
	setvfo (cmdK_XMTS, freq, offset);
	if (test) {
		char szVal[20];
		sprintf(szVal,"t %ld ", freq - offset);
		writeTestLog(szVal);
	}
	return (sendCommand (cmdK_XMTS));
}

bool setXcvrXmtFreq (long freq, int offset)
{
	setvfo (cmdK_XMTF, freq, offset);
	if (test) {
		char szVal[20];
		sprintf(szVal,"T %ld ", freq - offset);
		writeTestLog(szVal);
	}
	return (sendCommand (cmdK_XMTF));
}

bool setXcvrSimplex()
{
	cmdK_VFOM[2] = 0x01;		
	return (sendCommand (cmdK_VFOM));
}

bool setXcvrSplit()
{
	cmdK_VFOM[2] = 0x04;
	return (sendCommand (cmdK_VFOM));
}

bool setXcvrListenOnReceive()
{
	cmdK_VFOM[2] = 0x02;
	return (sendCommand (cmdK_VFOM));
}


// Transceiver mode
bool setXcvrMode(int mode) 
{
	xcvrState.MODE = mode;
	switch (mode) {
		case LSB :
			cmdK_MODE[2] = 0x05;
			break;
		case USB :
			cmdK_MODE[2] = 0x04;
			break;
		case CW :
			cmdK_MODE[2] = 0x02;
			break;
		case AM :
			cmdK_MODE[2] = 0x01;
			break;
		case FM :
			cmdK_MODE[2] = 0x03;
			break;
		default :
			cmdK_MODE[2] = 0x04;
	}
	return (sendCommand(cmdK_MODE));
}

// Volume control
bool setXcvrVolume(double val) 
{
	xcvrState.VOL = val;
	int short vol = (int short)(val * 255); // 0 - 0xFF;
	cmdK_VOLU[2] = vol;
	return (sendCommand(cmdK_VOLU));
}

// IF shift control
bool setXcvrIFshift(double val) 
{
	xcvrState.IFSHIFT = val;
	cmdK_IFSH[2] = (int)(val / 10) &0xFF;
	return (sendCommand(cmdK_IFSH));
}

// Bandwidth controls
bool setXcvrBW(int sel) 
{
	xcvrState.BW = sel;
	cmdK_BW[2] = iBW[sel];
	sendCommand(cmdK_BW);
	return 0;
}

// Transceiver max power level
bool setXcvrPower(double val)
{
	xcvrState.MAXPWR = val;
	cmdK_PWR[2] = (int) val & 0xFF;
	return (sendCommand(cmdK_PWR));
}


// Transceiver Notch Width
bool setXcvrNotchWidth(int val)
{
	xcvrState.NOTCHWIDTH = val;
	cmdK_NTCW[2] = val & 0xFF;
	return (sendCommand(cmdK_NTCW));
}

// Transceiver Notch Filter Freq
bool setXcvrNotch(double val)
{
	xcvrState.NOTCHFREQ = val;
	int nval = (int)(val) / 10 - 20;
	cmdK_NTCF[2] = nval & 0xFF;
	return (sendCommand(cmdK_NTCF));
}

// Transceiver Noise reduction - Auto notch depth
// This uses the same command structure as NR
// there only Auto Notch AND Noise Reduction are mutually exclusive
// The software toggles between them (see support.cpp)
bool setXcvrNotchDepth(double val)
{
	xcvrState.NOTCHDEPTH = val;
	cmdK_NDLV[2] = (int)val & 0xFF;
	return (sendCommand(cmdK_NDLV));
}

// Transceiver Mic/Gain control
bool setXcvrMicGain(double val)
{
	xcvrState.MICGAIN = val;
	cmdK_MICG[2] = (int)(val * 255) & 0xFF;
	return (sendCommand(cmdK_MICG));
}

// Transceiver attenuator control
bool setXcvrAttControl(int val)
{
	xcvrState.ATTEN = val;
	cmdK_ATT[2] = val & 0xFF;
	return (sendCommand(cmdK_ATT));
}

// Transceiver preamp control
bool setXcvrPreamp(int val)
{
	xcvrState.PREAMP = val;
	cmdK_PRE0[2] = val & 0xFF;
	return (sendCommand(cmdK_PRE0));
}

// Transceiver Noise reduction
bool setXcvrNR(int val)
{
	xcvrState.NR = val;
	cmdK_NDON[2] = val & 0xFF;
	return (sendCommand(cmdK_NDON));
}

bool setXcvrNRlevel(double nr)
{
	xcvrState.NR_LEVEL = nr;
	cmdK_NDLV[2] = (int)nr & 0xFF;
	return (sendCommand(cmdK_NDLV));
}


// Transceiver TUNE mode
bool setXcvrTune(int val)
{
	cmdK_ATU0[2] = val & 0xFF;
	return (sendCommand(cmdK_ATU0));
}

// Tranceiver PTT on/off
bool setXcvrPTT(int val)
{
	cmdK_PTT[2] = val & 0xFF;
	return (sendCommand(cmdK_PTT));
}

bool setXcvrRITfreq(double rit)
{
	
	int iRIT;
	if (fabs(rit) < 800) {
		iRIT = (int)(rit/10);
		xcvrState.RIT = cmdK_RITL[2] = iRIT;
		return sendCommand(cmdK_RITL);
	} else {
		iRIT = (int)(rit/100);
		xcvrState.RIT = cmdK_RITU[2] = iRIT;
		return sendCommand(cmdK_RITU);
	}
}

static double cwK = 255.0 / (log(2) * 4.0 );

bool setXcvrWPM(double wpm)
{
	xcvrState.CWSPEED = wpm;
	int iWPM = (int)(ceil(cwK * log(wpm/5)));
	cmdK_CWSP[2] = iWPM; 		
	return sendCommand(cmdK_CWSP);
}

bool setXcvrCWMON(double val)
{
	xcvrState.CWMON = val;
	cmdK_CWSM[2] = (int)(val) & 0xFF;
	return sendCommand(cmdK_CWSM);
}

bool setXcvrCarrier(int val)
{
	if (val == 1) {
		sendCommand(cmdK_TUN1); // constant carrier
	} else {
		sendCommand(cmdK_TUN0); // carrier off
	}
	return true;
}

bool setXcvrNOOP()
{
	return sendCommand(cmdK_NOOP);
}

void initXcvrState()
{
	sldrNOTCH->value(xcvrState.NOTCHFREQ);
	sldrDepth->value(xcvrState.NOTCHDEPTH);
	opNOTCH->value (xcvrState.NOTCHWIDTH);
	btnAttenuator->value (xcvrState.ATTEN);
	btnPreamp->value(xcvrState.PREAMP);
	sldrVOLUME->value (xcvrState.VOL);
	sldrPOWER->value (xcvrState.MAXPWR);
	sldrMICGAIN->value (xcvrState.MICGAIN);
	sldrSideTone->value (xcvrState.CWMON);
	sldrAntiVox->value(xcvrState.ANTIVOX);
	sldrVoxDelay->value(xcvrState.VOXDELAY);
	sldrVoxLevel->value(xcvrState.VOXLEVEL);
	cntrWPM->value(xcvrState.CWSPEED);
	sldrCWattack->value(xcvrState.CWATTACK);
	sldrCWweight->value(xcvrState.CWWEIGHT);
	sldrCompression->value(xcvrState.SPEECHCOMP);
	sldrXmtEqualizer->value(xcvrState.EQUALIZER);
	sldrSqlLevel->value(xcvrState.SQLEVEL);
	sldrAgcSpeed->value(xcvrState.AGCSPEED);
	sldrAgcAction->value(xcvrState.AGCACTION);
	sldrNR->value (xcvrState.NR_LEVEL);
	sldrIFSHIFT->value(xcvrState.IFSHIFT);	
	sldrRIT->value(0.0);
	opMODE->value(xcvrState.MODE);
	
	mnuCWmode->value(xcvrState.CWMODE);	
	mnuCWoffset->value(xcvrState.CWOFFSET);
	mnuCWdefFilter->value(xcvrState.CWFILTER);	

	btnQSKonoff->value(xcvrState.QSK);
	btnNotch->value(0);								// manual Notch
	btnVoxOnOff->value(0);
	btnAmpOnOff->value(xcvrState.AMP);
	btnSpchProc->value(xcvrState.SPEECHPROC);
	btnSpchMon->value(xcvrState.SPCHMONITOR);
	btnNR->value (xcvrState.NR);
	btnRIT->value(0);
	
	sendCommand (cmdK_NOOP);						// send NOOP command

	FreqDisp->value (xcvrState.FREQ);				// Set Display Frequency
	setXcvrRcvFreq (xcvrState.FREQ, 0);				// Receive frequency
	setXcvrSplitFreq (xcvrState.FREQ, 0);				// Transmit frequency
	setXcvrSimplex();								// Simplex mode
	setXcvrListenOnReceive();						// Listen on Receive
	setXcvrNotch(xcvrState.NOTCHFREQ);

	cbbtnNotch();
	cbAttenuator();
	cbPreamp();
	setVolume();
	setPower();
	setMicGain();
	cbSidetone();
	cbsldrAntiVox();
	cbsldrVoxDelay();
	cbsldrVoxLevel();
	cbVoxOnOff();
	cbQSKonoff();
	cbWPM();
	cbCWattack();
	cbCWweight();
	cbCWmode();

	btnSQLtype[1]->value(xcvrState.SQUELCH);
	btnSQLtype[0]->value(!xcvrState.SQUELCH);
	cbSQLtype();
	
	cbbtnAmpOnOff();
	cbbtnSpchProc();	
	cbsldrCompression();

	cmdK_ATU0[2] = xcvrState.ANTTUNE;	// ****
	sendCommand(cmdK_ATU0);

	cbCWoffset();
	cbCWdefFilter();
	cbsldrXmtEqualizer();	
	cbSqlLevel();
	cbSpchMon()	;
	cbsldrAgcSpeed();
	cbNR();
	cbbtnNR();
	cbsldrAgcAction();
	cbRIT();
	cbbtnRIT();
	setMode();
	setIFshift();
	
	setXcvrRcvFreq (xcvrState.FREQ, 0);
	setXcvrListenOnReceive();
}

void Calibrate()
{
	sendCommand(cmdK_PFCAL);		// request freq. ref. cal.
}

int checkCalibrate(long int refstd)
{
	setvfo (cmdK_REFF, refstd, 0);	// set the ref freq
	sendCommand (cmdK_REFF);
	setvfo (cmdK_RCVF, refstd, 0);	// set rcvr to that freq
	sendCommand (cmdK_RCVF);
	cmdK_MODE[2] = 2;				// set to CW mode
	sendCommand (cmdK_MODE);
	cmdK_NTCF[2] = 0;				// set notch to OFF
	sendCommand (cmdK_NTCF);
	cmdK_SQL[2] = 0x7F;				// set squelch to max
	sendCommand (cmdK_SQL);
	cmdK_AGC[2] = 0;				// set AGC to fast
	sendCommand (cmdK_AGC);
	sendCommand (cmdK_BW5);			// set to 1 kHz CW BW
	cmdK_VOLU[2] = 0x80;			// set volume to mid scale
	sendCommand (cmdK_VOLU);
	avgrcvsig = 0;
	avgcnt = 0;
	computeavg = true;
	while (avgcnt < 32) 
		Fl::wait();
	computeavg = false;
	avgrcvsig /= avgcnt;
	if (avgrcvsig < 101) return 2;
	if (avgrcvsig < 110) return 1;
	return 0;
}

