#include "dialogs.h"

Fl_Double_Window *dlgFreqCalib = NULL;
Fl_Double_Window *dlgRcvConfig = NULL;
Fl_Double_Window *dlgXmtConfig = NULL;
Fl_Double_Window *dlgCwParams  = NULL;
Fl_Double_Window *dlgAntPorts  = NULL;
Fl_Double_Window *dlgDisplayConfig = NULL;
Fl_Double_Window *dlgCommsConfig = NULL;
Fl_Double_Window *dlgViewLog = NULL;
Fl_Double_Window *dlgNRAM = NULL;

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
	long int refstd = (long int)(cntrFreqStd->value() * 1000000.0);
	int success;
	success = checkCalibrate(refstd);
	switch (success) {
	case 0:
		fl_message("Signal strength too low!");
		break;
	case 1:
		if (fl_choice("Weak signal, use anyway?", "No", "Yes", NULL) == 1) {
			Calibrate();
			fl_message("Calibration completed.");
		}
		break;
	case 2:
		Calibrate();
		fl_message("Calibration completed.");
		break;
	}
}

void cbCalFinished()
{
	closeFreqCalibDialog();
	initXcvrState();
}

//-----------Receiver settings dialog

void openRcvConfigDialog()
{
	if (!dlgRcvConfig)
		dlgRcvConfig = RcvParamDialog();
	dlgRcvConfig->show();
}

void closeRcvDialog()
{
	dlgRcvConfig->hide();
}

void cbsldrAgcAction()
{
	xcvrState.AGCACTION = (int)sldrAgcAction->value();
	cmdK_AGCA[2] = xcvrState.AGCACTION;
	sendCommand(cmdK_AGCA);
}

void cbsldrAgcSpeed()
{
	int agcval;
	xcvrState.AGCSPEED = (int)sldrAgcSpeed->value();
	agcval = 255 - xcvrState.AGCSPEED;
	if (btnNR->value() == 1) {
		agcval -= 0x10;
		if (agcval < 0x02) agcval = 0x02;
	}
	cmdK_AGC[2]  = agcval;
	sendCommand(cmdK_AGC);
}

void cbSqlLevel()
{
	xcvrState.SQLEVEL = (int)sldrSqlLevel->value();
	sldrSQdisp->value(xcvrState.SQLEVEL);
	if (xcvrState.SQLEVEL == -127)
		sldrSQdisp->hide();
	else
		sldrSQdisp->show();
	cmdK_SQL[2]  = -xcvrState.SQLEVEL;
	sendCommand(cmdK_SQL);
}

void cbSQLtype()
{
	if (btnSQLtype[1]->value() == 1)
		xcvrState.SQUELCH = 1;
	else
		xcvrState.SQUELCH = 0;
	cmdK_SQL0[2] = xcvrState.SQUELCH;
	sendCommand(cmdK_SQL0);
}

//-----------Transmit settings dialog

void openXmtConfigDialog()
{
	if (!dlgXmtConfig)
		dlgXmtConfig = XmtParamDialog();
	dlgXmtConfig->show();
}

void closeXmtDialog()
{
	dlgXmtConfig->hide();
}

void cbbtnSpchProc()
{
	xcvrState.SPEECHPROC = btnSpchProc->value();
	cmdK_SPP0[2] = xcvrState.SPEECHPROC;
	sendCommand(cmdK_SPP0);
}

void cbSpchMon()
{
	xcvrState.SPCHMONITOR = btnSpchMon->value();
	cmdK_SPOF[2] = xcvrState.SPCHMONITOR;
	sendCommand(cmdK_SPOF);
}

