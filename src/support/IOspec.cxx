// This document describes the command and telemetry communication interface
// between the firmware in the 505DSP radio and the control software in the
// user's personal computer (PC). All command and telemetry data will be
// sent as ASCII character data. Each command "packet" will have a preamble
// ASCII STX character and a postamble ASCII ETX character.

// COMMAND STRUCTURE (PC to Radio)
// Upon receipt of each command, the radio will send a single acknowledgement
// byte to the PC. The format of this byte will be:
//		0FFH - good command
//		0FEH - error

//On error, the PC will make 2 retries before declaring a problem to the user.

//#include "IOspec.h"
//Command  Definition (first byte is command length)
//-------  ----------
unsigned char cmdK_AGC[] = {2,'A',0x00}; // AGC speed, 00H = fast to FFH = slow

unsigned char cmdK_AON[] = {2,'a',0x00}; // Amplifier off.
unsigned char cmdK_AOF[] = {2,'a',0x01}; // Amplifier on.

unsigned char cmdK_BW[]  = {2,'B',0x00}; // never sent unless byte 2 set
unsigned char cmdK_BW0[] = {2,'B',0x01}; // RX BW filter, SSB 3.5kHz.
unsigned char cmdK_BW1[] = {2,'B',0x02}; // RX BW filter, SSB 2.7kHz
unsigned char cmdK_BW2[] = {2,'B',0x03}; // RX BW filter, SSB 2.4kHz.
unsigned char cmdK_BW3[] = {2,'B',0x04}; // RX BW filter, SSB 2.1kHz
unsigned char cmdK_BW4[] = {2,'B',0x05}; // RX BW filter, SSB 1.7kHz.
unsigned char cmdK_BW5[] = {2,'B',0x06}; // RX BW filter, CW 1kHz.
unsigned char cmdK_BW6[] = {2,'B',0x07}; // RX BW filter, CW 500Hz.
unsigned char cmdK_BW7[] = {2,'B',0x08}; // RX BW filter, CW 200Hz.
unsigned char cmdK_BW8[] = {2,'B',0x09}; // RX BW filter, CW 100Hz.
unsigned char cmdK_BDH[] = {2,'B',0x0a}; // Data High Filter
unsigned char cmdK_BDM[] = {2,'B',0x0b}; // Data Medium Filter

unsigned char cmdK_RSER[] = {2,'b',0x00}; // BITE - Serial #, H/W & S/W version request.
unsigned char cmdK_RIMP[] = {2,'b',0x01}; // BITE - Request antenna impedance data.
unsigned char cmdK_RFCAL[] = {2,'b',0x06}; // BITE - Request freq. ref. calibration table.
unsigned char cmdK_RSMTR[] = {2,'b',0x03}; // BITE - Request S-Meter calibration table.
unsigned char cmdK_PFCAL[] = {2,'b',0x08}; // BITE - Perform freq. ref. cal.
unsigned char cmdK_RPCAL[] = {2,'b',0x09}; // BITE - Request phase det. calibration table.
unsigned char cmdK_RCBAL[] = {2,'b',0x0c}; // BITE - Request carrier balance value.

unsigned char cmdK_RTIME[] = {2,'b',0x2f}; // BITE - Request on time value.

unsigned char cmdK_cmdM[] = {2,'b',0x36}; // BITE - Request TCXO DVM value.
unsigned char cmdK_cmdN[] = {2,'b',0x37}; // BITE - Request DDS value for Rx frequency.
unsigned char cmdK_cmdO[] = {2,'b',0x38}; // BITE - Request mode.
unsigned char cmdK_Rpmx[] = {2,'b',0x39}; // BITE - Request maximum power setting.

