//======================================================================
// Class definition for General x-y plot fltk class
//======================================================================

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

// draw text
	clr = FL_WHITE;
	fl_color(clr);
	for (vector<text>::iterator p = texts.begin(); p != texts.end(); p++) {
		if (p->clr != clr) {
			clr = p->clr;
			fl_color( clr );
		}
		fl_draw(p->txt, p->x, p->y);
	}

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

void XYplot::xy_tooltip(void* obj)
//void xy_tooltip(void* obj)
{
	struct point {
		int x, y;
		bool operator==(const point& p) { return x == p.x && y == p.y; }
		bool operator!=(const point& p) { return !(*this == p); }
	};
	static point p[3] = { {0, 0}, {0, 0}, {0, 0} };

	XYplot* v = reinterpret_cast<XYplot*>(obj);

	memmove(p, p+1, 2 * sizeof(point));
	p[2].x = Fl::event_x(); p[2].y = Fl::event_y();

	if (p[2] == p[1] && p[2] != p[0]) {
		if (v->disp_tooltip) {
			Fl_Tooltip::enter_area(v, p[2].x, p[2].y, 100, 100, 
				v->disp_tooltip(v->xmin + (p[2].x - v->x())*(v->xmax - v->xmin)/v->w(), 
								v->ymin + (v->h() + v->y() - p[2].y)*(v->ymax - v->ymin)/v->h() ));
		} else {
			snprintf(v->tip, sizeof(v->tip), "x %d, y %d", p[2].x - v->x(), p[2].y - v->y());
			Fl_Tooltip::enter_area(v, p[2].x, p[2].y, 100, 100, v->tip);
		}
	} else if (p[2] != p[1])
		Fl_Tooltip::exit(v);

	Fl::repeat_timeout((double)Fl_Tooltip::hoverdelay(), xy_tooltip, obj);
}


int XYplot::handle(int event)
{
	switch (event) {
		case FL_KEYBOARD:
			return 0;
		case FL_PUSH:
			do_callback();
			return 1;
		case FL_ENTER:
			if (!show_xval) break;
			tooltip_delay = Fl_Tooltip::delay();
			Fl_Tooltip::enable(true);
			Fl_Tooltip::delay(0.0f);
			Fl::add_timeout(tooltip_delay / 2.0, xy_tooltip, this);
			break;
		case FL_LEAVE:
			if (!show_xval) break;
			Fl_Tooltip::enable(true);
			Fl_Tooltip::delay(tooltip_delay);
			Fl::remove_timeout(xy_tooltip, this);
			break;
	}
	return 1;
}

