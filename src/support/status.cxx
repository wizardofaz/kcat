/* -----------------------------------------------------------------------------
 * status structure / methods
 *
 * A part of "rig", a rig control program compatible with fldigi / xmlrpc i/o
 *
 * copyright Dave Freese 2009, w1hkj@w1hkj.com
 *
*/

#include <iostream>
#include <fstream>
#include <string>

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>

#include "kcat.h"
#include "IOspec.h"
#include "test.h"
#include "support.h"
#include "cstack.h"
#include "kcat_io.h"
#include "config.h"
#include "status.h"

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
	4, // CWOFFSET = 700 Hz
	1, // CWFILTER

	0, // SQUELCH
	0, // AMP
	0, // SPEECHPROC
	0, // SPEECHCOMP
	1, // ANTTUNE
	0, // autotune

	0, // EQUALIZER
	0, // PREAMP
	-127, //SQLEVEL

	0, // SPCHMONITOR
	0, // NR
	128, // AGCSPEED
	128, // AGCACTION
	0, // RIT 

	0, // VOXLEVEL

	0.4, // VOL
	200.0, // NOTCHFREQ
	20.0, // MAXPWR
	0.5, // MICGAIN
	0.05, //CWMON 
	20.0, // CWSPEED
	false, // bool	FARNSWORTH;
	15.0, // double	FARNSWORTH_WPM;

	0.0, // NR_LEVEL
	0.0, // RITFREQ
	0.0, // IFSHIFT
	0.0,  // NOTCH DEPTH

	0,		// MAIN_X
	0,		// MAIN_Y

	14070000,	// int	vfoA_freq;
	1,			// int	vfoA_imode;
	7,			// int	vfoA_iBW;

	7035000,	// int	vfoB_freq;
	1,			// int	vfoB_imode;
	7,			// int	vfoB_iBW;

	0.0,		// double VFOADJ
	0.0,		// double VFO_OFFSET

	148,		// int	swrRed;
	0,			// int	swrGreen;
	148,		// int	swrBlue;

	180,		// int	pwrRed;
	0,			// int	pwrGreen;
	0,			// int	pwrBlue;

	0,			// int	smeterRed;
	180,		// int	smeterGreen;
	0,			// int	smeterBlue;

	255,		// int	peakRed;
	0,			// int	peakGreen;
	0,			// int	peakBlue;

	0,			// int	 fg_red;
	0,			// int	 fg_green;
	0,			// int	 fg_blue;

	232,		// int	 bg_red;
	255,		// int	 bg_green;
	232,		// int	 bg_blue;

	0,			// int	fg_sys_red;
	0,			// int	fg_sys_green;
	0,			// int	fg_sys_blue;

	192,		// int	bg_sys_red;
	192,		// int	bg_sys_green;
	192,		// int	bg_sys_blue;

	255,		// int	bg2_sys_red;
	255,		// int	bg2_sys_green;
	255,		// int	bg2_sys_blue;

	232,		// int	slider_red;
	255,		// int	slider_green;
	232,		// int	slider_blue;

	0,			// int	slider_btn_red;
	0,			// int	slider_btn_green;
	128,		// int	slider_btn_blue;

	255,		// int	lighted_btn_red;
	255,		// int	lighted_btn_green;
	0,			// int	lighted_btn_blue;

	FL_COURIER,	// ARIAL Fl_font fontnbr;

	"gtk+",		// string	ui_scheme;

	"",			// string ttyport

	"7362",		// string server_port
	"127.0.0.1",// string server_address

	true,		// bool tooltips

	(Fl_Color)0,			// Fl_Color bgclr;
	(Fl_Color)0,			// Fl_Color fgclr;

