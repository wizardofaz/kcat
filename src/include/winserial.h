// SerialComm.h: interface for the CSerialComm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <windows.h>
#include <string>

#ifdef UCHAR
#undef UCHAR
typedef unsigned char UCHAR
#endif

class CSerialComm  {
public:
	CSerialComm() {};
	CSerialComm( char * strPortName) {
		CSerialComm();
		OpenPort( strPortName);
	};
	virtual	~CSerialComm() {};

//Methods
	BOOL OpenPort(char * strPortName, int baud = CBR_9600);
	void ClosePort();
	BOOL ConfigurePort(DWORD BaudRate,BYTE ByteSize,DWORD fParity,BYTE  Parity,BYTE StopBits);

	bool IsBusy() { return busyflag; };
	void IsBusy(bool val) { busyflag = val; };

	BOOL ReadByte(UCHAR &resp);
	int  ReadData (unsigned char *b, int nbr);
	int  ReadBuffer (unsigned char *b, int nbr) {
	  return ReadData (b,nbr);
	}
	int  ReadChars (unsigned char *b, int nbr, int msec);
	DWORD GetBytesRead();

	BOOL WriteByte(UCHAR bybyte);
	DWORD GetBytesWritten();
	int WriteBuffer(unsigned char *str, int nbr);

	BOOL SetCommunicationTimeouts(DWORD ReadIntervalTimeout,DWORD ReadTotalTimeoutMultiplier,DWORD ReadTotalTimeoutConstant,DWORD WriteTotalTimeoutMultiplier,DWORD WriteTotalTimeoutConstant);
	BOOL SetCommTimeout();

	void Timeout(int tm) { timeout = tm; return; };
	int  Timeout() { return timeout; };
	void FlushBuffer() {};

//Members
private:
	std::string		szPortName;
	//For use by CreateFile
	HANDLE			hComm;

	//DCB Defined in WinBase.h
	DCB				dcb;
	COMMTIMEOUTS	CommTimeoutsSaved;
	COMMTIMEOUTS	CommTimeouts;

	//Is the Port Ready?
	BOOL			bPortReady;
	
	//Number of Bytes Written to port
	DWORD			nBytesWritten;

	//Number of Bytes Read from port
	DWORD			nBytesRead;

	//Number of bytes Transmitted in the cur session
	DWORD			nBytesTxD;

	int  timeout;
	bool busyflag;
	
};

#endif
