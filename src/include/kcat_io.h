#ifndef _KACHINA_IO_H
#define _KACHINA_IO_H

#include <string>
#include "cstack.h"

extern bool serial_busy;

extern std::string retval;

extern bool startComms(const char *, int);

extern bool sendCommand(char *str);
extern bool sendCmd(std::string &str);
extern bool RequestData (char *cmd, unsigned char *buff, int nbr);

extern void setXcvrMode(int mode);
extern void setXcvrRcvFreq(long freq, int offset);
extern void setXcvrXmtFreq(long freq, int offset);
extern void setXcvrSplitFreq(long freq, int offset);
extern void setXcvrBW(int sel);
extern void setXcvrIFshift(double val);
extern void setXcvrVolume(double val);
extern void setXcvrPower(double val);
extern void setXcvrNotch(double val);
extern void setXcvrNotchDepth(double val);
extern void setXcvrNotchWidth(int val);
extern void setXcvrMicGain(double val);
extern void setXcvrAttControl(int val);
extern void setXcvrPreamp(int val);
extern void setXcvrNR(int val);
extern void setXcvrNRlevel(double nr);
extern void setXcvrTune(int val);
extern void setXcvrPTT(int val);
extern void setXcvrRITfreq(double rit);
extern void setXcvrSimplex();
extern void setXcvrSplit();
extern void setXcvrListenOnReceive();
extern void setXcvrCarrier(int val);
extern void setXcvrSPOT(int val);
extern void setXcvrEqualizer(int val);
extern void setXcvrSpeechProcessor(int val);
extern void setXcvrCompression(int val);
extern void setXcvrSpchMon(int val);
extern void setXcvrVoxOnOff(int val);
extern void setXcvrAntiVox(int val);
extern void setXcvrVoxDelay(int val);
extern void setXcvrAmpOnOff(int val);
extern void setXcvrCWattack(int val);
extern void setXcvrCWweight(int val);
extern void setXcvrCWmode(int val);
extern void setXcvrCWoffset(int val);
extern void setXcvrCWdefFilter(int val);
extern void setXcvrQSKonoff(int val);
extern void setXcvrAGCaction(int val);
extern void setXcvrAgcSpeed(int val);
extern void setXcvrSqlLevel(int val);
extern void setXcvrSqlType(int val);

extern void setXcvrWPM(double wpm);
extern void setXcvrCWMON(double val);

extern void setXcvrNOOP();

extern void initXcvrState();

extern cStack commstack;
#endif
