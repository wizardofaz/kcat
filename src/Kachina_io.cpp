#include <math.h>
#include <string>

#include "Kachina.h"
#include "IOspec.h"
#include "test.h"
#include "support.h"
#include "cstack.h"
#include "Kachina_io.h"
#include "config.h"
#include "status.h"
#include "debug.h"
#include "util.h"

using namespace std;

extern bool test;

extern CSerialComm KachinaSerial;
// maximum number of retries on sending the command
// Rig may be serial_busy internally and not accept the command

#define ANTA 0x40  // 01000000
#define ANTB 0x80  // 10000000
#define ANTBA 0x00 // 00000000
#define ANTAB 0xC0 // 11000000

#define MAXTRIES 2
#ifdef WIN32
#define LOOPS 20
#else
#define LOOPS 4
#endif

static string cmd = "";

cStack commstack(1000);

unsigned char modes[5] = {'0','1','2','3','4'}; // AM, CW, LSB, USB, FM

bool startComms(const char *szPort, int baudrate)
{
	if (KachinaSerial.OpenPort((char *)szPort, baudrate) == false)
		return false;
	KachinaSerial.Timeout(50); // msec timeout for read from rig
	return true;
}

bool serial_busy = false;

bool sendCmd(string &str) {
	return sendCommand( (char*)str.c_str());
}

bool sendCommand(char *str)
{
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
}

bool RequestData (char *cmd, unsigned char *buff, int nbr)
{
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
}

#include <FL/Fl_Check_Button.H>
static int aports[] = {0x40, 0xC0, 0x00, 0x80}; // AA AB BA BB

int antPort( long freq)
{
	int aport = aports[0];
	if (iAntSel)
		aport = aports[iAntSel - 1];
	else if (numantports > 0) {
		int f = freq / 1000;
		for (int i = numantports - 1; i > -1; i--)
			if (f >= antports[i].freq) {
				aport = aports[ antports[i].rcv + 2*antports[i].xmt ];
				break;
			}
	}
	return aport;
}

void setvfo (char *cmd, long freq, long offset)
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
void setXcvrRcvFreq (long freq, int offset)
{
	setvfo (cmdK_RCVF, freq, offset);
	cmd = cmdK_RCVF;
	sendCmd(cmd);;
	LOG_WARN("%ld %s", freq, str2hex(cmd.c_str(), cmd.length()));
}

void setXcvrSplitFreq (long freq, int offset)
{
	setvfo (cmdK_XMTS, freq, offset);
	cmd = cmdK_XMTS;
	sendCmd(cmd);
	LOG_WARN("%ld %s", freq, str2hex(cmd.c_str(), cmd.length()));
}

void setXcvrXmtFreq (long freq, int offset)
{
	setvfo (cmdK_XMTF, freq, offset);
	cmd = cmdK_XMTF;
	sendCmd(cmd);
	LOG_WARN("%ld %s", freq, str2hex(cmd.c_str(), cmd.length()));
}

void setXcvrSimplex()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x01;
	sendCmd(cmd);
}

void setXcvrSplit()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x04;
	sendCmd(cmd);
}

void setXcvrListenOnReceive()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x02;
	sendCmd(cmd);
}


// Transceiver mode
void setXcvrMode(int mode)
{
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
	cmd = cmdK_MODE;
	sendCmd(cmd);;
}

// Volume control
void setXcvrVolume(double val)
{
	xcvrState.VOL = val;
	int short vol = (int short)(val * 255); // 0 - 0xFF;
	cmdK_VOLU[2] = vol;
	cmd = cmdK_VOLU;
	sendCmd(cmd);;
}

// IF shift control
void setXcvrIFshift(double val)
{
	xcvrState.IFSHIFT = val;
	cmdK_IFSH[2] = (int)(val / 10) &0xFF;
	cmd = cmdK_IFSH;
	sendCmd(cmd);;
}

// Bandwidth controls
void setXcvrBW(int sel)
{
	cmdK_BW[2] = iBW[sel];
	cmd = cmdK_BW;
	sendCmd(cmd);;
}

// Transceiver max power level
void setXcvrPower(double val)
{
	xcvrState.MAXPWR = val;
	cmdK_PWR[2] = (int) val & 0xFF;
	cmd = cmdK_PWR;
	sendCmd(cmd);;
}


// Transceiver Notch Width
void setXcvrNotchWidth(int val)
{
	xcvrState.NOTCHWIDTH = val;
	cmdK_NTCW[2] = val & 0xFF;
	cmd = cmdK_NTCW;
	sendCmd(cmd);;
}

