// =====================================================================
// serial comms
// =====================================================================

#ifdef WIN32
//////////////////////////////////////////////////////////////////////
// CSerialComm class.
//////////////////////////////////////////////////////////////////////

#include "serialcomm.h"

///////////////////////////////////////////////////////
// Function name	: CSerialComm::OpenPort
// Description	    : Opens the port specified by strPortName
// Return type		: BOOL 
// Argument         : CString strPortName
///////////////////////////////////////////////////////
BOOL CSerialComm::OpenPort(char * szPort, int baud)  {

	szPortName = "//./";
	szPortName.append(szPort);

	hComm = CreateFile(szPortName.c_str(),
              GENERIC_READ | GENERIC_WRITE,
              0,
              0,
              OPEN_EXISTING,
              0,
              0);

	if(hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	ConfigurePort( baud, 8, FALSE, NOPARITY, ONESTOPBIT);
	return TRUE;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::ClosePort
// Description	    : Closes the Port
// Return type		: void 
///////////////////////////////////////////////////////
void CSerialComm::ClosePort()
{
	if (hComm) {
		bPortReady = SetCommTimeouts (hComm, &CommTimeoutsSaved);
		CloseHandle(hComm);
	}
	return;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::GetBytesRead
// Description	    : 
// Return type		: DWORD 
///////////////////////////////////////////////////////
DWORD CSerialComm::GetBytesRead()
{
	return nBytesRead;
}

///////////////////////////////////////////////////////
// Function name	: CSerialComm::GetBytesWritten
// Description	    : returns total number of bytes written to port
// Return type		: DWORD 
///////////////////////////////////////////////////////
DWORD CSerialComm::GetBytesWritten()
{
	return nBytesWritten;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::ReadByte
// Description	    : Reads a byte from the selected port
// Return type		: BOOL 
// Argument         : BYTE& by
///////////////////////////////////////////////////////
BOOL CSerialComm::ReadByte(UCHAR& by)
{
static	BYTE byResByte[1024];
static	DWORD dwBytesTxD=0;

	if (!hComm) return FALSE;
	if (ReadFile (hComm, &byResByte[0], 1, &dwBytesTxD, 0))	{
		if (dwBytesTxD == 1) {
			by= (UCHAR)byResByte[0];
			return TRUE;
		}
	}
	by = 0;
	return FALSE;
}

int  CSerialComm::ReadData (unsigned char *buf, int nchars)
{
	DWORD dwRead = 0;
	int retval;
	busyflag = true;
	retval = ReadFile(hComm, buf, nchars, &dwRead, NULL);
	busyflag = false;
	if (!retval) 
		return 0;
	return (int) dwRead;
}

int CSerialComm::ReadChars (unsigned char *buf, int nchars, int msec)
{
	if (msec) Sleep(msec);
	return ReadData (buf, nchars);
}
	
///////////////////////////////////////////////////////
// Function name	: CSerialComm::WriteByte
// Description	    : Writes a Byte to teh selected port
// Return type		: BOOL 
// Argument         : BYTE by
///////////////////////////////////////////////////////
BOOL CSerialComm::WriteByte(UCHAR by)
{
	if (!hComm) return FALSE;
	nBytesWritten = 0;
	if (WriteFile(hComm,&by,1,&nBytesWritten,NULL)==0)
		return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////
// Function name	: CSerialComm::WriteBuffer
// Description	    : Writes a string to the selected port
// Return type		: BOOL 
// Argument         : BYTE by
///////////////////////////////////////////////////////
int CSerialComm::WriteBuffer(unsigned char *buff, int n)
{
	if (!hComm) 
		return 0;
	busyflag = true;
	WriteFile (hComm, buff, n, &nBytesWritten, NULL);
	busyflag = false;
	return nBytesWritten;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::SetCommunicationTimeouts
// Description	    : Sets the timeout for the selected port
// Return type		: BOOL 
// Argument         : DWORD ReadIntervalTimeout
// Argument         : DWORD ReadTotalTimeoutMultiplier
// Argument         : DWORD ReadTotalTimeoutConstant
// Argument         : DWORD WriteTotalTimeoutMultiplier
// Argument         : DWORD WriteTotalTimeoutConstant
///////////////////////////////////////////////////////
BOOL CSerialComm::SetCommunicationTimeouts(
	DWORD ReadIntervalTimeout,
  DWORD ReadTotalTimeoutMultiplier,
  DWORD ReadTotalTimeoutConstant,
  DWORD WriteTotalTimeoutMultiplier,
  DWORD WriteTotalTimeoutConstant
)
{
	if((bPortReady = GetCommTimeouts (hComm, &CommTimeoutsSaved))==0)	{
		return FALSE;
	}

	CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
	CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;
	
	bPortReady = SetCommTimeouts (hComm, &CommTimeouts);
	
	if(bPortReady ==0) { 
		CloseHandle(hComm);
		return FALSE;
	}
		
	return TRUE;
}

BOOL CSerialComm::SetCommTimeout() {
	return SetCommunicationTimeouts (0L, 0L, 0L, 0L, 0L);
//  return SetCommunicationTimeouts ( MAXDWORD, MAXDWORD, 100L, MAXDWORD, 100L);
  }

///////////////////////////////////////////////////////
// Function name	: ConfigurePort
// Description	    : Configures the Port
// Return type		: BOOL 
// Argument         : DWORD BaudRate
// Argument         : BYTE ByteSize
// Argument         : DWORD fParity
// Argument         : BYTE  Parity
// Argument         : BYTE StopBits
///////////////////////////////////////////////////////
BOOL CSerialComm::ConfigurePort(DWORD	BaudRate, 
								BYTE	ByteSize, 
								DWORD	dwParity, 
								BYTE	Parity, 
								BYTE	StopBits)
{
	if((bPortReady = GetCommState(hComm, &dcb))==0) {
//		fl_message("GetCommState Error on %s", szPortName.c_str());
		CloseHandle(hComm);
		return FALSE;
	}

	dcb.BaudRate		    =	BaudRate;
	dcb.ByteSize		    =	ByteSize;
	dcb.Parity		      =	Parity ;
	dcb.StopBits		    =	StopBits;
	dcb.fBinary		      =	TRUE;
	dcb.fDsrSensitivity =	FALSE;
	dcb.fParity		      =	dwParity;
	dcb.fOutX			      =	FALSE;
	dcb.fInX			      =	FALSE;
	dcb.fNull			      =	FALSE;
	dcb.fAbortOnError	  =	TRUE;
	dcb.fOutxCtsFlow	  =	FALSE;
	dcb.fOutxDsrFlow	  =	FALSE;
	dcb.fDtrControl	    =	DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity =	FALSE;
	dcb.fRtsControl	    =	RTS_CONTROL_DISABLE;
	dcb.fOutxCtsFlow	  =	FALSE;
	dcb.fOutxCtsFlow	  =	FALSE;

	bPortReady = SetCommState(hComm, &dcb);
	if(bPortReady == 0) {
		CloseHandle(hComm);
		return FALSE;
	}
  return SetCommTimeout();
}

#else

//////////////////////////////////////////////////////////////////////
// CSerialComm class for Linux
//////////////////////////////////////////////////////////////////////

#include "linserial.h"
#include <FL/fl_ask.H>

CSerialComm::CSerialComm() {
	timeout = 200; //msec
}

CSerialComm::~CSerialComm() {
	ClosePort(); 
}

///////////////////////////////////////////////////////
// Function name	: CSerialComm::OpenPort
// Description	    : Opens the port specified by strPortName
// Return type		: BOOL 
// Argument         : CString strPortName
///////////////////////////////////////////////////////
bool CSerialComm::OpenPort(char * szPort, int baud)  {
	szPortName = "/dev/";
	szPortName.append(szPort);
	if ((fd = open( szPortName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
		return false;
// save current port settings
	tcgetattr (fd, &oldtio);
	
	int tioarg = TIOCM_RTS | TIOCM_DTR;
	ioctl(fd, TIOCMBIC, &tioarg);
	
	bzero (&newtio, sizeof(newtio));
	newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
// set input mode to non-canonical, no echo
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	
	tcflush (fd, TCIFLUSH);
	tcsetattr (fd, TCSANOW, &newtio);
	
	return true;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::ClosePort
// Description	    : Closes the Port
// Return type		: void 
///////////////////////////////////////////////////////
void CSerialComm::ClosePort()
{
//	int tioarg = TIOCM_RTS | TIOCM_DTR;
	if (!fd) return;
// release the RTS & DTR signal lines just in case it was the rig control port	
//	ioctl(fd, TIOCMBIC, &tioarg);
	tcsetattr (fd, TCSANOW, &oldtio);
	close(fd);
	return;
}

bool  CSerialComm::IOselect ()
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);
	tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout % 1000) * 1000;
	retval = select (FD_SETSIZE, &rfds, (fd_set *)0, (fd_set *)0, &tv);
	if (retval <= 0) // no response from serial port or error returned
		return false;
	return true;
}


///////////////////////////////////////////////////////
// Function name	: CSerialComm::ReadBuffer
// Description	    : Reads upto nchars from the selected port
// Return type		: # characters received 
// Argument         : pointer to buffer; # chars to read
///////////////////////////////////////////////////////
int  CSerialComm::ReadBuffer (unsigned char *buf, int nchars)
{
	if (!fd) return 0;
	int retnum, nread = 0;
	while (nchars > 0) {
		if (!IOselect()) {
			return 0;
		}
		retnum = read (fd, (char *)(buf + nread), nchars);
		if (retnum <= 0) {
			busyflag = false;
			return 0;
		}
		nread += retnum;
		nchars -= retnum;
	}	
	return nread;
}

///////////////////////////////////////////////////////
// Function name	: CSerialComm::WriteBuffer
// Description	    : Writes a string to the selected port
// Return type		: BOOL 
// Argument         : BYTE by
///////////////////////////////////////////////////////
int CSerialComm::WriteBuffer(unsigned char *buff, int n)
{
	if (!fd) 
		return 0;
	busyflag = true;
	int ret = write (fd, buff, n);
	busyflag = false;
	return ret;
}

///////////////////////////////////////////////////////
// Function name : CSerialComm::FlushBuffer
// Description   : flushes the pending rx chars
// Return type   : void
///////////////////////////////////////////////////////
void CSerialComm::FlushBuffer()
{
	if (!fd) return;
	tcflush (fd, TCIFLUSH);
}

#endif
