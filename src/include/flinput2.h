#ifndef FL_INPUT2_
#define FL_INPUT2_

#include <FL/Fl_Input.H>
#include <stdio.h>

class Fl_Input2 : public Fl_Input
{
public:
	Fl_Input2(int x, int y, int w, int h, const char* l = 0);
	int handle(int event);
	void add(const char *);
	void setCallback (void (*cbf)(int) ){ 
		cbFunc = cbf;
	}
	void do_callback(int b) { if (cbFunc) cbFunc(b); }
private:
	void (*cbFunc)(int);
};

#endif // FL_INPUT2_
