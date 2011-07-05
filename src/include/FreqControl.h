// "$Id: FreqControl.cpp,v  2006/02/26"
//
// Frequency Control Widget for the Fast Light Tool Kit (Fltk)
//
// Copyright 2005-2006, Dave Freese W1HKJ
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "w1hkj@w1hkj.com".
//
// Usage:
//	Create a multi-digit receiver / transceiver frequency control widget
//
// label used to pass # digits & decimal position to control
// the widget can be used in Fluid & initialized with the
// number of digits as the label string
// default is 7; min number is 1, max number is 9 as in
//
// cFreqControl myFreqConrol(x0, y0, w0, h0, "N");  where N is # digits
// cFreqControl *pMyFreqControl = new cFreqControl(x0,y0,w0,h0,"N");

#ifndef _FREQCONTROL_H_
#define _FREQCONTROL_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Enumerations.H>

#ifdef MAX_DIGITS
#undef MAX_DIGITS
#endif
#define MAX_DIGITS 10

#ifdef MIN_DIGITS
#undef MIN_DIGITS
#endif
#define MIN_DIGITS 4

class Fl_Box;
class Fl_Float_Input;

class cFreqControl : public Fl_Group {
friend void cbSelectDigit (Fl_Widget *btn, void * nbr);
public:
	cFreqControl(int x, int y, int w, int h, const char *lbl = "7");
	~cFreqControl();
	void updatevalue();
	void value(long lv);
	long value(){return val;};
	void font(Fl_Font fnt);
	void SetONCOLOR (uchar r, uchar g, uchar b);
	void SetOFFCOLOR (uchar r, uchar g, uchar b);
	void GetONCOLOR (uchar &r, uchar &g, uchar &b) {
			Fl::get_color(ONCOLOR, r, g, b);
	};
	void GetOFFCOLOR (uchar &r, uchar &g, uchar &b) {
			Fl::get_color(OFFCOLOR, r, g, b);
	};
	void SetONOFFCOLOR( Fl_Color, Fl_Color);
	void setCallBack (int (*cbf)() ){ cbFunc = cbf;};
	void do_callback() { if (cbFunc) cbFunc(); }
	int  handle(int event);
	void visual_beep();

	void set_precision(int val) {
		if (val % 10) val = 1;
		precision = (val < 1) ? 1 : (val > 1000) ? 1000 : val;
	}

private:
	Fl_Repeat_Button	  	*Digit[MAX_DIGITS];
	Fl_Float_Input			*finp;
	static const char	 	*Label[];
	int					mult[MAX_DIGITS];
	Fl_Box				*decbx;
	Fl_Font  font_number;
	Fl_Color OFFCOLOR;
	Fl_Color ONCOLOR;
	Fl_Color SELCOLOR;
	Fl_Color ILLUMCOLOR;
	int nD;
	int active;
	long maxVal;
	long minVal;
	void DecFreq(int n);
	void IncFreq(int n);
	int (*cbFunc)();
	static void freq_input_cb(Fl_Widget* input, void* arg);
protected:
	long val, oldval;
	int  precision;
};

#endif 
