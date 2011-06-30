#ifndef IOSPEC_H
#define IOSPEC_H

#define STX 0x02
#define ETX 0x03

extern char cmdK_AGC[];  // AGC speed, 00H = fast to FFH = slow

extern char cmdK_AON[];  // Amplifier off.
extern char cmdK_AOF[];  // Amplifier on.

extern char cmdK_BW0[];  // RX BW filter, SSB 3.5kHz.
extern char cmdK_BW1[];  // RX BW filter, SSB 2.7kHz
extern char cmdK_BW2[];  // RX BW filter, SSB 2.4kHz.
extern char cmdK_BW3[];  // RX BW filter, SSB 2.1kHz
extern char cmdK_BW4[];  // RX BW filter, SSB 1.7kHz.
extern char cmdK_BW5[];  // RX BW filter, CW 1kHz.
extern char cmdK_BW6[];  // RX BW filter, CW 500Hz.
extern char cmdK_BW7[];  // RX BW filter, CW 200Hz.
extern char cmdK_BW8[];  // RX BW filter, CW 100Hz.
extern char cmdK_BDH[];  // Data High Filter
extern char cmdK_BDM[];  // Data Medium Filter

extern char cmdK_RSER[]; // BITE - Serial #, H/W & S/W version request.
extern char cmdK_RIMP[]; // BITE - Request antenna impedance data.
extern char cmdK_SIMP[]; // BITE - Send antenna impedance data.
extern char cmdK_RSMTR[];// BITE - Request S-Meter calibration table.
extern char cmdK_SSMTR[];// BITE - Send S-Meter calibration table.
extern char cmdK_PSMTR[];// BITE - Perform S-Meter calibration.
extern char cmdK_RFCAL[];// BITE - Request freq. ref. calibration table.
extern char cmdK_SFCAL[];// BITE - Send freq. ref. calibration table.
extern char cmdK_PFCAL[];// BITE - Perform freq. ref. cal.
extern char cmdK_RPCAL[];// BITE - Request phase det. calibration table.
extern char cmdK_SPCAL[];// BITE - Send phase det. calibration table.
extern char cmdK_PPCAL[];// BITE - Perform phase det. cal.
extern char cmdK_RCBAL[];// BITE - Request carrier balance value.
extern char cmdK_SCBAL[];// BITE - Send carrier balance value.
extern char cmdK_PCBAL[];// BITE - Perform carrier balance.
extern char cmdK_cmd0[]; // Reserved.
extern char cmdK_SM0[]; // BITE - S-Meter cal -130 dBm.
extern char cmdK_SM1[]; // BITE - S-Meter cal -120 dBm.
extern char cmdK_SM2[]; // BITE - S-Meter cal -110 dBm.
extern char cmdK_SM3[]; // BITE - S-Meter cal -100 dBm.
extern char cmdK_SM4[]; // BITE - S-Meter cal  -90 dBm.
extern char cmdK_SM5[]; // BITE - S-Meter cal  -80 dBm.
extern char cmdK_SM6[]; // BITE - S-Meter cal  -70 dBm.
extern char cmdK_SM7[]; // BITE - S-Meter cal  -60 dBm.
extern char cmdK_SM8[]; // BITE - S-Meter cal  -50 dBm.
extern char cmdK_SM9[]; // BITE - S-Meter cal  -40 dBm.
extern char cmdK_SM10[];// BITE - S-Meter cal  -30 dBm.
extern char cmdK_SM11[];// BITE - S-Meter cal  -20 dBm.
extern char cmdK_SM12[];// BITE - S-Meter cal  -10 dBm.
extern char cmdK_SM13[];// BITE - S-Meter cal 0 dBm.
extern char cmdK_SM14[];// BITE - S-Meter cal   10 dBm.
extern char cmdK_SM15[];// BITE - S-Meter cal   20 dBm.
extern char cmdK_cmd1[];// Reserved.
extern char cmdK_SAGC[];// BITE - Send AGC DVM value.
extern char cmdK_cmd2[];// BITE - Send lock detector 1 DVM value.
extern char cmdK_cmd3[];// BITE - Send lock detector 2 DVM value.
extern char cmdK_Qfwd[];// BITE - Send forward power DVM value.
extern char cmdK_Qref[];// BITE - Send reflected power DVM value.
extern char cmdK_Qphs[];// BITE - Send phase detector DVM value.
extern char cmdK_cmd7[];// BITE - Send Tx audio DVM value.
extern char cmdK_Qt_A[];// BITE - Send temperature A DVM value.
extern char cmdK_Qt_B[];// BITE - Send temperature B DVM value.
extern char cmdK_cmdA[];// BITE - Send R->T switching data.
extern char cmdK_cmdB[];// BITE - Send T->R switching data.
extern char cmdK_cmdC[];// BITE - Send synthesizer lock time 1 data.
extern char cmdK_cmdD[];// BITE - Send synthesizer lock time 2 data.
extern char cmdK_cmdE[];// BITE - Send ALC over-shoot data.
extern char cmdK_RTIME[];// BITE - Request on time value.
extern char cmdK_cmdG[];// BITE - Reset on time value.
extern char cmdK_cmdH[];// BITE - Request fault data.
extern char cmdK_cmdI[];// BITE - Reset fault data.
extern char cmdK_cmdJ[];// BITE - Set firmware serial number.
extern char cmdK_cmdK[];// BITE - Perform powerup self test.
extern char cmdK_cmdL[];// BITE - Set password.
extern char cmdK_cmdM[];// BITE - Request TCXO DVM value.
extern char cmdK_cmdN[];// BITE - Request DDS value for Rx frequency.
extern char cmdK_cmdO[];// BITE - Request mode.
extern char cmdK_Qpmx[];// BITE - Request maximum power setting.
extern char cmdK_cmdQ[];// BITE - Perform freq. ref. tilt