void cbsldrCompression()
{
	int val = (int)sldrCompression->value();
	xcvrState.SPEECHCOMP = val;
	if (val >= 230) val = 184 + (val - 230)*(255 - 184)/(255 - 230);
	else if (val >= 205) val = 128 + (val - 205)*(184 - 128)/(230 - 205);
	else if (val >= 180) val = 86 + (val - 180)*(128 - 86)/(205 - 180);
	else if (val >= 155) val = 55 + (val - 155)*(86 - 55)/(180 - 155);
	else if (val >= 128) val = 31 + (val - 128)*(55 - 31)/(155 - 128);
	else if (val >= 105) val = 16 + (val - 105)*(31 - 16)/(128 - 105);
	else if (val >= 80) val = 6 + (val - 80)*(16 - 6)/(105 - 80);
	else if (val >= 50) val = 1 + (val - 50)*(6 - 1)/(80 - 50);
	else val = 0;
	cmdK_CMPR[2] = xcvrState.SPEECHCOMP;
	sendCommand(cmdK_CMPR);
}

void cbSidetone()
{
	setXcvrCWMON (sldrSideTone->value());
}

void cbVoxOnOff()
{
	if (btnVoxOnOff->value() == 0) {
		sldrVoxLevel->deactivate();
		cmdK_VOXL[2] = 0;
		sendCommand(cmdK_VOXL);
	} else {
		sldrVoxLevel->activate();
		cmdK_VOXL[2] = xcvrState.VOXLEVEL;
		sendCommand(cmdK_VOXL);
	}
}

void cbsldrVoxLevel()
{
	xcvrState.VOXLEVEL = (int)sldrVoxLevel->value();
	cmdK_VOXL[2] = xcvrState.VOXLEVEL;
	sendCommand(cmdK_VOXL);
}

void cbsldrAntiVox()
{
	xcvrState.ANTIVOX = (int)sldrAntiVox->value();
	cmdK_AVXL[2] = xcvrState.ANTIVOX;
	sendCommand(cmdK_AVXL);
}

void cbsldrVoxDelay()
{
	xcvrState.VOXDELAY = (int)sldrVoxDelay->value();
	cmdK_AVXD[2] = xcvrState.VOXDELAY;
	sendCommand(cmdK_AVXD);
}

void cbsldrXmtEqualizer()
{
	xcvrState.EQUALIZER = (int)sldrXmtEqualizer->value();
	cmdK_cmdR[2] = xcvrState.EQUALIZER;
	sendCommand(cmdK_cmdR);
}

void cbbtnAmpOnOff()
{
	xcvrState.AMP = btnAmpOnOff->value();
	cmdK_AON[2]  = xcvrState.AMP;
	sendCommand(cmdK_AON); 
}


// CW parameters

void cbCWattack()
{
	xcvrState.CWATTACK = (int)sldrCWattack->value();
	cmdK_CWDY[2] = xcvrState.CWATTACK; 		
	sendCommand(cmdK_CWDY);
}

void cbCWweight()
{
	xcvrState.CWWEIGHT = (int)sldrCWweight->value();
	cmdK_XWGT[2] = xcvrState.CWWEIGHT; 		
	sendCommand(cmdK_XWGT);
}

void cbCWmode()
{
	xcvrState.CWMODE = mnuCWmode->value();
	cmdK_CWLH[2] = xcvrState.CWMODE; 		
	sendCommand(cmdK_CWLH);
}

void cbCWoffset()
{
	xcvrState.CWOFFSET = mnuCWoffset->value();
	cmdK_CWO0[2] = xcvrState.CWOFFSET + 3;		
	sendCommand(cmdK_CWO0);
}

void cbCWdefFilter()
{
	xcvrState.CWFILTER = mnuCWdefFilter->value();
	cmdK_CWWI[2] = xcvrState.CWFILTER;		
	sendCommand(cmdK_CWWI);
}

void cbQSKonoff()
{
	xcvrState.QSK = btnQSKonoff->value();
	cmdK_QSK0[2] = xcvrState.QSK; 			
	sendCommand(cmdK_QSK0);
}

void cbCWspot()
{
}

void openCwParamDialog()
{
	if (!dlgCwParams)
		dlgCwParams = CwParamDialog();
	dlgCwParams->show();
}