unsigned char cmdK_CWO0[] = {2,'C',0x03}; // CW Offset freq - 300Hz.
unsigned char cmdK_CWO1[] = {2,'C',0x04}; // CW Offset freq - 400Hz.
unsigned char cmdK_CWO2[] = {2,'C',0x05}; // CW Offset freq - 500Hz.
unsigned char cmdK_CWO3[] = {2,'C',0x06}; // CW Offset freq - 600Hz.
unsigned char cmdK_CWO4[] = {2,'C',0x07}; // CW Offset freq - 700Hz.
unsigned char cmdK_CWO5[] = {2,'C',0x08}; // CW Offset freq - 800Hz.

unsigned char cmdK_CWWI[] = {2,'c',0x00}; // CW Filter default - wide.
unsigned char cmdK_CWNA[] = {2,'c',0x01}; // CW Filter default - narrow.

unsigned char cmdK_CWDY[] = {2,'D',0x00}; // CW keyer dynamics, 00H to FFH
//				(00H = soft, FFH = hard).

// NO-OP keep alive command. Sent every
// 15 seconds so radio doesn't close
// serial connection to the PC.
unsigned char cmdK_NOOP[] = {2,'d',0x00}; 

unsigned char cmdK_cmdR[] = {2,'E',0x00}; // Transmit equalization shift frequency,
//				Bass - Treble. The values range from
//				-128 to 127.

unsigned char cmdK_SPOF[] = {2,'e',0x00}; // Speech monitor off.
unsigned char cmdK_SPON[] = {2,'e',0x01}; // Speech monitor on.

unsigned char cmdK_VFOM[] = {2,'F',0X01}; // 1 = Simplex on, Rx and Tx are equal.
								 // 2 = Listen on the receive frequency.
								 // 3 = Listen on the transmit frequency.
								 // 4 = Split on.

unsigned char cmdK_CTSS[] = {2,'f',0x00}; // CTCSS tone frequency for FM,
//				00H to 2AH (0 is off and 01-2A
//				correspond to 42 frequency codes).

unsigned char cmdK_ATT[] = {2,'G',0X00}; // Receive attenuator as per next two
unsigned char cmdK_ATT0[] = {2,'G',0x00}; // Receive attenuator off.
unsigned char cmdK_ATT1[] = {2,'G',0x01}; // Receive attenuator on.

unsigned char cmdK_AGCA[] = {2,'g',0x00}; // AGC action, 00H to FFH
//				(00H = min, FFH = max).

unsigned char cmdK_cmdS[] = {2,'h',0x00}; //		505TVR (00H = off, 01H = on)

unsigned char cmdK_CMPR[] = {2,'H',0x00}; // Speech compression. Hex value is
//				relative compression with 00H being
//				minimum compression and 0FFH being
//				maximum compression.

unsigned char cmdK_IFSH[] = {2,'I',0x00}; // IF filter shift frequency. Step size is
//				10Hz. Hex value is determined by the
//				equation (Hz / 10) + 128. The frequency
//				range is -1280Hz to 1270Hz.

unsigned char cmdK_IMPM[] = {3,'i',0x00,0x00}; 
//   Impedance matching, used by KCCAL to get
//				50 ohm capacitance/inductance load. Value
//				is determined by the following bits.
//				Bit 0  : 20 PF
//				Bit 1  : 40 PF
//				Bit 2  : 80 PF
//				Bit 3  : 160 PF
//				Bit 4  : 320 PF
//				Bit 5  : 640 PF
//				Bit 6  : 1280 PF
//				Bit 7  : 0 = output cap., 1 = input cap.
//				Bits 8-13 : inductance 0-63

unsigned char cmdK_RITU[] = {2,'J',0x00}; // RIT 100Hz steps, -99 to -8 and 8 to 99.
//				This covers the ranges -9900Hz to -800Hz
//				and 800Hz to 9900Hz.

unsigned char cmdK_RITL[] = {2,'j',0x00}; // RIT 10Hz steps, -79 to 79.
//				This covers the ranges -790Hz to 790Hz.

