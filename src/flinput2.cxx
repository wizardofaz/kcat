// =====================================================================
//
// flinput2.cxx
//
// Author: Stelios Buononos, M0GLD
//         David Freese, W1HKJ (modified for kcat)
// Copyright: 2010
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  It is
// copyright under the GNU General Public License.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the Free Software
// Foundation, Inc.
// 59 Temple Place, Suite 330
// Boston, MA  02111-1307 USA
//
// =====================================================================

#include "config.h"

#include <string.h>

#include <cctype>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Tooltip.H>

#include "icons.h"
#include "flinput2.h"
#include "util.h"

enum { OP_UNDO, OP_CUT, OP_COPY, OP_PASTE, OP_DELETE, OP_CLEAR, OP_SELECT_ALL };

static Fl_Menu_Item cmenu[] = {
	{ make_icon_label("Undo", edit_undo_icon), 0, 0, 0, FL_MENU_DIVIDER, _FL_MULTI_LABEL },
	{ make_icon_label("Cut", edit_cut_icon), 0, 0, 0, 0, _FL_MULTI_LABEL },
	{ make_icon_label("Copy", edit_copy_icon), 0, 0, 0, 0, _FL_MULTI_LABEL },
	{ make_icon_label("Paste", edit_paste_icon), 0, 0, 0, 0, _FL_MULTI_LABEL },
	{ make_icon_label("Delete", trash_icon), 0, 0, 0, 0, _FL_MULTI_LABEL },
	{ make_icon_label("Clear", edit_clear_icon), 0, 0, 0, FL_MENU_DIVIDER, _FL_MULTI_LABEL },
	{ make_icon_label("Select All", edit_select_all_icon), 0, 0, 0, 0, _FL_MULTI_LABEL },
	{ 0 }
};
static bool cmenu_init = false;


Fl_Input2::Fl_Input2(int x, int y, int w, int h, const char* l)
	: Fl_Input(x, y, w, h, l)
{
	if (!cmenu_init) {
		for (size_t i = 0; i < sizeof(cmenu)/sizeof(*cmenu) - 1; i++)
			if (cmenu[i].labeltype() == _FL_MULTI_LABEL)
				set_icon_label(&cmenu[i]);
		cmenu_init = true;
	}
}

void Fl_Input2::add(const char *str)
{
	position(strlen(value()));
	insert(str);
}

int Fl_Input2::handle(int event)
{
	switch (event) {
	case FL_KEYBOARD: {
		int b = Fl::event_key();
		int p = position();
		// stop the move-to-next-field madness, we have Tab for that!
		if (unlikely((b == FL_Left && p == 0) || (b == FL_Right && p == size()) ||
			     (b == FL_Up && line_start(p) == 0) ||
			     (b == FL_Down && line_end(p) == size())))
			return 1;
		else if (unlikely(Fl::event_state() & (FL_ALT | FL_META))) {
			switch (b) {
			case 'c': { // capitalise
				if (readonly() || p == size())
					return 1;

				while (p < size() && isspace(*(value() + p)))
					p++;
				if (p == size())
					return 1;
				char c = toupper(*(value() + p));
				replace(p, p + 1, &c, 1);
				position(word_end(p));
			}
				return 1;
			case 'u': case 'l': { // upper/lower case
				if (readonly() || p == size())
					return 1;
				while (p < size() && isspace(*(value() + p)))
					p++;
				int n = word_end(p) - p;
				if (n == 0)
					return 1;

				char* s = new char[n];
				memcpy(s, value() + p, n);
				if (b == 'u')
					for (int i = 0; i < n; i++)
						s[i] = toupper(s[i]);
				else
					for (int i = 0; i < n; i++)
						s[i] = tolower(s[i]);
				replace(p, p + n, s, n);
				position(p + n);
				delete [] s;
				return 1;
			}
			default:
				break;
			}
		}
	}
		return Fl_Input::handle(event);
	case FL_MOUSEWHEEL: {
		if (!((type() & (FL_MULTILINE_INPUT | FL_MULTILINE_OUTPUT)) && Fl::event_inside(this)))
			return Fl_Input::handle(event);
		int d;
		if (!((d = Fl::event_dy()) || (d = Fl::event_dx())))
			return Fl_Input::handle(event);
		if (Fl::focus() != this)
			take_focus();
		up_down_position(d + (d > 0 ? line_end(position()) : line_start(position())));
		return 1;
	}
	case FL_PUSH:
		if (Fl::event_button() == FL_RIGHT_MOUSE)
			break;
		// fall through
	default:
		return Fl_Input::handle(event);
	}

	bool sel = position() != mark(), ro = readonly();
	set_active(&cmenu[OP_UNDO], !ro);
	set_active(&cmenu[OP_CUT], !ro && sel);
	set_active(&cmenu[OP_COPY], sel);
	set_active(&cmenu[OP_PASTE], !ro);
	set_active(&cmenu[OP_DELETE], !ro && sel);
	set_active(&cmenu[OP_CLEAR], !ro && size());
	set_active(&cmenu[OP_SELECT_ALL], size());

	take_focus();
	window()->cursor(FL_CURSOR_DEFAULT);
	int t = Fl_Tooltip::enabled();
	Fl_Tooltip::disable();
	const Fl_Menu_Item* m = cmenu->popup(Fl::event_x(), Fl::event_y());
	Fl_Tooltip::enable(t);

	if (!m)
		return 1;
	switch (m - cmenu) {
	case OP_UNDO:
		undo();
		break;
	case OP_CUT:
		cut();
		copy_cuts();
		break;
	case OP_COPY:
		copy(1);
		break;
	case OP_PASTE:
		Fl::paste(*this, 1);
		break;
	case OP_DELETE:
		cut();
		break;
	case OP_CLEAR:
		cut(0, size());
		break;
	case OP_SELECT_ALL:
		position(0, size());
		break;
	}

	return 1;
}