// message store
	"CQ",							// string	label_1;
	"CQ CQ CQ DE <CLL> <CLL> K ",	// string	edit_msg1;
	"call",							// string	label_2;
	"<STA> DE <CLL> <CLL> K",		// string	edit_msg2;
	"m 3",							// string	label_3;
	"",								// string	edit_msg3;
	"m 4",							// string	label_4;
	"",								// string	edit_msg4;
	"Xout",							// string	label_5;
	"R <#> <X>",					// string	edit_msg5;
	"Xlog",							// string	label_6;
	"<LOG><+>",						// string	edit_msg6;
	"X--",							// string	label_7;
	"<->",							// string	edit_msg7;
	"X++",							// string	label_8;
	"<+>",							// string	edit_msg8;
	"LOG",							// string	label_9;
	"<LOG>",						// string	edit_msg9;
	"m 10",							// string	label_10;
	"",								// string	edit_msg10;
	"m 11",							// string	label_11;
	"",								// string	edit_msg11;
	"m 12",							// string	label_12;
	"",								// string	edit_msg12;

	"",								//string	tag_call;
	"",								//string	tag_qth;
	"",								//string	tag_loc;
	"",								//string	tag_op;

	false,		// bool		xml_logbook

	1,			// int	serial_nbr;
	0,			// int	time_span;
	1,			// bool	band;
	true,		// bool	zeros;
	false,		// bool	dups;
	""			// string xout
};