unsigned char cmdK_CWLH[] = {2,'K',0x01}; // CW keyer mode, left hand.
unsigned char cmdK_CWRH[] = {2,'K',0x02}; // CW keyer mode, right hand.
unsigned char cmdK_CWST[] = {2,'K',0x03}; // CW keyer mode, straight.

unsigned char cmdK_CWT0[] = {2,'k',0x00}; // CW spotting tone off.
unsigned char cmdK_CWT1[] = {2,'k',0x01}; // CW spotting tone on.

unsigned char cmdK_SQL[] = {2,'L',0x00}; // Level sensitive squelch value, 00H to 7FH.

unsigned char cmdK_MODE[] = {2,'M',0X00}; // Mode iaw with following
unsigned char cmdK_MDAM[] = {2,'M',0x01}; // Mode, AM.
unsigned char cmdK_MDCW[] = {2,'M',0x02}; // Mode, CW.
unsigned char cmdK_MDFM[] = {2,'M',0x03}; // Mode, FM.
unsigned char cmdK_MDUS[] = {2,'M',0x04}; // Mode, USB.
unsigned char cmdK_MDLS[] = {2,'M',0x05}; // Mode, LSB.

unsigned char cmdK_MICG[] = {2,'m',0x00}; // Mic/CW gain. Hex value is relative gain
//				level with 00H being minimum gain and
//				0FFH being maximum gain.

unsigned char cmdK_NTCW[] = {2,'N',0x03}; // Notch width; 0=wide, 1=medium, 2=narrow,
//				3=automatic.

unsigned char cmdK_NTCF[] = {2,'n',0x00}; // Notch frequency in 10Hz steps. Hex value is
//				determined by the equation (Hz / 10) - 20.
//				The range is 210Hz to 2750Hz and a value of
//				zero indicates the notch is turned off.

unsigned char cmdK_NDOF[] = {2,'O',0x00}; // Noise reduction off.Request
unsigned char cmdK_NDON[] = {2,'O',0x01}; // Noise reduction on.

unsigned char cmdK_NDLV[] = {2,'o',0x00}; // Noise reduction level control.
//				Hex value is relative level with 00H
//				being minimum and 0FFH being maximum.

unsigned char cmdK_SPP0[] = {2,'P',0x00}; // Speech processor off.
unsigned char cmdK_SPP1[] = {2,'P',0x01}; // Speech processor on.

unsigned char cmdK_PRE0[] = {2,'p',0x00}; // Pre amp off.
unsigned char cmdK_PRE1[] = {2,'p',0x01}; // Pre amp on.

unsigned char cmdK_SQL0[] = {2,'Q',0x00}; // Level sensitive squelch (level set to 127).
unsigned char cmdK_SQL1[] = {2,'Q',0x01}; // Syllabic squelch (level set to 127).

unsigned char cmdK_QSK0[] = {2,'q',0x00}; // CW keyer QSK off.
unsigned char cmdK_QSK1[] = {2,'q',0x01}; // CW keyer QSK on.

unsigned char cmdK_RCVF[] = {5,'R',0x00,0x00,0x00,0x00}; 
// DDS value for receive frequency.
//				The allowable range will be 30kHz to 30MHz.
//				The uppermost 2 bits of the high byte will
//				indicate antenna port as follows:
//					00 - port B/A
//					01 - port A
//					10 - port B
//					11 - port A/B

//				The DDS value is computed using:

//				DDS = 2.2369621333 * (75000000 + freq)
//				where freq = input frequency in Hz.

unsigned char cmdK_REFF[] = {5,'r',0x00,0x00,0x00,0x00}; 
// DDS value for reference calibration frequency.
//				The allowable range will be 30kHz to 30MHz.

//				The DDS value is computed using:

//				DDS = 2.2369621333 * (75000000 + freq)
//				where freq = input frequency in Hz.

unsigned char cmdK_CWSP[] = {2,'S',0x00}; // CW keyer speed, 00H to FFH
//				(00H = 5 wpm, FFH = 80 wpm).