void closeCwParamDialog()
{
	dlgCwParams->hide();
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


//==============================================================
// Comm port dialog

struct CPT {
  int nbr;
  char *szPort;
  } commPortTable[12];


int  commportnbr = 0;
int  iNbrCommPorts  = 0;
char szCommPorts[512];
char szttyport[20] = "";

bool waitfordialog = false;

void initCommPortTable () {
#ifdef WIN32
  char szTestPort[] = "COMxx";
  for (int i = 0; i < 12; i++) {
    commPortTable[i].nbr = 0;
    commPortTable[i].szPort = 0;
  }
  commPortTable[0].szPort = new char(6);
  strcpy(commPortTable[0].szPort,"None");
  strcpy(szCommPorts,"None");
  iNbrCommPorts = 0;
  for (int i = 1; i < 12; i++) {
	sprintf(szTestPort, "COM%d", i);
    if (KachinaSerial.OpenPort (szTestPort) == true) {
      iNbrCommPorts++;
      commPortTable[iNbrCommPorts].szPort = new char(strlen(szTestPort)+1);
      commPortTable[iNbrCommPorts].nbr = i;
      strcpy(commPortTable[iNbrCommPorts].szPort, szTestPort);
      strcat(szCommPorts,"|");
      strcat(szCommPorts, szTestPort);
    }
	KachinaSerial.ClosePort();
  }
#else
  char szTestPort[] = "ttySx";
  char szTestUSB[] = "ttyUSBx";
  for (int i = 0; i < 8; i++) {
    commPortTable[i].nbr = 0;
    commPortTable[i].szPort = 0;
  }
  commPortTable[0].szPort = new char(6);
  strcpy(commPortTable[0].szPort,"None");
  strcpy(szCommPorts,"None");
  iNbrCommPorts = 0;
  for (int i = 1; i < 8; i++) {
    szTestPort[4] = '0' + i - 1;
    szTestUSB[6] = '0' + i - 1;
    if (KachinaSerial.OpenPort (szTestPort) == true) {
      iNbrCommPorts++;
      commPortTable[iNbrCommPorts].szPort = new char(6);
      commPortTable[iNbrCommPorts].nbr = i;
      strcpy(commPortTable[iNbrCommPorts].szPort, szTestPort);
      strcat(szCommPorts,"|");
      strcat(szCommPorts, szTestPort);
    }
  }
  int j = 0;
  for (int k = iNbrCommPorts; k < 8; k++, j++) {
    szTestUSB[6] = '0' + j - 1;
    if (KachinaSerial.OpenPort (szTestUSB) == true) {
      iNbrCommPorts++;
      commPortTable[iNbrCommPorts].szPort = new char(6);
      commPortTable[iNbrCommPorts].nbr = j + 8;
      strcpy(commPortTable[iNbrCommPorts].szPort, szTestUSB);
      strcat(szCommPorts,"|");
      strcat(szCommPorts, szTestUSB);
    }
	KachinaSerial.ClosePort();
  }
#endif
}

void cbOkCommsDialog()
{
	dlgCommsConfig->hide();
	waitfordialog = false;
	commportnbr = selectCommPort->value();
	strcpy(szttyport,commPortTable[commportnbr].szPort);
}


void setCommsPort()
{
	if (dlgCommsConfig == NULL)
		dlgCommsConfig = CommsDialog();
	initCommPortTable();
	selectCommPort->clear();
	selectCommPort->add(szCommPorts);	
	selectCommPort->value(commportnbr);
	waitfordialog = true;
	dlgCommsConfig->show();
	while (waitfordialog) Fl::wait();
}

// Frequency display colors

uchar fg_red, fg_green, fg_blue;
uchar bg_red, bg_green, bg_blue;
uchar sl_red, sl_green, sl_blue;

void cbPrefBackground()
{
	FreqDisp->GetOFFCOLOR(bg_red, bg_green, bg_blue);
	lblTest->color(fl_rgb_color(bg_red, bg_green, bg_blue));
	const char *title = "Background color";
	if (fl_color_chooser(title, bg_red, bg_green, bg_blue)) {
		FreqDisp->SetOFFCOLOR(bg_red, bg_green, bg_blue);
		lblTest->color(fl_rgb_color (bg_red, bg_green, bg_blue));
	}
	dlgDisplayConfig->redraw();
}

void cbPrefForeground()
{
	FreqDisp->GetONCOLOR(fg_red, fg_green, fg_blue);
	lblTest->labelcolor(fl_rgb_color(fg_red, fg_green, fg_blue));
	const char *title = "Foreground color";
	if (fl_color_chooser(title, fg_red, fg_green, fg_blue)) {
		FreqDisp->SetONCOLOR(fg_red, fg_green, fg_blue);
		lblTest->labelcolor(fl_rgb_color (fg_red, fg_green, fg_blue));
	}
	dlgDisplayConfig->redraw();
}

void cbSelectColor()
{
	FreqDisp->GetSELCOLOR(sl_red, sl_green, sl_blue);
	lblSelect->labelcolor(fl_rgb_color(sl_red, sl_green, sl_blue));
	const char *title = "Digit Select color";
	if (fl_color_chooser(title, sl_red, sl_green, sl_blue)) {
		FreqDisp->SetSELCOLOR(sl_red, sl_green, sl_blue);
		lblSelect->labelcolor(fl_rgb_color (sl_red, sl_green, sl_blue));
	}
	dlgDisplayConfig->redraw();
}

void cbSmeterColor()
{
	uchar red, green, blue;
	Fl::get_color(btnSmeterColor->color(), red, green, blue);
	if (fl_color_chooser("Smeter color", red, green, blue))
		btnSmeterColor->color(fl_rgb_color (red, green, blue));
	dlgDisplayConfig->redraw();
}

void cbPWRcolor()
{
	uchar red, green, blue;
	Fl::get_color(btnPowercolor->color(), red, green, blue);
	if (fl_color_chooser("Power meter color", red, green, blue))
		btnPowercolor->color(fl_rgb_color (red, green, blue));
	dlgDisplayConfig->redraw();
}

void cbSWRcolor()
{
	uchar red, green, blue;
	Fl::get_color(btnSWRcolor->color(), red, green, blue);
	if (fl_color_chooser("SWR meter color", red, green, blue))
		btnSWRcolor->color(fl_rgb_color (red, green, blue));
	dlgDisplayConfig->redraw();
}

void cbOkDisplayDialog()
{
	dlgDisplayConfig->hide();
	FreqDisp->SetONOFFCOLOR(lblTest->labelcolor(), lblTest->color());
	btnSWR->color(btnSWRcolor->color());
	btnSWR->color2(btnSWRcolor->color());
	btnSWR->redraw();
	btnPower->color(btnPowercolor->color());
	btnPower->color2(btnPowercolor->color());
	btnPower->redraw();
	btnSmeter->color(btnSmeterColor->color());
	btnSmeter->color2(btnSmeterColor->color());
	btnSmeter->redraw();
}

void setDisplayColors()
{
	if (dlgDisplayConfig == NULL)
		dlgDisplayConfig = DisplayDialog();
	unsigned char red, green, blue;
	FreqDisp->GetONCOLOR(red,green,blue);
	lblTest->labelcolor(fl_rgb_color(red,green,blue));
	FreqDisp->GetOFFCOLOR(red,green,blue);
	lblTest->color(fl_rgb_color(red,green,blue));
	lblSelect->color(fl_rgb_color(red,green,blue));
	FreqDisp->GetSELCOLOR(red,green,blue);
	lblSelect->labelcolor(fl_rgb_color(red,green,blue));
	btnSWRcolor->color(btnSWR->color());
	btnPowercolor->color(btnPower->color());
	btnSmeterColor->color(btnSmeter->color());
	dlgDisplayConfig->show();
}

// Log Viewer

static Fl_Text_Buffer txtBuffer;
void openLogViewer()
{
	if (!dlgViewLog) {
		dlgViewLog = ViewLogDialog();
		txtViewLog->buffer(&txtBuffer);
	}
}

void closeLogViewer()
{
	if (dlgViewLog)
		dlgViewLog->hide();
}

void cbViewLog()
{
	int dlgshow = mnuViewLog->value();
	if (!dlgViewLog) openLogViewer();
	if (dlgshow)
		dlgViewLog->show();
	else
		dlgViewLog->hide();
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