void XCVRSTATE::saveLastState()
{
	Fl_Preferences spref(homedir.c_str(), "w1hkj.com", PACKAGE_NAME);

	mainX = window->x();
	mainY = window->y();

	vfoA_freq  = vfoA.freq;
	vfoA_iBW   = vfoA.iBW;
	vfoA_imode = vfoA.imode;

	vfoB_freq  = vfoB.freq;
	vfoB_iBW   = vfoB.iBW;
	vfoB_imode = vfoB.imode;

	spref.set("version", PACKAGE_VERSION);
	spref.set("mainx", mainX);
	spref.set("mainy", mainY);

	spref.set("fg_red", fg_red);
	spref.set("fg_green", fg_green);
	spref.set("fg_blue", fg_blue);

	spref.set("bg_red", bg_red);
	spref.set("bg_green", bg_green);
	spref.set("bg_blue", bg_blue);

	spref.set("smeter_red", smeterRed);
	spref.set("smeter_green", smeterGreen);
	spref.set("smeter_blue", smeterBlue);

	spref.set("power_red", pwrRed);
	spref.set("power_green", pwrGreen);
	spref.set("power_blue", pwrBlue);

	spref.set("swr_red", swrRed);
	spref.set("swr_green", swrGreen);
	spref.set("swr_blue", swrBlue);

	spref.set("peak_red", peakRed);
	spref.set("peak_green", peakGreen);
	spref.set("peak_blue", peakBlue);

	spref.set("fg_sys_red", fg_sys_red);
	spref.set("fg_sys_green", fg_sys_green);
	spref.set("fg_sys_blue", fg_sys_blue);

	spref.set("bg_sys_red", bg_sys_red);
	spref.set("bg_sys_green", bg_sys_green);
	spref.set("bg_sys_blue", bg_sys_blue);

	spref.set("bg2_sys_red", bg2_sys_red);
	spref.set("bg2_sys_green", bg2_sys_green);
	spref.set("bg2_sys_blue", bg2_sys_blue);

	spref.set("slider_red", slider_red);
	spref.set("slider_green", slider_green);
	spref.set("slider_blue", slider_blue);

	spref.set("slider_btn_red", slider_btn_red);
	spref.set("slider_btn_green", slider_btn_green);
	spref.set("slider_btn_blue", slider_btn_blue);

	spref.set("lighted_btn_red", lighted_btn_red);
	spref.set("lighted_btn_green", lighted_btn_green);
	spref.set("lighted_btn_blue", lighted_btn_blue);

	spref.set("fontnbr", fontnbr);

//	spref.set("tooltips", tooltips);

	spref.set("ui_scheme", ui_scheme.c_str());
	spref.set("vfo", VFO);
	spref.set("notchwidth", NOTCHWIDTH);
	spref.set("atten", ATTEN);
	spref.set("antivox", ANTIVOX);
	spref.set("voxdelay", VOXDELAY);

	spref.set("qsk", QSK);
	spref.set("cwattack", CWATTACK);
	spref.set("cwweight", CWWEIGHT);
	spref.set("cwmode", CWMODE);
	spref.set("cwoffset", CWOFFSET);
	spref.set("cwfilter", CWFILTER);

	spref.set("squelch", SQUELCH);
	spref.set("amp", AMP);
	spref.set("speecproc", SPEECHPROC);
	spref.set("speechcomp", SPEECHCOMP);
	spref.set("anttune", ANTTUNE);
	spref.set("autotune", autotune);

	spref.set("equalizer", EQUALIZER);
	spref.set("preamp", PREAMP);
	spref.set("sqlevel", SQLEVEL);

	spref.set("spchmonitor", SPCHMONITOR);
	spref.set("nr", NR);
	spref.set("agcspeed", AGCSPEED);
	spref.set("agcaction", AGCACTION);
	spref.set("rit", RIT);

	spref.set("voxlevel", VOXLEVEL);

	spref.set("vol", VOL);
	spref.set("notchfreq", NOTCHFREQ);
	spref.set("maxpwr", MAXPWR);
	spref.set("micgain", MICGAIN);
	spref.set("cwmon", CWMON);
	spref.set("cwspeed", CWSPEED);
	spref.set("farnsworth", FARNSWORTH);
	spref.set("farnsworth_wpm", FARNSWORTH_WPM);

	spref.set("nr_level", NR_LEVEL);
	spref.set("ritfreq", RITFREQ);
	spref.set("ifshift", IFSHIFT);
	spref.set("notchdepth", NOTCHDEPTH);

	spref.set("vfoA_freq", vfoA_freq);
	spref.set("vfoA_imode", vfoA_imode);
	spref.set("vfoA_iBW", vfoA_iBW);

	spref.set("vfoB_freq", vfoB_freq);
	spref.set("vfoB_imode", vfoB_imode);
	spref.set("vfoB_iBW", vfoB_iBW);

	spref.set("vfoadj", VFOADJ);
	spref.set("vfo_offset", VFO_OFFSET);

	spref.set("ttyport", ttyport.c_str());
	spref.set("server_port", server_port.c_str());
	spref.set("server_addr", server_address.c_str());

	spref.set("tooltips", tooltips);

	spref.set("label1", label_1.c_str()); 
	spref.set("msg1", edit_msg1.c_str());
	spref.set("label2", label_2.c_str()); 
	spref.set("msg2", edit_msg2.c_str());
	spref.set("label3", label_3.c_str()); 
	spref.set("msg3", edit_msg3.c_str());
	spref.set("label4", label_4.c_str()); 
	spref.set("msg4", edit_msg4.c_str());
	spref.set("label5", label_5.c_str()); 
	spref.set("msg5", edit_msg5.c_str());
	spref.set("label6", label_6.c_str()); 
	spref.set("msg6", edit_msg6.c_str());
	spref.set("label7", label_7.c_str()); 
	spref.set("msg7", edit_msg7.c_str());
	spref.set("label8", label_8.c_str()); 
	spref.set("msg8", edit_msg8.c_str());
	spref.set("label9", label_9.c_str()); 
	spref.set("msg9", edit_msg9.c_str());
	spref.set("label10", label_10.c_str()); 
	spref.set("msg10", edit_msg10.c_str());
	spref.set("label11", label_11.c_str()); 
	spref.set("msg11", edit_msg11.c_str());
	spref.set("label12", label_12.c_str()); 
	spref.set("msg12", edit_msg12.c_str());

	spref.set("tag_cll", tag_cll.c_str());
	spref.set("tag_qth", tag_qth.c_str());
	spref.set("tag_loc", tag_loc.c_str());
	spref.set("tag_opr", tag_opr.c_str());

	spref.set("xml_logbook", xml_logbook);

	spref.set("logbook_ser_nbr", serial_nbr);
	spref.set("logbook_time_span", time_span);
	spref.set("logbook_zeros", zeros);
	spref.set("logbook_dups", dups);
	spref.set("logbook_band", band);
	spref.set("logbook_xout", xout.c_str());

	std::string fname = homedir;
	fname.append("kcat.ant");

	ofstream outANT(fname.c_str());
	if (outANT) {
		for (int i = 0; i < numantports; i++) {
			outANT << antports[i].freq << " ";
			outANT << antports[i].rcv << " ";
			outANT << antports[i].xmt << endl;
		}
		outANT.close();
	}
}

