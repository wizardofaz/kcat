#ifndef _KACHINA_IO_H
#define _KACHINA_IO_H

#include "cstack.h"

extern bool serial_busy;

extern bool startComms(char *, int);

extern bool sendCommand(char *str);
extern bool RequestData (char *cmd, unsigned char *buff, int nbr);

extern bool setXcvrMode(int mode);
extern bool setXcvrRcvFreq(long freq, int offset);
extern bool setXcvrXmtFreq(long freq, int offset);
extern bool setXcvrSplitFreq(long freq, int offset);
extern bool setXcvrBW(int sel);
extern bool setXcvrIFshift(double val);
extern bool setXcvrVolume(double val);
extern bool setXcvrPower(double val);
extern bool setXcvrNotch(double val);
extern bool setXcvrNotchDepth(double val);
extern bool setXcvrNotchWidth(int val);
extern bool setXcvrMicGain(double val);
extern bool setXcvrAttControl(int val);
extern bool setXcvrPreamp(int val);
extern bool setXcvrNR(int val);
extern bool setXcvrNRlevel(double nr);
extern bool setXcvrTune(int val);
extern bool setXcvrPTT(int val);
extern bool setXcvrRITfreq(double rit);
extern bool setXcvrSimplex();
extern bool setXcvrSplit();
extern bool setXcvrListenOnReceive();
extern bool setXcvrCarrier(int val);

extern bool setXcvrWPM(double wpm);
extern bool setXcvrCWMON(double val);

extern bool setXcvrNOOP();

extern void initXcvrState();

extern cStack commstack;
#endif
