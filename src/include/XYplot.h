// XYplot.h
#ifndef XYplot_H
#define XYplot_H

#if defined(WIN32) && !defined(__CYGWIN__)
#	include <direct.h>
#else
#	include <unistd.h>
#endif
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/fl_draw.H>
#include <FL/math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class XYplot : public Fl_Widget {
public:
	struct point {double x; double y; Fl_Color clr;};
	struct line {double x1; double y1; double x2; double y2; Fl_Color clr;};

	XYplot(int X,int Y,int W,int H, const char *lbl="") : Fl_Widget(X,Y,W,H,lbl)
	{
		box(FL_FLAT_BOX);
		bckgnd_clr = FL_BLACK;
		scale_clr = FL_GRAY;
		xmin = 0.0; xmax = 1.0;
		ymin = 0.0; ymax = 1.0;
		clear();
	}
	virtual void draw();

	void color(Fl_Color clr) { bckgnd_clr = clr; }
	void scale_color(Fl_Color clr) { scale_clr = clr; }
	void clear_points() { points.clear(); }
	void clear_axis() { axis.clear(); }
	void clear_lines() { lines.clear(); }
	void clear() { points.clear(); lines.clear(); axis.clear(); }
	void add_point(point ctr) { points.push_back(ctr); }
	void add_line(line ln) { lines.push_back(ln); }
	void add_axis(line ln) { axis.push_back(ln); }
	void xmin_max(double min, double max) {xmin = min; xmax = max;}
	void ymin_max(double min, double max) {ymin = min; ymax = max;}
	void end() {};

protected:
	vector<point> points;
	vector<line> lines;
	vector<line> axis;
	Fl_Color bckgnd_clr;
	Fl_Color scale_clr;
	double xmin;
	double xmax;
	double ymin;
	double ymax;

	void draw_axis();
	void Plot_Fjw();
	void Plot_V();
};

#endif
