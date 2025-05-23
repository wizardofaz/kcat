#include <math.h>
#include <string>

#include "kcat.h"
#include "IOspec.h"
#include "test.h"
#include "support.h"
#include "cstack.h"
#include "kcat_io.h"
#include "config.h"
#include "status.h"
#include "debug.h"
#include "util.h"

#include "threads.h"

using namespace std;

extern bool test;

extern CSerialComm kcatSerial;
// maximum number of retries on sending the command
// Rig may be serial_busy internally and not accept the command

#define ANTA 0x40  // 01000000
#define ANTB 0x80  // 10000000
#define ANTBA 0x00 // 00000000
#define ANTAB 0xC0 // 11000000

#define MAXTRIES 2
#define LOOPS 50

char *cmd;

cStack commstack(1000);

unsigned char modes[5] = {'0','1','2','3','4'}; // AM, CW, LSB, USB, FM

bool startComms(const char *szPort, int baudrate)
{
	if (testing) return true;
	kcatSerial.Device(szPort);
	kcatSerial.Baud(baudrate);
	kcatSerial.Stopbits(1);
	if (kcatSerial.OpenPort() == false)
		return false;
//	kcatSerial.Timeout(50); // msec timeout for read from rig
	return true;
}

//bool serial_busy = false;

string retval;

bool sendCmd(char *str)
{
	if (testing) {
		retval = "TESTING";
		return true;
	}
	int len = str[0];
	int nret, loopcnt;
	bool ret = false;
	unsigned char *sendbuff = new unsigned char(len+2);
	unsigned char retbuff[3];

	guard_lock serial_lock(&mutex_serial);

// create command string
	sendbuff[0] = STX;
	for (int n = 1; n <= len; n++) sendbuff[n] = str[n];
	sendbuff[len+1] = ETX;
	len += 2;

	for (int i = 0; i < MAXTRIES; i++) {
		nret = kcatSerial.WriteBuffer ((const char *)sendbuff, len);
		if (nret != len)
			continue; // write error, retry
		loopcnt = 0;
		do  {
			memset(retbuff, 0, 3);
			nret = kcatSerial.ReadBuffer ((char *)retbuff, 1);
			if (retbuff[0] == 0xFF) { // 505 accepted the command
				retval = "OK";
				ret = true;
				goto cmddone;
			}
			if (retbuff[0] == 0xFE) { // 505 rejected the command
				retval = "REJ";
				goto cmddone;
				break;
			}
			commstack.push(retbuff[0]); // telemetry data
		} while (++loopcnt < LOOPS);
	}
	retval = "FAIL";
cmddone:
	delete [] sendbuff;
	watchdog_count = 1500;
	return ret;
}