void XCVRSTATE::loadLastState()
{
	Fl_Preferences spref(homedir.c_str(), "w1hkj.com", PACKAGE_NAME);

	if (spref.entryExists("version")) {

		char defbuffer[200];
		int i = 0;

		spref.get("mainx", mainX, mainX);
		spref.get("mainy", mainY, mainY);

		spref.get("fg_red", fg_red, fg_red);
		spref.get("fg_green", fg_green, fg_green);
		spref.get("fg_blue", fg_blue, fg_blue);

		spref.get("bg_red", bg_red, bg_red);
		spref.get("bg_green", bg_green, bg_green);
		spref.get("bg_blue", bg_blue, bg_blue);

		spref.get("smeter_red", smeterRed, smeterRed);
		spref.get("smeter_green", smeterGreen, smeterGreen);
		spref.get("smeter_blue", smeterBlue, smeterBlue);

		spref.get("power_red", pwrRed, pwrRed);
		spref.get("power_green", pwrGreen, pwrGreen);
		spref.get("power_blue", pwrBlue, pwrBlue);

		spref.get("swr_red", swrRed, swrRed);
		spref.get("swr_green", swrGreen, swrGreen);
		spref.get("swr_blue", swrBlue, swrBlue);

		spref.get("peak_red", peakRed, peakRed);
		spref.get("peak_green", peakGreen, peakGreen);
		spref.get("peak_blue", peakBlue, peakBlue);

		spref.get("fg_sys_red", fg_sys_red, fg_sys_red);
		spref.get("fg_sys_green", fg_sys_green, fg_sys_green);
		spref.get("fg_sys_blue", fg_sys_blue, fg_sys_blue);

		spref.get("bg_sys_red", bg_sys_red, bg_sys_red);
		spref.get("bg_sys_green", bg_sys_green, bg_sys_green);
		spref.get("bg_sys_blue", bg_sys_blue, bg_sys_blue);

		spref.get("bg2_sys_red", bg2_sys_red, bg2_sys_red);
		spref.get("bg2_sys_green", bg2_sys_green, bg2_sys_green);
		spref.get("bg2_sys_blue", bg2_sys_blue, bg2_sys_blue);

		spref.get("slider_red", slider_red, slider_red);
		spref.get("slider_green", slider_green, slider_green);
		spref.get("slider_blue", slider_blue, slider_blue);

		spref.get("slider_btn_red", slider_btn_red, slider_btn_red);
		spref.get("slider_btn_green", slider_btn_green, slider_btn_green);
		spref.get("slider_btn_blue", slider_btn_blue, slider_btn_blue);

		spref.get("lighted_btn_red", lighted_btn_red, lighted_btn_red);
		spref.get("lighted_btn_green", lighted_btn_green, lighted_btn_green);
		spref.get("lighted_btn_blue", lighted_btn_blue, lighted_btn_blue);

		i = (int)fontnbr;
		spref.get("fontnbr", i, i); fontnbr = (Fl_Font)i;
//		i = 0;
//		if (spref.get("tooltips", i, i)) tooltips = i;

		spref.get("ui_scheme", defbuffer, "gtk+", 199);
		ui_scheme = defbuffer;

		spref.get("vfo", VFO, VFO);
		spref.get("notchwidth", NOTCHWIDTH, NOTCHWIDTH);
		spref.get("atten", ATTEN, ATTEN);
		spref.get("antivox", ANTIVOX, ANTIVOX);
		spref.get("voxdelay", VOXDELAY, VOXDELAY);

		spref.get("qsk", QSK, QSK);
		spref.get("cwattack", CWATTACK, CWATTACK);
		spref.get("cwweight", CWWEIGHT, CWWEIGHT);
		spref.get("cwmode", CWMODE, CWMODE);
		spref.get("cwoffset", CWOFFSET, CWOFFSET);
		spref.get("cwfilter", CWFILTER, CWFILTER);

		spref.get("squelch", SQUELCH, SQUELCH);
		spref.get("amp", AMP, AMP);
		spref.get("speecproc", SPEECHPROC, SPEECHPROC);
		spref.get("speechcomp", SPEECHCOMP, SPEECHCOMP);
		spref.get("anttune", ANTTUNE, ANTTUNE);
		spref.get("autotune", autotune, autotune);

		spref.get("equalizer", EQUALIZER, EQUALIZER);
		spref.get("preamp", PREAMP, PREAMP);
		spref.get("sqlevel", SQLEVEL, SQLEVEL);

		spref.get("spchmonitor", SPCHMONITOR, SPCHMONITOR);
		spref.get("nr", NR, NR);
		spref.get("agcspeed", AGCSPEED, AGCSPEED);
		spref.get("agcaction", AGCACTION, AGCACTION);
		spref.get("rit", RIT, RIT);

		spref.get("voxlevel", VOXLEVEL, VOXLEVEL);

		spref.get("vol", VOL, VOL);
		spref.get("notchfreq", NOTCHFREQ, NOTCHFREQ);
		spref.get("maxpwr", MAXPWR, MAXPWR);
		spref.get("micgain", MICGAIN, MICGAIN);
		spref.get("cwmon", CWMON, CWMON);
		spref.get("cwspeed", CWSPEED, CWSPEED);
		spref.get("farnsworth", i, FARNSWORTH); FARNSWORTH = i;
		spref.get("farnsworth_wpm", FARNSWORTH_WPM, FARNSWORTH_WPM);

		spref.get("nr_level", NR_LEVEL, NR_LEVEL);
		spref.get("ritfreq", RITFREQ, RITFREQ);
		spref.get("ifshift", IFSHIFT, IFSHIFT);
		spref.get("notchdepth", NOTCHDEPTH, NOTCHDEPTH);

		spref.get("vfoA_freq", vfoA_freq, vfoA_freq);
		spref.get("vfoA_imode", vfoA_imode, vfoA_imode);
		spref.get("vfoA_iBW", vfoA_iBW, vfoA_iBW);

		spref.get("vfoB_freq", vfoB_freq, vfoB_freq);
		spref.get("vfoB_imode", vfoB_imode, vfoB_imode);
		spref.get("vfoB_iBW", vfoB_iBW, vfoB_iBW);

		spref.get("vfoadj", VFOADJ, VFOADJ);
		spref.get("vfo_offset", VFO_OFFSET, VFO_OFFSET);

		spref.get("ttyport", defbuffer, "", 199);
		ttyport = defbuffer;

		spref.get("server_port", defbuffer, "7362", 199);
		server_port = defbuffer;
		spref.get("server_addr", defbuffer, "127.0.0.1", 199);
		server_address = defbuffer;

		i = 0;
		if (spref.get("tooltips", i, i)) tooltips = i;

		spref.get("label1", defbuffer, label_1.c_str(), 199);
			label_1 = defbuffer;
		spref.get("msg1", defbuffer, edit_msg1.c_str(), 199);
			edit_msg1 = defbuffer;
		spref.get("label2", defbuffer, label_2.c_str(), 199);
			label_2 = defbuffer;
		spref.get("msg2", defbuffer, edit_msg2.c_str(), 199);
			edit_msg2 = defbuffer;
		spref.get("label3", defbuffer, label_3.c_str(), 199);
			label_3 = defbuffer;
		spref.get("msg3", defbuffer, edit_msg3.c_str(), 199);
			edit_msg3 = defbuffer;
		spref.get("label4", defbuffer, label_4.c_str(), 199);
			label_4 = defbuffer;
		spref.get("msg4", defbuffer, edit_msg4.c_str(), 199);
			edit_msg4 = defbuffer;
		spref.get("label5", defbuffer, label_5.c_str(), 199);
			label_5 = defbuffer;
		spref.get("msg5", defbuffer, edit_msg5.c_str(), 199);
			edit_msg5 = defbuffer;
		spref.get("label6", defbuffer, label_6.c_str(), 199);
			label_6 = defbuffer;
		spref.get("msg6", defbuffer, edit_msg6.c_str(), 199);
			edit_msg6 = defbuffer;
		spref.get("label7", defbuffer, label_7.c_str(), 199);
			label_7 = defbuffer;
		spref.get("msg7", defbuffer, edit_msg7.c_str(), 199);
			edit_msg7 = defbuffer;
		spref.get("label8", defbuffer, label_8.c_str(), 199);
			label_8 = defbuffer;
		spref.get("msg8", defbuffer, edit_msg8.c_str(), 199);
			edit_msg8 = defbuffer;
		spref.get("label9", defbuffer, label_9.c_str(), 199);
			label_9 = defbuffer;
		spref.get("msg9", defbuffer, edit_msg9.c_str(), 199);
			edit_msg9 = defbuffer;
		spref.get("label10", defbuffer, label_10.c_str(), 199);
			label_10 = defbuffer;
		spref.get("msg10", defbuffer, edit_msg10.c_str(), 199);
			edit_msg10 = defbuffer;
		spref.get("label11", defbuffer, label_11.c_str(), 199);
			label_11 = defbuffer;
		spref.get("msg11", defbuffer, edit_msg11.c_str(), 199);
			edit_msg11 = defbuffer;
		spref.get("label12", defbuffer, label_12.c_str(), 199);
			label_12 = defbuffer;
		spref.get("msg12", defbuffer, edit_msg12.c_str(), 199);
			edit_msg12 = defbuffer;

		spref.get("tag_cll", defbuffer, "", 199); tag_cll = defbuffer;
		spref.get("tag_qth", defbuffer, "", 199); tag_qth = defbuffer;
		spref.get("tag_loc", defbuffer, "", 199); tag_loc = defbuffer;
		spref.get("tag_opr", defbuffer, "", 199); tag_opr = defbuffer;

		spref.get("xml_logbook", i, i); xml_logbook = (i == 1);

		spref.get("logbook_ser_nbr", serial_nbr, serial_nbr);
		spref.get("logbook_time_span", time_span, time_span);
		spref.get("logbook_zeros", i, i); zeros = (i == 1);
		spref.get("logbook_dups", i, i); dups = (i == 1);
		spref.get("logbook_band", i, i); band = (i == 1);
		spref.get("logbook_xout", defbuffer, "", 199); xout = defbuffer;

	}

	std::string fname = homedir;
	int freq, rcv, xmt;
	fname.append("kcat.ant");
	ifstream inANT(fname.c_str());

	if (inANT) {
		int ports = 0;
		while (!inANT.eof()) {
			freq = 0;
			inANT >> freq >> rcv >> xmt;
			if (freq) {
				if (!ports) { ports = 1; numantports = 0;}
				antports[numantports].freq = freq;
				antports[numantports].rcv = rcv;
				antports[numantports].xmt = xmt;
				numantports++;
			}
		}
		inANT.close();
	}

	bgclr = fl_rgb_color(bg_red, bg_green, bg_blue);
	fgclr = fl_rgb_color(fg_red, fg_green, fg_blue);

	Fl::background( bg_sys_red, bg_sys_green, bg_sys_blue);
	Fl::background2( bg2_sys_red, bg2_sys_green, bg2_sys_blue);
	Fl::foreground( fg_sys_red, fg_sys_green, fg_sys_blue);

	Fl_Color btn_lt_color = fl_rgb_color( lighted_btn_red, lighted_btn_green, lighted_btn_blue);
	Fl_Color btn_slider = fl_rgb_color( slider_btn_red, slider_btn_green, slider_btn_blue);
	Fl_Color bg_slider = fl_rgb_color(slider_red, slider_green, slider_blue);

	FreqDisp->SetONOFFCOLOR( fgclr, bgclr);
	FreqDispB->SetONOFFCOLOR( fgclr, fl_color_average(bgclr, FL_BLACK, 0.87));

	FreqDisp->font(fontnbr);
	FreqDispB->font(fontnbr);

	grpMeters->color(bgclr);

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

	btnVol->selection_color(btn_lt_color);
	btnNR->selection_color(btn_lt_color);
	btnIFsh->selection_color(btn_lt_color);
	btnNotch->selection_color(btn_lt_color);
	btnAttenuator->selection_color(btn_lt_color);
	btnPreamp->selection_color(btn_lt_color);
	btnNR->selection_color(btn_lt_color);
	btnNotch->selection_color(btn_lt_color);
	btn_autotune->selection_color(btn_lt_color);
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

	Fl::scheme(ui_scheme.c_str());

	if (tooltips) {
		Fl_Tooltip::enable(1);
		mnuTooltips->set();
	} else {
		mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

}
