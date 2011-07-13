// SerialComm.h: interface for the CSerialComm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SERIAL_H
#define SERIAL_H

#include <string>

using namespace std;

#ifndef __WIN32__

#include <termios.h>

class CSerialComm  {
public:
	CSerialComm();
	~CSerialComm();

//Methods
	bool OpenPort();
	bool CheckPort(string);

	bool IsOpen() { return fd < 0 ? 0 : 1; };
	void ClosePort();

	void Device (std::string dev) { device = dev;};
	std::string Device() { return device;};

	void Baud(int b) { baud = b;};
	int  Baud() { return baud;};

	void Timeout(int tm) { timeout = tm;}
	int  Timeout() { return timeout; }

	void Retries(int r) { retries = r;}
	int  Retries() { return retries;}

	void RTS(bool r){rts = r;}
	bool RTS(){return rts;}
	void setRTS(bool r);

	void RTSptt(bool b){rtsptt = b;}
	bool RTSptt(){return rtsptt;}

	void DTR(bool d){dtr = d;}
	bool DTR(){return dtr;}
	void setDTR(bool d);

	void DTRptt(bool b){dtrptt = b;}
	bool DTRptt(){return dtrptt;}

	void RTSCTS(bool b){rtscts = b;}
	bool RTSCTS(){return rtscts;}
	void SetPTT(bool b);

	void Stopbits(int n) {stopbits = (n == 1 ? 1 : 2);}
	int  Stopbits() { return stopbits;}

	int  ReadBuffer (char *b, int nbr);
	int  WriteBuffer(const char *str, int nbr);
	void FlushBuffer();


private:
//Members
	std::string	device;
	int		fd;
	int		baud;
	int		speed;
	struct	termios oldtio, newtio;
	int		timeout;
	int		retries;
	int		status, origstatus;
	bool	dtr;
	bool	dtrptt;
	bool	rts;
	bool	rtsptt;
	bool	rtscts;
	int		stopbits;
	char	bfr[2048];
//Methods
	bool	IOselect();
};

//=============================================================================
// MINGW serial port implemenation
//=============================================================================

#else //__WIN32__

#include <windows.h>

class CSerialComm  {
public:
	CSerialComm() {
		rts = dtr = false;
		rtsptt = dtrptt = false;
		rtscts = false;
		baud = CBR_9600;
		stopbits = 2;
		hComm = 0;
	};
	CSerialComm( std::string portname) {
		device = portname;
		CSerialComm();
//		OpenPort();
	};
	virtual	~CSerialComm() {};

//Methods
	bool OpenPort();
	bool IsOpen();
	bool CheckPort(string);
	void ClosePort();
	bool ConfigurePort(DWORD BaudRate,BYTE ByteSize,DWORD fParity,BYTE  Parity,BYTE StopBits);

	bool IsBusy() { return busyflag; };
	void IsBusy(bool val) { busyflag = val; };

	bool ReadByte(char &resp);
	int  ReadData (char *b, int nbr);
	int  ReadBuffer (char *b, int nbr) {
	  return ReadData (b,nbr);
	}
	int  ReadChars (char *b, int nbr, int msec);
	DWORD GetBytesRead();

	bool WriteByte(char bybyte);
	DWORD GetBytesWritten();
	int WriteBuffer(const char *str, int nbr);

	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,DWORD ReadTotalTimeoutMultiplier,DWORD ReadTotalTimeoutConstant,DWORD WriteTotalTimeoutMultiplier,DWORD WriteTotalTimeoutConstant);
	bool SetCommTimeout();

	void Timeout(int tm) { timeout = tm; return; };
	int  Timeout() { return timeout; };
	void FlushBuffer();

	void Device (std::string dev) { device = dev;};
	std::string Device() { return device;};

	void Baud(int b) { baud = b;};
	int  Baud() { return baud;};

	void Retries(int r) { retries = r;}
	int  Retries() { return retries;}

	void RTS(bool r){rts = r;}
	bool RTS(){return rts;}
	void setRTS(bool b);

	void RTSptt(bool b){rtsptt = b;}
	bool RTSptt(){return rtsptt;}

	void DTR(bool d){dtr = d;}
	bool DTR(){return dtr;}
	void setDTR(bool b);

	void DTRptt(bool b){dtrptt = b;}
	bool DTRptt(){return dtrptt;}

	void RTSCTS(bool b){rtscts = b;}
	bool RTSCTS(){return rtscts;}
	void SetPTT(bool b);

	void Stopbits(int n) {stopbits = (n == 1 ? 1 : 2);}
	int  Stopbits() { return stopbits;}

//Members
private:
	std::string device;
	//For use by CreateFile
	HANDLE			hComm;

	//DCB Defined in WinBase.h
	DCB				dcb;
	COMMTIMEOUTS	CommTimeoutsSaved;
	COMMTIMEOUTS	CommTimeouts;

	//Is the Port Ready?
	bool			bPortReady;

	//Number of Bytes Written to port
	DWORD			nBytesWritten;

	//Number of Bytes Read from port
	DWORD			nBytesRead;

	//Number of bytes Transmitted in the cur session
	DWORD			nBytesTxD;

	int  timeout;
	bool busyflag;

	int		baud;
	int		retries;

	bool	dtr;
	bool	dtrptt;
	bool	rts;
	bool	rtsptt;
	bool	rtscts;
	int		stopbits;
};

#endif // __WIN32__

#endif // SERIAL_H