// Transceiver Notch Filter Freq
void setXcvrNotch(double val)
{
	xcvrState.NOTCHFREQ = val;
	int nval = (int)(val) / 10 - 20;
	cmdK_NTCF[2] = nval & 0xFF;
	cmd = cmdK_NTCF;
	sendCmd(cmd);;
}

// Transceiver Noise reduction - Auto notch depth
// This uses the same command structure as NR
// there only Auto Notch AND Noise Reduction are mutually exclusive
// The software toggles between them (see support.cpp)
void setXcvrNotchDepth(double val)
{
	xcvrState.NOTCHDEPTH = val;
	cmdK_NDLV[2] = (int)val & 0xFF;
	cmd = cmdK_NDLV;
	sendCmd(cmd);;
}

// Transceiver Mic/Gain control
void setXcvrMicGain(double val)
{
	xcvrState.MICGAIN = val;
	cmdK_MICG[2] = (int)(val * 255) & 0xFF;
	cmd = cmdK_MICG;
	sendCmd(cmd);;
}

// Transceiver attenuator control
void setXcvrAttControl(int val)
{
	xcvrState.ATTEN = val;
	cmdK_ATT[2] = val & 0xFF;
	cmd = cmdK_ATT;
	sendCmd(cmd);;
}

// Transceiver preamp control
void setXcvrPreamp(int val)
{
	xcvrState.PREAMP = val;
	cmdK_PRE0[2] = val & 0xFF;
	cmd = cmdK_PRE0;
	sendCmd(cmd);;
}

// Transceiver Noise reduction
void setXcvrNR(int val)
{
	xcvrState.NR = val;
	cmdK_NDON[2] = val & 0xFF;
	cmd = cmdK_NDON;
	sendCmd(cmd);;
}

void setXcvrNRlevel(double nr)
{
	xcvrState.NR_LEVEL = nr;
	cmdK_NDLV[2] = (int)nr & 0xFF;
	cmd = cmdK_NDLV;
	sendCmd(cmd);;
}


// Transceiver TUNE mode
void setXcvrTune(int val)
{
	cmdK_ATU0[2] = val & 0xFF;
	cmd = cmdK_ATU0;
	sendCmd(cmd);;
}

// Tranceiver PTT on/off
void setXcvrPTT(int val)
{
	cmdK_PTT[2] = val & 0xFF;
	cmd = cmdK_PTT;
	sendCmd(cmd);;
}

void setXcvrRITfreq(double rit)
{

	int iRIT;
	if (fabs(rit) < 800) {
		iRIT = (int)(rit/10);
		xcvrState.RIT = cmdK_RITL[2] = iRIT;
		cmd = cmdK_RITL;
	} else {
		iRIT = (int)(rit/100);
		xcvrState.RIT = cmdK_RITU[2] = iRIT;
		cmd = cmdK_RITU;
	}
	sendCmd(cmd);;
}

static double cwK = 255.0 / (log(2) * 4.0 );

void setXcvrWPM(double wpm)
{
	xcvrState.CWSPEED = wpm;
	int iWPM = (int)(ceil(cwK * log(wpm/5)));
	cmdK_CWSP[2] = iWPM;
	cmd = cmdK_CWSP;
	sendCmd(cmd);;
}

void setXcvrCWMON(double val)
{
	xcvrState.CWMON = val;
	cmdK_CWSM[2] = (int)(val) & 0xFF;
	cmd = cmdK_CWSM;
	sendCmd(cmd);;
}

void setXcvrSPOT(int val)
{
	cmd = cmdK_CWT0;
	if (val) cmd = cmdK_CWT1;
	sendCmd(cmd);;
}

void setXcvrCarrier(int val)
{
	cmd = cmdK_TUN0;
	if (val == 1) sendCommand(cmdK_TUN1); // constant carrier
	sendCmd(cmd);;
}

void setXcvrNOOP()
{
	cmd = cmdK_NOOP;
	sendCmd(cmd);;
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
	ctr_vfo_adj->value(xcvrState.VFOADJ);

	vfoA.freq = xcvrState.vfoA_freq;
	vfoA.imode = xcvrState.vfoA_imode;
	vfoA.iBW = xcvrState.vfoA_iBW;
	FreqDisp->value(vfoA.freq);
	opMODE->value(vfoA.imode);
	opBW->value(vfoA.iBW);

	vfoB.freq = xcvrState.vfoB_freq;
	vfoB.imode = xcvrState.vfoB_imode;
	vfoB.iBW = xcvrState.vfoB_iBW;
	FreqDispB->value(vfoB.freq);

	movFreq();
	setXcvrSimplex();							// Simplex mode
	setXcvrListenOnReceive();					// Listen on Receive
	setBW();

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
	cbbtnRIT();
	cbIFsh();

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

