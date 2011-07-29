// Class definition for Log/Log plot
// db vs freq

#include "XYplot.h"

void XYplot::draw()
{
	Fl_Color clr;
	fl_clip(x(),y(),w(),h());
	fl_color(bckgnd_clr);
	fl_rectf(x(),y(),w(),h());
	fl_push_matrix();
// set up translate & scale for 0,0 at lower left corner of area
	fl_translate(x(),y() + h());

	double xscale = 1.0*w()/(xmax - xmin);
	double yscale = -1.0*h()/(ymax - ymin);
	fl_scale( xscale, yscale);

// draw points
	clr = FL_WHITE;
	fl_color(clr);
	for (vector<point>::iterator p = points.begin(); p != points.end(); p++) {
		if (p->clr != clr) {
			clr = p->clr;
			fl_color( clr );
		}
		fl_point(p->x - xmin, p->y - ymin);
	}

// draw axis
	clr = FL_GRAY;
	fl_color(clr);
	for (vector<line>::iterator p = axis.begin(); p != axis.end(); p++) {
		if (p->clr != clr) {
			clr = p->clr;
			fl_color( clr );
		}
		fl_begin_line();
		fl_vertex( p->x1 - xmin, p->y1 - ymin );
		fl_vertex( p->x2 - xmin, p->y2 - ymin);
		fl_end_line();
	}

// draw lines
	clr = FL_WHITE;
	fl_color(clr);
	for (vector<line>::iterator p = lines.begin(); p != lines.end(); p++) {
		if (p->clr != clr) {
			clr = p->clr;
			fl_color( clr );
		}
		fl_begin_line();
		fl_vertex( p->x1 - xmin, p->y1 - ymin );
		fl_vertex( p->x2 - xmin, p->y2 - ymin);
		fl_end_line();
	}
	fl_pop_matrix();
	fl_pop_clip();
}
