// =====================================================================
//
// xmlrpc_io.cxx
//
// connect to logbook xmlrpc server
//
// =====================================================================

#include <iostream>
#include <cmath>
#include <cstring>
#include <stdlib.h>

#include <FL/Fl.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>

#include "XmlRpc.h"

#include "support.h"
#include "config.h"
#include "date.h"
#include "icons.h"
#include "timeops.h"

using namespace XmlRpc;
using namespace std;

char *szTime(int typ)
{
	static char szDt[80];
	time_t tmptr;
	tm sTime;
	time (&tmptr);
	switch (typ) {
		case 0:
			localtime_r(&tmptr, &sTime);
			strftime(szDt, 79, "%H%M", &sTime);
			break;
		case 1:
			localtime_r(&tmptr, &sTime);
			strftime(szDt, 79, "%H:%M", &sTime);
			break;
		case 2:
			gmtime_r (&tmptr, &sTime);
			strftime(szDt, 79, "%H%MZ", &sTime);
			break;
		case 3:
			gmtime_r (&tmptr, &sTime);
			strftime(szDt, 79, "%H:%MZ", &sTime);
			break;
		case 4:
			gmtime_r (&tmptr, &sTime);
			strftime(szDt, 79, "%H%M UTC", &sTime);
			break;
		case 5:
			gmtime_r (&tmptr, &sTime);
			strftime(szDt, 79, "%H:%M UTC", &sTime);
			break;
		default:
			localtime_r(&tmptr, &sTime);
			strftime(szDt, 79, "%H%ML", &sTime);
	}
	return szDt;
}

static const char *month_name[] =
{
  "January",
  "Febuary",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

char *szDate(int fmt)
{
	static char szDt[20];
	static char szMonth[10];

	time_t tmptr;
	tm sTime;
	time (&tmptr);
	if ((fmt & 0x80) == 0x80) {
		gmtime_r (&tmptr, &sTime);
	} else {
		localtime_r(&tmptr, &sTime);
	}
	switch (fmt & 0x7F) {
		case 1 :
			snprintf (szDt, sizeof(szDt), "%02d/%02d/%02d",
				sTime.tm_mon + 1, 
				sTime.tm_mday, 
				sTime.tm_year > 100 ? sTime.tm_year - 100 : sTime.tm_year);
			break;
		case 2 :
			snprintf (szDt, sizeof(szDt), "%4d-%02d-%02d", 
				sTime.tm_year + 1900,
				sTime.tm_mon + 1, 
				sTime.tm_mday);
			break;
		case 3 :  
			snprintf (szDt, sizeof(szDt), "%s %2d, %4d",
				month_name[sTime.tm_mon], 
				sTime.tm_mday, 
				sTime.tm_year + 1900);
			break;
		case 4 :
			strcpy (szMonth, month_name [sTime.tm_mon]);
			szMonth[3] = 0; 
			snprintf (szDt, sizeof(szDt), "%s %2d, %4d", 
				szMonth,
				sTime.tm_mday, 
				sTime.tm_year + 1900);
			break;
		case 5 :
			strcpy (szMonth, month_name [sTime.tm_mon]);
			szMonth[3] = 0;
			for (int i = 0; i < 3; i++) szMonth[i] = toupper(szMonth[i]);
			snprintf (szDt, sizeof(szDt), "%s %d", 
				szMonth, 
				sTime.tm_mday);
			break;
		case 6 :
			snprintf (szDt, sizeof(szDt), "%4d%02d%02d", 
				sTime.tm_year + 1900,
				sTime.tm_mon + 1, 
				sTime.tm_mday);
			break;
		case 0 :
		default :
			snprintf (szDt, sizeof(szDt), "%02d/%02d/%04d",
				sTime.tm_mon + 1, 
				sTime.tm_mday,
				sTime.tm_year + 1900); 
		break;
	}
	return szDt;
}

XmlRpcClient log_client("localhost", 8421);

static bool connected = false;

bool test_connection()
{
	XmlRpcValue result;
	if (log_client.execute("system.listMethods", XmlRpcValue(), result))
		return true;
	return false;
}

void xml_get_record(const char *callsign)
{
	if (!connected) return;
	XmlRpcValue oneArg, result;
	oneArg[0] = callsign;
	if (log_client.execute("log.get_record", oneArg, result)) {
		string adifline = std::string(result);
		size_t pos1 = adifline.find("<NAME:");
		if (pos1 == std::string::npos) {
			txt_name->value("");
			return;
		}
		pos1 = adifline.find(">", pos1) + 1;
		size_t pos2 = adifline.find("<", pos1);
		string name = adifline.substr(pos1, pos2 - pos1);
		txt_name->value(name.c_str());
	} else {
		fl_alert2("Logbook server: no response");
		txt_name->value("");
	}
//  else
//    std::cout << "Error calling 'log.get_record'\n\n";
}

void xml_add_record()
{
	if (!connected || txt_sta->value()[0] == 0) return;

	if (!test_connection()) {
		fl_alert2("Logbook server not running");
		btn_log_it->deactivate();
		btn_dups->deactivate();
		btnConnect->value(0);
		return;
	}

	char *szt = szTime(2);
	char *szdt = szDate(0x86);
	char sznbr[6];
	string call = txt_sta->value();
	string freq = txt_freq->value();
	string name = txt_name->value();
	string xin = txt_xchg->value();
	snprintf(sznbr, sizeof(sznbr), "%d", xcvrState.serial_nbr);

	XmlRpcValue oneArg, result;
	char adifrec[200];
	snprintf(adifrec, sizeof(adifrec), "\
<FREQ:%d>%s\
<CALL:%d>%s\
<NAME:%d>%s\
<MODE:2>CW\
<QSO_DATE:8>%s<TIME_ON:4>%s<TIME_OFF:4>%s\
<STX:%d>%s\
<STX_STRING:%d>%s\
<SRX_STRING:%d>%s\
<RST_RCVD:3>599<RST_SENT:3>599<EOR>",
		freq.length(), freq.c_str(),
		call.length(), call.c_str(),
		name.length(), name.c_str(),
		szdt, szt, szt,
		strlen(sznbr), sznbr,
		xcvrState.xout.length(), xcvrState.xout.c_str(),
		xin.length(), xin.c_str());
	oneArg[0] = adifrec;
	log_client.execute("log.add_record", oneArg, result);
//	std::cout << "log.add_record result " << result << "\n\n";
}

void xml_dup_check()
{
	if (!connected) return;
	Fl_Color call_clr = FL_BACKGROUND2_COLOR;
	XmlRpcValue sixargs, result;
	sixargs[0] = txt_sta->value();
	sixargs[1] = "CW";
	sixargs[2] = "0";
	sixargs[3] = "0";
	sixargs[4] = "0";
	sixargs[5] = "0";
	if (log_client.execute("log.check_dup", sixargs, result)) {
		string res = std::string(result);
		if (res == "true")
			call_clr = fl_rgb_color( 255, 110, 180);
	} else {
		fl_alert2("Logbook server: no response");
	}
	txt_sta->color(call_clr);
	txt_sta->redraw();
}

void connect_to_server()
{
	if (btnConnect->value()) {
		if ((connected = test_connection())) {
			btn_log_it->activate();
			btn_dups->activate();
		} else {
			btn_log_it->deactivate();
			btn_dups->deactivate();
			btnConnect->value(0);
		}
	} else {
		connected = false;
		btn_log_it->deactivate();
		btn_dups->deactivate();
	}
}