extern char cmdK_CWO0[];// CW Offset freq - 300Hz.
extern char cmdK_CWO1[];// CW Offset freq - 400Hz.
extern char cmdK_CWO2[];// CW Offset freq - 500Hz.
extern char cmdK_CWO3[];// CW Offset freq - 600Hz.
extern char cmdK_CWO4[];// CW Offset freq - 700Hz.
extern char cmdK_CWO5[];// CW Offset freq - 800Hz.

extern char cmdK_CWWI[];// CW Filter default - wide.
extern char cmdK_CWNA[];// CW Filter default - narrow.

extern char cmdK_CWDY[];// CW keyer dynamics, 00H to FFH (00H = soft, FFH = hard).

extern char cmdK_NOOP[]; 

extern char cmdK_cmdR[];// Transmit equalization shift frequency,

extern char cmdK_SPOF[];// Speech monitor off.
extern char cmdK_SPON[];// Speech monitor on.

extern char cmdK_VFOM[];// 1 = Simplex on, Rx and Tx are equal.
						// 2 = Listen on the receive frequency.
						// 3 = Listen on the transmit frequency.
						// 4 = Split on.


extern char cmdK_CTSS[];// CTCSS tone frequency for FM,

extern char cmdK_ATT[]; // Receive attenuator as per next two
extern char cmdK_ATT0[];// Receive attenuator off.
extern char cmdK_ATT1[];// Receive attenuator on.

extern char cmdK_AGCA[];// AGC action, 00H to FFH	(00H = min, FFH = max).

extern char cmdK_cmdS[];//	505TVR (00H = off, 01H = on)

extern char cmdK_CMPR[];// Speech compression. 

extern char cmdK_IFSH[];// IF filter shift frequency. 

extern char cmdK_IMPM[]; 

extern char cmdK_RITU[];// RIT 100Hz steps, -99 to -8 and 8 to 99.
extern char cmdK_RITL[];// RIT 10Hz steps, -79 to 79.

extern char cmdK_CWLH[];// CW keyer mode, left hand.
extern char cmdK_CWRH[];// CW keyer mode, right hand.
extern char cmdK_CWST[];// CW keyer mode, straight.

extern char cmdK_CWT0[];// CW spotting tone off.
extern char cmdK_CWT1[];// CW spotting tone on.

extern char cmdK_SQL[]; // Level sensitive squelch value, 00H to 7FH.

extern char cmdK_MODE[];// Mode iaw with following
extern char cmdK_MDAM[];// Mode, AM.
extern char cmdK_MDCW[];// Mode, CW.
extern char cmdK_MDFM[];// Mode, FM.
extern char cmdK_MDUS[];// Mode, USB.
extern char cmdK_MDLS[];// Mode, LSB.

extern char cmdK_MICG[];// Mic/CW gain. Hex value is relative gain

extern char cmdK_NTCW[];// Notch width; 0=wide, 1=medium, 2=narrow,	3=automatic.

extern char cmdK_NTCF[];// Notch frequency in 10Hz steps.

extern char cmdK_NDOF[];// Noise reduction off.
extern char cmdK_NDON[];// Noise reduction on.

extern char cmdK_NDLV[];// Noise reduction level control.

extern char cmdK_SPP0[];// Speech processor off.
extern char cmdK_SPP1[];// Speech processor on.

extern char cmdK_PRE0[];// Pre amp off.
extern char cmdK_PRE1[];// Pre amp on.

extern char cmdK_SQL0[];// Level sensitive squelch (level set to 127).
extern char cmdK_SQL1[];// Syllabic squelch (level set to 127).

extern char cmdK_QSK0[];// CW keyer QSK off.
extern char cmdK_QSK1[];// CW keyer QSK on.

extern char cmdK_RCVF[]; 
extern char cmdK_REFF[];// set Reference Frequency
extern char cmdK_CWSP[];// CW keyer speed, 00H to FFH	(00H = 5 wpm, FFH = 80 wpm).
extern char cmdK_CWSM[];// CW keyer/Speech monitor sidetone, 00H to FFH
extern char cmdK_XMTF[]; 
extern char cmdK_XMTS[]; 

extern char cmdK_ATU0[];// Antenna tuning off.
extern char cmdK_ATU1[];// Antenna tuning on.
extern char cmdK_ATU2[];// Antenna tuning start. A radio with with an

extern char cmdK_ANTA0[];// Clear port A antenna impedance data.
extern char cmdK_ANTB0[];// Clear port B antenna impedance data.

extern char cmdK_VOLU[]; // Audio volume. Hex value is relative volume

extern char cmdK_XDIT[]; // Transmit a CW dit.
extern char cmdK_XDAH[]; // Transmit a CW dah.
extern char cmdK_XLSP[]; // Transmit a CW interletter space.
extern char cmdK_XWSP[]; // Transmit a CW interword space.
extern char cmdK_XABR[]; // Abort transmit of CW message.
extern char cmdK_TUN0[]; // Constant tune carrier off.
extern char cmdK_TUN1[]; // Constant tune carrier on.

extern char cmdK_PWR[];  // Max power out 1 - 100 watts.

extern char cmdK_XWGT[]; // CW keyer weight, 00H to FFH

extern char cmdK_VOXL[]; // VOX level, 00H to FFH (00H = off).

extern char cmdK_PTT[];  // Push to talk (01H = Tx, 00H = Rx).

extern char cmdK_AVXL[]; // AntiVOX level, 00H to FFH.

extern char cmdK_AVXD[]; // VOX Delay, 00H to FFH. (00H = short, FFH = long).

#endif
