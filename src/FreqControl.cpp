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

#include "FreqControl.h"
#include <iostream>

using namespace std;

const char *cFreqControl::Label[10] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	
void cFreqControl::IncFreq (int nbr) {
	if (!enabled) return;
	long v = 1;
	v = val + mult[nbr];
	if (v <= maxVal) val = v;
	updatevalue();
	if (cbFunc) (*cbFunc)();
}

void cFreqControl::DecFreq (int nbr) {
	if (!enabled) return;
	long v = 1;
	v = val - mult[nbr];
	if (v >= minVal)
 	  val = v;
	updatevalue();
	if (cbFunc) (*cbFunc)();
}

void cFreqControl::Illuminate (int nbr) {
	Digit[active]->labelcolor(ONCOLOR);
	Digit[active]->redraw();
	active = nbr;
	Digit[active]->labelcolor(SELCOLOR);
	Digit[active]->redraw();
}

void cFreqControl::RotR() {
	if (active == 0) Illuminate(nD - 1);
	else Illuminate(active - 1);
	Fl::focus(Digit[active]);
}

void cFreqControl::RotL() {
	if (active == (nD - 1)) Illuminate(0);
	else Illuminate(active + 1);
	Fl::focus(Digit[active]);
}

void cbSelectDigit (Fl_Widget *btn, void * nbr)
{
	Fl_Button *b = (Fl_Button *)btn;
	bool top = (Fl::event_y() < b->y() + b->h()/2);
	int Nbr = (int)(reinterpret_cast<long> (nbr));
	
	cFreqControl *fc = (cFreqControl *)b->parent();
	if (top)
		fc->IncFreq(Nbr);
	else
		fc->DecFreq(Nbr);
	fc->damage();
}

cFreqControl::cFreqControl(int x, int y, int w, int h, char *lbl):
			  Fl_Group(x,y,w,h,"") {
	ONCOLOR = FL_YELLOW;
	OFFCOLOR = FL_BLACK;
	SELCOLOR = fl_rgb_color(100, 100, 100);
	SELCOLOR = FL_DARK_GREEN;
	val = 0;
	nD = 8; // nD <= MAXDIGITS

	int pw = 6; // decimal width
	int fcWidth = (w - pw - 4)/nD;
	int fcFirst = x;
	int fcTop = y;
	int fcHeight = h;
	long int max;
	int xpos;

	box(FL_DOWN_BOX);
	max = 1;
	for (int n = 0; n < nD; n++) {
		xpos = fcFirst + (nD - 1 - n) * fcWidth + 2;
		if (n < 3) xpos += pw;
		Digit[n] = new Fl_Button (
			xpos,
			fcTop + 2,
			fcWidth,
			fcHeight-4,
			" ");
		Digit[n]->box(FL_FLAT_BOX);	
		Digit[n]->labelfont(FL_COURIER);
		if (n == 0)
			Digit[n]->labelcolor(SELCOLOR);
		else
			Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, OFFCOLOR);
		Digit[n]->labelsize(fcHeight);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		mult[n] = max;
		max *= 10;
	}
	decbx = new Fl_Box(fcFirst + (nD - 3) * fcWidth + 2, fcTop + 2, pw, fcHeight-4,".");
	decbx->box(FL_FLAT_BOX);
	decbx->labelfont(FL_COURIER);
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->labelsize(fcHeight);
	
	enabled = true;
	cbFunc = NULL;
	maxVal = max * 10 - 1;
	minVal = 0;
	Illuminate(3);
	end();
}

cFreqControl::~cFreqControl()
{
	for (int i = 0; i < nD; i++) {
		delete Digit[i];
	}
}


void cFreqControl::updatevalue()
{
	long v = val;
	for (int n = 0; n < nD; n++) {
		Digit[n]->label(v == 0 ? " " : Label[v % 10]);
		v /= 10;
	}
	redraw();
//	damage();
}

void cFreqControl::SetColors()
{
    for (int n = 0; n < nD; n++) {
    	if (n == active)
    		Digit[n]->labelcolor(SELCOLOR);
    	else
			Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
		Digit[n]->redraw();
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->redraw();
}

void cFreqControl::SetONOFFCOLOR( Fl_Color ONcolor, Fl_Color OFFcolor)
{
    OFFCOLOR = OFFcolor;
    ONCOLOR = ONcolor;
	SetColors();
}

void cFreqControl::SetONCOLOR (uchar r, uchar g, uchar b) 
{
	ONCOLOR = fl_rgb_color (r, g, b);
	SetColors();
}

void cFreqControl::SetOFFCOLOR (uchar r, uchar g, uchar b) 
{
	OFFCOLOR = fl_rgb_color (r, g, b);
	SetColors();
}

void cFreqControl::SetSELCOLOR (uchar r, uchar g, uchar b) 
{
	SELCOLOR = fl_rgb_color (r, g, b);
	SetColors();
}

void cFreqControl::value(long lv)
{
  val = lv;
  updatevalue();
}

int cFreqControl::handle (int event) {
	switch (event) {
	case FL_KEYBOARD:
		int key = Fl::event_key();
		if (key == FL_Left) {
			RotL();
			return 1;
		}
		if (key == FL_Right) {
			RotR();
			return 1;
		}
		if (key == FL_Up) {
			IncFreq(active);
			Fl::flush();
			return 1;
		}
		if (key == FL_Down) {
			DecFreq(active);
			Fl::flush();
			return 1;
		}
	}
	
	if (event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
		int xpress = Fl::event_x() ;
    	for (int n = 0; n < nD; n++) {
    		if (Digit[n]->x() < xpress) {
				bool top = (Fl::event_y() < Digit[n]->y() + Digit[n]->h()/2);
				if (top)
					IncFreq(n);
				else
					DecFreq(n);
				damage();
				if (active != n) 
					Illuminate(n);
				return 1;
    		}
    	}
	}
	if (event == FL_MOUSEWHEEL) {
		int xpress = Fl::event_x() ;
    	for (int n = 0; n < nD; n++) {
    		if (Digit[n]->x() < xpress) {
				if (Fl::e_dy < 0)
					IncFreq(n);
				else
					DecFreq(n);
				damage();
				if (active != n) 
					Illuminate(n);
				return 1;
    		}
    	}
	}
	
	return 0;
}