unsigned char cmdK_CWSM[] = {2,'s',0x00}; // CW keyer/Speech monitor sidetone, 00H to FFH
//				(00H = min, FFH = max).

unsigned char cmdK_XMTF[] = {5,'T',0x00,0x00,0x00,0x00}; 
// DDS value for transmit frequency. The
//				allowable range will be 30kHz to 30MHz (the
//				radio will restrict the range to 1.8-30MHz).
//				The uppermost 2 bits of the high byte will
//				indicate antenna port as follows:
//					00 - port B/A
//					01 - port A
//					10 - port B
//					11 - port A/B

//				The DDS value is computed using:

//				DDS = 2.2369621333 * (75000000 + freq)
//				where freq = input frequency in Hz.

unsigned char cmdK_XMTS[] = {5,'t',0x00,0x00,0x00,0x00}; 
// Save DDS value for transmit frequency,
//				but do not tune the radio.

unsigned char cmdK_ATU0[] = {2,'U',0x00}; // Antenna tuning off.
unsigned char cmdK_ATU1[] = {2,'U',0x01}; // Antenna tuning on.
unsigned char cmdK_ATU2[] = {2,'U',0x02}; // Antenna tuning start. A radio with with an
//				ATU will make an audible pass/fail tone after
//				the antenna tuning cycle completes.
unsigned char cmdK_ANTA0[] = {2,'U',0x03}; // Clear port A antenna impedance data.
unsigned char cmdK_ANTB0[] = {2,'U',0x04}; // Clear port B antenna impedance data.

unsigned char cmdK_VOLU[] = {2,'V',0x10}; // Audio volume. Hex value is relative volume
//				level with 00H being minimum volume and 0FFH
//				being maximum volume.

unsigned char cmdK_XDIT[] = {2,'v',0x00}; // Transmit a CW dit.
unsigned char cmdK_XDAH[] = {2,'v',0x01}; // Transmit a CW dah.
unsigned char cmdK_XLSP[] = {2,'v',0x02}; // Transmit a CW interletter space.
unsigned char cmdK_XWSP[] = {2,'v',0x03}; // Transmit a CW interword space.
unsigned char cmdK_XABR[] = {2,'v',0x04}; // Abort transmit of CW message.
unsigned char cmdK_TUN0[] = {2,'v',0x05}; // Constant tune carrier off.
unsigned char cmdK_TUN1[] = {2,'v',0x06}; // Constant tune carrier on.

unsigned char cmdK_PWR[] = {2,'W',0x00}; // Max power out 1 - 100 watts.

unsigned char cmdK_XWGT[] = {2,'w',0x80}; // CW keyer weight, 00H to FFH
//				(00H = light, FFH = heavy).

unsigned char cmdK_VOXL[] = {2,'X',0x00}; // VOX level, 00H to FFH (00H = off).

unsigned char cmdK_PTT[]  = {2,'x',0x00}; // Push to talk (01H = Tx, 00H = Rx).

unsigned char cmdK_AVXL[] = {2,'Y',0x00}; // AntiVOX level, 00H to FFH.

unsigned char cmdK_AVXD[] = {2,'y',0x00}; // VOX Delay, 00H to FFH.

//				(00H = short, FFH = long).

// unused commands in kcat

