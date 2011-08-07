#ifndef _status_H
#define _status_H

#include <string>
#include <FL/Fl.H>
#include <FL/Enumerations.H>

using namespace std;

// kcat parameters & state variables
struct XCVRSTATE {
	int		VFO; // SIMPLEX
	int		NOTCHWIDTH;
	int		ATTEN;
	int		ANTIVOX;
	int		VOXDELAY; //5

	int		QSK;
	int		CWATTACK;
	int		CWWEIGHT;
	int		CWMODE; // left handed = 0x01
	int		CWOFFSET; // 700 Hz
	int		CWFILTER; // narrow

	int		SQUELCH; // level sensitive
	int		AMP; // off
	int		SPEECHPROC; // off
	int		SPEECHCOMP; // min
	int		ANTTUNE; // on 15

	int		EQUALIZER; // flat
	int		PREAMP; // off
	int		SQLEVEL; //20

	int		SPCHMONITOR; // off
	int		NR; // off
	int		AGCSPEED; // fast
	int		AGCACTION; //
	int		RIT; //25

	int		VOXLEVEL;

	double	VOL;
	double	NOTCHFREQ;
	double	MAXPWR;
	double	MICGAIN;
	double	CWMON;
	double	CWSPEED;

	double	NR_LEVEL; // min
	double	RITFREQ;
	double	IFSHIFT;
	double	NOTCHDEPTH;

	int		mainX;
	int		mainY;

	int		vfoA_freq;
	int		vfoA_imode;
	int		vfoA_iBW;

	int		vfoB_freq;
	int		vfoB_imode;
	int		vfoB_iBW;

	double	VFOADJ;
	double	VFO_OFFSET;

	int		swrRed;
	int		swrGreen;
	int		swrBlue;

	int		pwrRed;
	int		pwrGreen;
	int		pwrBlue;

	int		smeterRed;
	int		smeterGreen;
	int		smeterBlue;

	int		peakRed;
	int		peakGreen;
	int		peakBlue;

	int		fg_red;
	int		fg_green;
	int		fg_blue;

	int		bg_red;
	int		bg_green;
	int		bg_blue;

	int		fg_sys_red;
	int		fg_sys_green;
	int		fg_sys_blue;

	int		bg_sys_red;
	int		bg_sys_green;
	int		bg_sys_blue;

	int		bg2_sys_red;
	int		bg2_sys_green;
	int		bg2_sys_blue;

	int		slider_red;
	int		slider_green;
	int		slider_blue;

	int		slider_btn_red;
	int		slider_btn_green;
	int		slider_btn_blue;

	int		lighted_btn_red;
	int		lighted_btn_green;
	int		lighted_btn_blue;

	Fl_Font fontnbr;

	string	ui_scheme;

	string	ttyport;

	string	server_port;
	string	server_address;

	bool tooltips;

	Fl_Color bgclr;
	Fl_Color fgclr;

	void saveLastState();
	void loadLastState();
};

extern struct XCVRSTATE xcvrState;


#endif