bool RequestData (char *cmd, unsigned char *buff, int nbr)
{
	if (testing) {
		retval = "TESTING";
		return false;
	}
	int len = cmd[0];
	int nret, loopcnt;
	bool ret = false;
	unsigned char *sendbuff = new unsigned char(len+2);
	unsigned char retbuff[3];
	char szTemp[10];
	guard_lock serial_lock(&mutex_serial);

// create command string
	sendbuff[0] = STX;
	for (int n = 1; n <= len; n++) sendbuff[n] = cmd[n];
	sendbuff[len+1] = ETX;
	len += 2;

	retval.clear();
	for (int i = 0; i < MAXTRIES; i++) {
		nret = kcatSerial.WriteBuffer ((const char *)sendbuff, len);
		if (nret != len)
			continue; // write error, retry

		loopcnt = 0;
		do  {
			memset(retbuff, 0, 3);
			nret = kcatSerial.ReadBuffer ((char *)retbuff, 1);
			if (retbuff[0] == 0xFD) { // kcat is sending the data
				kcatSerial.ReadBuffer ((char *)buff, nbr);
				for (int i = 0; i < nbr; i++) {
					snprintf(szTemp, sizeof(szTemp), " %02X", buff[i]);
					retval.append(szTemp);
					if (i != 0 && !(i%16)) retval.append("\n");
				}
				ret = true;
				goto reqdone;
			}
			if (retbuff[0] == 0xFE) { // 505 rejected the command
				retval = "REJ";
				goto reqdone;
			}
			commstack.push(retbuff[0]); // telemetry data
		} while (++loopcnt < LOOPS);
	}
	retval = "FAIL";
reqdone:
	delete [] sendbuff;
	return ret;
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

/* convert freq in Hz to 505DSP DDS form and overlay in cmd */
void setvfo (char *cmd, long freq)
{
	long val;

	/* overlay DDS frequency onto one of the cmdK commands */
	val = (unsigned long)floor(0.5 // round to nearest integer 
		+ (2.2369621333 * (75000000.0 + freq))
		+((freq * xcvrState.VFOADJ)*1e-6 + xcvrState.VFO_OFFSET));
	cmd[5] = val & 0xFF; val = val >> 8;
	cmd[4] = val & 0xFF; val = val >> 8;
	cmd[3] = val & 0xFF; val = val >> 8;
	cmd[2] = (val & 0x3F) | antPort(freq);
}

// freq in Hz
void setXcvrRcvFreq (long freq)
{
	setvfo (cmdK_RCVF, freq);
	cmd = cmdK_RCVF;
	sendCmd(cmd);
	LOG_INFO("%ld %s : %s", freq, str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSplitFreq (long freq)
{
	setvfo (cmdK_XMTS, freq);
	cmd = cmdK_XMTS;
	sendCmd(cmd);
	LOG_INFO("%ld %s : %s", freq, str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrXmtFreq (long freq)
{
	setvfo (cmdK_XMTF, freq);
	cmd = cmdK_XMTF;
	sendCmd(cmd);
	LOG_INFO("%ld %s : %s", freq, str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSimplex()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x01;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSplit()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x04;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrListenOnReceive()
{
	cmd = cmdK_VFOM;
	cmd[2] = 0x02;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
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
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Volume control
void setXcvrVolume(double val)
{
	xcvrState.VOL = val;
	int short vol = (int short)(val * 255); // 0 - 0xFF;
	cmdK_VOLU[2] = vol;
	cmd = cmdK_VOLU;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// IF shift control
void setXcvrIFshift(double val)
{
	xcvrState.IFSHIFT = val;
	cmdK_IFSH[2] = (int)(val / 10) &0xFF;
	cmd = cmdK_IFSH;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Bandwidth controls
void setXcvrBW(int sel)
{
	cmdK_BW[2] = iBW[sel];
	cmd = cmdK_BW;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver max power level
void setXcvrPower(double val)
{
	xcvrState.MAXPWR = val;
	cmdK_PWR[2] = (int) val & 0xFF;
	cmd = cmdK_PWR;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}


// Transceiver Notch Width
void setXcvrNotchWidth(int val)
{
	xcvrState.NOTCHWIDTH = val;
	cmdK_NTCW[2] = val & 0xFF;
	cmd = cmdK_NTCW;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver Notch Filter Freq
void setXcvrNotch(double val)
{
	xcvrState.NOTCHFREQ = val;
	int nval = (int)(val) / 10 - 20;
	cmdK_NTCF[2] = nval & 0xFF;
	cmd = cmdK_NTCF;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver Noise reduction - Auto notch depth
// This uses the same command structure as NR
// there only Auto Notch AND Noise Reduction are mutually exclusive
// The software toggles between them (see support.cpp)
// 0 <= val <= 100
void setXcvrNotchDepth(double val)
{
	xcvrState.NOTCHDEPTH = val;
	cmdK_NDLV[2] = ((int)val * 255 / 100) & 0xFF;
	cmd = cmdK_NDLV;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver Mic/Gain control
void setXcvrMicGain(double val)
{
	xcvrState.MICGAIN = val;
	cmdK_MICG[2] = (int)(val * 255) & 0xFF;
	cmd = cmdK_MICG;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver attenuator control
void setXcvrAttControl(int val)
{
	xcvrState.ATTEN = val;
	cmdK_ATT[2] = val & 0xFF;
	cmd = cmdK_ATT;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver preamp control
void setXcvrPreamp(int val)
{
	xcvrState.PREAMP = val;
	cmdK_PRE0[2] = val & 0xFF;
	cmd = cmdK_PRE0;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Transceiver Noise reduction
void setXcvrNR(int val)
{
	xcvrState.NR = val;
	cmdK_NDON[2] = val & 0xFF;
	cmd = cmdK_NDON;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrNRlevel(double nr)
{
	xcvrState.NR_LEVEL = nr;
	cmdK_NDLV[2] = (int)nr & 0xFF;
	cmd = cmdK_NDLV;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}


// Transceiver TUNE mode
void setXcvrTune(int val)
{
	if (val == 0)
		cmd = cmdK_ATU0;
	else if (val == 1)
		cmd = cmdK_ATU1;
	else
		cmd = cmdK_ATU2;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

// Tranceiver PTT on/off
void setXcvrPTT(int val)
{
	cmdK_PTT[2] = val & 0xFF;
	cmd = cmdK_PTT;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
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
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

static double cwK = 255.0 / (log(2) * 4.0 );

void setXcvrWPM(double wpm)
{
	xcvrState.CWSPEED = wpm;
	int iWPM = (int)(floor(cwK * log(wpm/5.0)));
	if (iWPM > 255) iWPM = 255;
	cmdK_CWSP[2] = iWPM;
	cmd = cmdK_CWSP;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWMON(double val)
{
	xcvrState.CWMON = val;
	cmdK_CWSM[2] = (int)(val) & 0xFF;
	cmd = cmdK_CWSM;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSPOT(int val)
{
	cmd = cmdK_CWT0;
	if (val) cmd = cmdK_CWT1;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCarrier(int val)
{
	cmd = cmdK_TUN0;
	if (val == 1) cmd = cmdK_TUN1; // constant carrier
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrEqualizer(int val)
{

	cmd = cmdK_cmdR;
	cmd[2] = 0; // Equalizer set to 0
	if (val)
		cmd[2] = xcvrState.EQUALIZER;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSpeechProcessor(int val)
{
	cmd = cmdK_SPP0;
	cmd[2] = 0; // Speech processor OFF
	if (val)
		cmd[2] = xcvrState.SPEECHPROC;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCompression(int val)
{
	if (val >= 230) val = 184 + (val - 230)*(255 - 184)/(255 - 230);
	else if (val >= 205) val = 128 + (val - 205)*(184 - 128)/(230 - 205);
	else if (val >= 180) val = 86 + (val - 180)*(128 - 86)/(205 - 180);
	else if (val >= 155) val = 55 + (val - 155)*(86 - 55)/(180 - 155);
	else if (val >= 128) val = 31 + (val - 128)*(55 - 31)/(155 - 128);
	else if (val >= 105) val = 16 + (val - 105)*(31 - 16)/(128 - 105);
	else if (val >= 80) val = 6 + (val - 80)*(16 - 6)/(105 - 80);
	else if (val >= 50) val = 1 + (val - 50)*(6 - 1)/(80 - 50);
	else val = 0;
	cmd = cmdK_CMPR;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSpchMon(int val)
{
	cmd = cmdK_SPOF;
	cmd[2] = xcvrState.SPCHMONITOR;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrVoxOnOff(int val)
{
	cmd = cmdK_VOXL;
	cmd[2] = 0;
	if (val) cmd[2] = xcvrState.VOXLEVEL;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrAntiVox(int val)
{
	cmd = cmdK_AVXL;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrVoxDelay(int val)
{
	cmd = cmdK_AVXD;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrAmpOnOff(int val)
{
	cmd = cmdK_AON;
	cmd[2]  = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWattack(int val)
{
	cmd = cmdK_CWDY;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWweight(int val)
{
	cmd = cmdK_XWGT;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWmode(int val)
{
	cmd = cmdK_CWLH;
	cmd[2] = val + 1;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWoffset(int val)
{
	cmd = cmdK_CWO0;
	cmd[2] = val + 3;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrCWdefFilter(int val)
{
	cmd = cmdK_CWWI;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrQSKonoff(int val)
{
	cmd = cmdK_QSK0;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrAGCaction(int val)
{
	cmd = cmdK_AGCA;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrAgcSpeed(int val)
{
	int agcval = 255 - val;
	if (xcvrState.NR) {
		agcval -= 0x10;
		if (agcval < 0x02) agcval = 0x02;
	}
	cmd = cmdK_AGC;
	cmd[2]  = agcval;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSqlLevel(int val)
{
	cmd = cmdK_SQL;
	cmd[2]  = -val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrSqlType(int val)
{
	cmd = cmdK_SQL0;
	cmd[2] = val;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}

void setXcvrNOOP()
{
	cmd = cmdK_NOOP;
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
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
	cntrWPM2->value(xcvrState.CWSPEED);
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
	ctr_vfo_offset->value(xcvrState.VFO_OFFSET);

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

	if (xcvrState.autotune) {
		btn_autotune->value(1);
		sendCmd(cmdK_ATU2);
	}

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
	cmd = cmdK_PFCAL;
	sendCmd(cmd);		// request freq. ref. cal.
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());
}


int checkCalibrate(long refstd)
{
	if (testing) return -1;

	cmd = cmdK_NTCF;
	cmd[2] = 0;						// set notch to OFF
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());

	cmd = cmdK_SQL;
	cmd[2] = 0x7F;					// set squelch to OFF
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());

	cmd = cmdK_AGC;
	cmd[2] = 0;						// set AGC to fast
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());

	setvfo (cmdK_REFF, refstd);		// set the ref freq
	cmd = cmdK_REFF;
	sendCmd(cmd);
	LOG_INFO("%ld %s : %s", refstd, str2hex(cmd, cmd[0]+1), retval.c_str());

	setvfo (cmdK_RCVF, refstd);		// set rcvr to that freq
	cmd = cmdK_RCVF;
	sendCmd(cmd);
	LOG_INFO("%ld %s : %s", refstd, str2hex(cmd, cmd[0]+1), retval.c_str());

	cmd = cmdK_MODE;
	cmd[2] = 2;						// set to CW mode
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());

	cmd = cmdK_BW5;					// set to 1 kHz CW BW
	sendCmd(cmd);
	LOG_INFO("%s : %s", str2hex(cmd, cmd[0]+1), retval.c_str());

	avgrcvsig = 0;
	avgcnt = 0;
	computeavg = true;
	return 0;
}