//unsigned char cmdK_SIMP[] = {2,'b',0x02}; // BITE - Send antenna impedance data.
//unsigned char cmdK_SSMTR[] = {2,'b',0x04}; // BITE - Send S-Meter calibration table.
//unsigned char cmdK_PSMTR[] = {2,'b',0x05}; // BITE - Perform S-Meter calibration.
//unsigned char cmdK_SFCAL[] = {2,'b',0x07}; // BITE - Send freq. ref. calibration table.
//unsigned char cmdK_SPCAL[] = {2,'b',0x0a}; // BITE - Send phase det. calibration table.
//unsigned char cmdK_PPCAL[] = {2,'b',0x0b}; // BITE - Perform phase det. cal.
//unsigned char cmdK_SCBAL[] = {2,'b',0x0d}; // BITE - Send carrier balance value.
//unsigned char cmdK_PCBAL[] = {2,'b',0x0e}; // BITE - Perform carrier balance.
//unsigned char cmdK_cmd0[] = {2,'b',0x0f}; // Reserved.
//unsigned char cmdK_SM0[] = {2,'b',0x10}; // BITE - S-Meter cal -130 dBm.
//unsigned char cmdK_SM1[] = {2,'b',0x11}; // BITE - S-Meter cal -120 dBm.
//unsigned char cmdK_SM2[] = {2,'b',0x12}; // BITE - S-Meter cal -110 dBm.
//unsigned char cmdK_SM3[] = {2,'b',0x13}; // BITE - S-Meter cal -100 dBm.
//unsigned char cmdK_SM4[] = {2,'b',0x14}; // BITE - S-Meter cal  -90 dBm.
//unsigned char cmdK_SM5[] = {2,'b',0x15}; // BITE - S-Meter cal  -80 dBm.
//unsigned char cmdK_SM6[] = {2,'b',0x16}; // BITE - S-Meter cal  -70 dBm.
//unsigned char cmdK_SM7[] = {2,'b',0x17}; // BITE - S-Meter cal  -60 dBm.
//unsigned char cmdK_SM8[] = {2,'b',0x18}; // BITE - S-Meter cal  -50 dBm.
//unsigned char cmdK_SM9[] = {2,'b',0x19}; // BITE - S-Meter cal  -40 dBm.
//unsigned char cmdK_SM10[] = {2,'b',0x1a}; // BITE - S-Meter cal  -30 dBm.
//unsigned char cmdK_SM11[] = {2,'b',0x1b}; // BITE - S-Meter cal  -20 dBm.
//unsigned char cmdK_SM12[] = {2,'b',0x1c}; // BITE - S-Meter cal  -10 dBm.
//unsigned char cmdK_SM13[] = {2,'b',0x1d}; // BITE - S-Meter cal 0 dBm.
//unsigned char cmdK_SM14[] = {2,'b',0x1e}; // BITE - S-Meter cal   10 dBm.
//unsigned char cmdK_SM15[] = {2,'b',0x1f}; // BITE - S-Meter cal   20 dBm.
//unsigned char cmdK_cmd1[] = {2,'b',0x20}; // Reserved.
//unsigned char cmdK_SAGC[] = {2,'b',0x21}; // BITE - Send AGC DVM value.
//unsigned char cmdK_cmd2[] = {2,'b',0x22}; // BITE - Send lock detector 1 DVM value.
//unsigned char cmdK_cmd3[] = {2,'b',0x23}; // BITE - Send lock detector 2 DVM value.
//unsigned char cmdK_Qfwd[] = {2,'b',0x24}; // BITE - Send forward power DVM value.
//unsigned char cmdK_Qref[] = {2,'b',0x25}; // BITE - Send reflected power DVM value.
//unsigned char cmdK_Qphs[] = {2,'b',0x26}; // BITE - Send phase detector DVM value.
//unsigned char cmdK_cmd7[] = {2,'b',0x27}; // BITE - Send Tx audio DVM value.
//unsigned char cmdK_Qt_A[] = {2,'b',0x28}; // BITE - Send temperature A DVM value.
//unsigned char cmdK_Qt_B[] = {2,'b',0x29}; // BITE - Send temperature B DVM value.
//unsigned char cmdK_cmdA[] = {2,'b',0x2a}; // BITE - Send R->T switching data.
//unsigned char cmdK_cmdB[] = {2,'b',0x2b}; // BITE - Send T->R switching data.
//unsigned char cmdK_cmdC[] = {2,'b',0x2c}; // BITE - Send synthesizer lock time 1 data.
//unsigned char cmdK_cmdD[] = {2,'b',0x2d}; // BITE - Send synthesizer lock time 2 data.
//unsigned char cmdK_cmdE[] = {2,'b',0x2e}; // BITE - Send ALC over-shoot data.
//unsigned char cmdK_cmdG[] = {2,'b',0x30}; // BITE - Reset on time value.
//unsigned char cmdK_cmdH[] = {2,'b',0x31}; // BITE - Request fault data.
//unsigned char cmdK_cmdI[] = {2,'b',0x32}; // BITE - Reset fault data.
//unsigned char cmdK_cmdJ[] = {2,'b',0x33}; // BITE - Set firmware serial number.
//unsigned char cmdK_cmdK[] = {2,'b',0x34}; // BITE - Perform powerup self test.
//unsigned char cmdK_cmdL[] = {2,'b',0x35}; // BITE - Set password.
//unsigned char cmdK_cmdQ[] = {2,'b',0x3a}; // BITE - Perform freq. ref. tilt

