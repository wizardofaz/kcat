// SerialComm.h: interface for the CSerialComm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include <memory>

#define bzero(b,len) memset(b,0,(size_t)(len))

class CSerialComm  {
public:
	CSerialComm();
	virtual	~CSerialComm();

//Methods
	bool OpenPort(char * strPortName, int baud = B1200);
	bool IsOpen() { return fd < 0 ? 0 : 1; };
	bool IsBusy() { return busyflag; };
	void IsBusy(bool val) { busyflag = val; };
	void ClosePort();

	void Timeout(int tm) { timeout = tm; return; };
	int  Timeout() { return timeout; };
	
	int  ReadBuffer (unsigned char *b, int nbr);
	int  WriteBuffer(unsigned char *str, int nbr);
	void FlushBuffer();

	
private:
//Members
	char szPortName[12];
	int  fd;
	struct termios oldtio, newtio;
	int  timeout;
	bool busyflag;
	char bfr[2048];
//Methods
	bool IOselect();
};

#endif