/*				
When the mode is commanded to AM, the radio will automatically
change the Bandpass Filter to 6 kHz. When the mode is commanded
from AM to USB or LSB, the radio will automatically change the
Bandpass Filter to 2.4 kHz. When the mode is commanded from AM to
CW, the radio will automatically change Bandpass Filter to 2.4 kHz
(CW filter default wide) or 500 Hz (CW filter default narrow).
If mode is changed to AM, squelch will be set to Level Sensitive.
If mode is changed from AM to CW, LSB, or USB, then squelch will be
restored to what it was before going to AM.

During transmit, a VFO transition from split to simplex will
not be allowed.

The command inhibit table given below shows the conditions
when commands are inhibited from being sent to the radio.

		TX = radio transmiting
		AM/FM = AM and FM modes
		CW = CW mode

Command	Definition				Inhibit Conditions
-------	------------------------------------------
   A	AGC speed					AM/FM
   a	Amplifier
   B	Bandpass Filter				AM/FM
   b	BITE  TX
   C	CW Offset freq.
   c	CW Filter default TX
   D	CW keyer dynamics
   d	NO-OP Keep Alive
   E	Tx equal. freq.
   e	Speech monitor
   F	VFO   TX
   G	Receive attenuator
   g	AGC knee					AM/FM
   H	Speech Compression
   I	IF shift freq.				AM/FM
   i	Impedance matching
   J	RIT (100Hz)
   j	RIT (10Hz)
   K	CW keyer mode
   k	CW spotting tone
   L	Level sens. squelch
   M	Mode  TX
   m	Mic gain
   N	Notch width					AM/FM
   n	Notch frequency				AM/FM
   O	Noise reduction				AM/FM
   o	Noise red. level			AM/FM
   P	Speech processor
   p	Pre-amplifier
   Q	Squelch
   q	CW keyer QSK
   R	Rx. frequency tune
   r	Reference Cal freq.			TX
   S	CW keyer speed
   s	CW keyer sidetone
   T	Tx. frequency tune			TX
   t	Tx. frequency save			TX
   U	Antenna tuning
   V	Audio volume
   v	CW Tx buffer				AM/FM
   W	Max power out
   w	CW keyer weight
   X	VOX level
   x	Push to talk				CW
   Y	AntiVOX level
   y	VOX Delay

TELEMETRY STRUCTURE (Radio to PC)
The radio will send telemetry to the PC automatically at a 50 msec rate.
The telemetry format will be a single byte using the following values.

Value     Definition
-----     ----------
0 - 127   Receive signal in dBm.

128       Squelch open (busy).
129       Squelch closed.

130 - 139 Automatic Level Control 0 - 20 (each increment is 2 ALC).

140 - 189 Forward power in percent, 0 - 100% (each increment is 2%).

190 - 214 Reflected power in percent, 0 - 50% (each increment is 2%).

215       Heat sink over-temperature alarm.
216       Loss of synthesizer lock.
217       Self test failure.

220 - 249 Heat sink temperature in degrees C, 17.5 - 90 degrees C,
          each increment is 2.5 degrees C.

253  Start of data transfer.

	Current radio settings data. If the DDS value for receive
	frequency is requested, the 4 byte DDS value will be sent,
	highest byte first, plus a 2 byte checksum. The uppermost
	2 bits of the high byte will indicate antenna port. See 'R'
	command section above for converting between DDS value and
	frequency. If mode is requested, a single byte identifying
	the mode will be sent without a checksum. See 'M' command
	section above for value description. If maximum power
	setting is requested, a single byte containing the power
	setting in watts, 1 - 100, will be sent without a checksum.

	Antenna impedance data. A total of 256 bytes of data will
	always be sent (64 points x 4 bytes) plus a 2 byte checksum.
	The 64 points will contain data for antenna port A and B.
	In each 4 byte group, bytes 0-1 will always contain the 2
	most significant bytes of DDS. The 2 uppermost bits of byte
	0 will indicate the format of bytes 2-3 and the antenna port
	as follows:
		bits   bytes 2-3	   port
		 00	rho/phi			 A
		 01	cap/ind	(network data)	 A
		 10	cap/ind	(network data)	 B
		 11	rho/phi			 B (no tuner)
	When bytes 2-3 contain rho/phi data, their values will
	be as follows:
		byte 2 Rho (increment is 1 / 256).
		byte 3 Phase angle (increment is 2pi * (1 / 256)).

	S Meter calibration table. 16 one byte values will always
	be sent plus a 2 byte checksum.

	Frequency reference calibration table. 32 one byte values
	will always be sent plus a 2 byte checksum.

	Phase detector calibration table. 16 one byte values will
	always be sent plus a 2 byte checksum.

	Carrier balance value. A 2 byte value will be sent without
	a checksum.

	Version data. 4 byte integer serial number, 2 byte hardware
	version number (MSB alpha, LSB digit), 2 byte firmware
	version number, and a 2 byte checksum.

	Hour meter data. 4 byte integer hours of operation will be
	sent plus a 2 byte checksum.

	Fault record data. 10 two byte numbers will always be sent
	plus a 2 byte checksum.

	R->T switching data. 50 1 byte forward power values in
	percent, 0 - 100% (each increment is 2%), plus a 2 byte
	checksum.

	T->R switching data. 50 1 byte values (DVM format) plus a
	2 byte checksum.

	Synthesizer lock time 1 data. 50 1 byte values (DVM format)
	plus a 2 byte checksum.

	Synthesizer lock time 2 data. 50 1 byte values (DVM format)
	plus a 2 byte checksum.

	ALC over-shoot data. 50 1 byte forward power values in
	percent, 0 - 100% (each increment is 2%), plus a 2 byte
	checksum.

	Digital Volt Meter data can be one of the following types:
	AGC, lock detector 1, lock detector 2, forward power,
	reflected power, phase detector, Tx audio, temperature A,
	temperature B, and TCXO. A 1 byte value (DVM format) will
	be sent without a checksum.

DVM format:
	Digital Volt Meter (DVM) data will be contained in a single byte
	representing the voltage. Positive values will range from 0 - 127, 128
	will be zero, and negative values will range from 129 to 255. Each
	increment in value will represent 1/16th of a volt or 0.0625 volts.

254  Error.
255  Good command acknowledge.

The PC will calculate the Voltage Standing Wave Ratio (VSWR) using the
following formula:

		RP = 100 * RP / FP
		RHO = SQRT (RP / FP)
		VSWR = (1 + RHO) / (1 - RHO)

	where   FP = forward power
		RP = reflected power
		RHO  = reflection coefficient

A high VSWR level will trigger a warning based on the following criteria:
		0.0 - 1.999 = normal (no warning)
		2.0 - 2.999 = caution (yellow warning)
		3.0 - above = alarm (red warning)

Data Transfer Specifications
		Baud Rate   9600
		Data Bits   8
		Stop Bits   1
		Parity   none
*/
