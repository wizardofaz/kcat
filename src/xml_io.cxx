//======================================================================
// xml_io.cxx
//
// copyright 2009, W1HKJ
//
//======================================================================

#include <cstdlib>
#include <string>
#include <vector>
#include <queue>

#include "xml_io.h"
#include "support.h"
#include "debug.h"
#include "kcat.h"
#include "config.h"

#include "XmlRpc.h"

#define XML_DEBUG 0

using namespace std;
using XmlRpc::XmlRpcValue;

extern char *print(FREQMODE);

queue<long> qfreq;

static const double TIMEOUT = 0.5;

// these are get only
static const char* main_get_trx_state   = "main.get_trx_state";

// these are set only
static const char* main_set_tx          = "main.tx";
static const char* main_set_rx          = "main.rx";
static const char* rig_set_name         = "rig.set_name";
static const char* rig_set_modes        = "rig.set_modes";
static const char* rig_set_bandwidths   = "rig.set_bandwidths";
static const char* rig_take_control     = "rig.take_control";
static const char* rig_release_control  = "rig.release_control";

// these are get/set
static const char* main_get_frequency   = "main.get_frequency";
static const char* main_set_wf_sideband = "main.set_wf_sideband";
static const char* rig_set_frequency    = "rig.set_frequency";
static const char* rig_set_mode         = "rig.set_mode";
static const char* rig_get_mode         = "rig.get_mode";
static const char* rig_set_bandwidth    = "rig.set_bandwidth";
static const char* rig_get_bandwidth    = "rig.get_bandwidth";
static const char *rig_set_smeter       = "rig.set_smeter";
static const char *rig_set_pwrmeter     = "rig.set_pwrmeter";


static XmlRpc::XmlRpcClient* client;
static XmlRpcValue* status_query;

#define RIG_UPDATE_INTERVAL  50 // milliseconds
#define CHECK_UPDATE_COUNT   (500 / RIG_UPDATE_INTERVAL)

bool exit_xmlrpc = false;
bool fldigi_online = false;
bool rig_reset = false;
bool ptt_on = false;
bool ignore = false;

// not used
/*
class auto_mutex
{
	pthread_mutex_t& mutex;
	auto_mutex(const auto_mutex& m);
	auto_mutex& operator=(const auto_mutex& m);
public:
	auto_mutex(pthread_mutex_t& m) : mutex(m) { pthread_mutex_lock(&mutex); }
	~auto_mutex(void) { pthread_mutex_unlock(&mutex); }
};
*/

//=====================================================================
// socket ops
//=====================================================================

void open_rig_xmlrpc()
{
	client = new XmlRpc::XmlRpcClient( "127.0.0.1", 7362 );
	// XmlRpc::setVerbosity(5);

	// Create a value suitable for the system.multicall method, which
	// accepts a single argument: an array of struct { methodName, params }
	// where methodName is a string and params is an array of method
	// arguments.  The params member is mandatory so we use a single-element
	// array containing nil.

	XmlRpcValue nil(XmlRpc::nil);
	status_query = new XmlRpcValue;
	const char* status_methods[] = {
		main_get_trx_state, main_get_frequency,
		rig_get_mode, rig_get_bandwidth
	};
	for (size_t i = 0; i < sizeof(status_methods)/sizeof(*status_methods); i++) {
		(*status_query)[0][i]["methodName"] = status_methods[i];
		(*status_query)[0][i]["params"][0] = nil;
	};
}

void close_rig_xmlrpc()
{
	delete client;
	client = NULL;
	delete status_query;
	status_query = NULL;
}

static inline void execute(const char* name, const XmlRpcValue& param, XmlRpcValue& result)
{
	if (!client) open_rig_xmlrpc();
	if (!client->execute(name, param, result, TIMEOUT))
		throw XmlRpc::XmlRpcException(name);
}

// --------------------------------------------------------------------
// send functions
// --------------------------------------------------------------------

void send_modes() { 
	XmlRpcValue modes, res;
	int i = 0;
	for (const char** mode = szmodes; *mode; mode++, i++) {
		modes[0][i] = *mode;
	}

	try {
		execute(rig_set_modes, modes, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_bandwidths()
{
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = szBW; *bw; bw++, i++) {
		bandwidths[0][i] = *bw;
	}

	try {
		execute(rig_set_bandwidths, bandwidths, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_name()
{
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(PACKAGE_NAME), res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_ptt_changed(bool PTT)
{
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_new_freq(long freq)
{
	try {
		xmlvfo.freq = freq;
		XmlRpcValue f((double)freq), res;
		execute(rig_set_frequency, f, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_xml_freq(long freq)
{
	qfreq.push(freq);
}

void send_queue()
{
	while (!qfreq.empty()) {
		send_new_freq(qfreq.front());
		qfreq.pop();
	}
}

void send_new_mode(int md)
{
	try {
		xmlvfo.imode = md;
		XmlRpcValue mode(szmodes[md]), res;
		execute(rig_set_mode, mode, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_new_bandwidth(int bw)
{
	try {
		xmlvfo.iBW = bw;
		XmlRpcValue bandwidth(szBW[bw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_sideband()
{
	try {
		XmlRpcValue sideband(modetype[vfoA.imode] == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_smeter_val(int val)
{
	try {
		XmlRpcValue mval((int)val), res;
		execute(rig_set_smeter, mval, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_pwrmeter_val(int val)
{
	try {
		XmlRpcValue mval((int)val), res;
		execute(rig_set_pwrmeter, mval, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}


// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------

bool xmlvfo_changed = false;

static void check_for_ptt_change(const XmlRpcValue& trx_state)
{
	bool nuptt = (trx_state == "TX");
	if (nuptt != ptt_on) {
		if (XML_DEBUG) {
			string txstate = trx_state;
			LOG_WARN("%s", txstate.c_str());
		}
		ptt_on = nuptt;
		Fl::awake(setPTT, (void*)ptt_on);
	}
}

static void check_for_frequency_change(const XmlRpcValue& freq)
{
	double f = freq;
	long newfreq = (long)f;
	if (newfreq != xmlvfo.freq) {
		xmlvfo.freq = newfreq;
		xmlvfo_changed = true;
	}
}

static void check_for_mode_change(const XmlRpcValue& new_mode)
{
	string smode = new_mode;
	if (smode != szmodes[vfoA.imode]) {
		int imode = 0;
		while (szmodes[imode] != NULL && smode != szmodes[imode])
			imode++;
		if (szmodes[imode] != NULL && imode != xmlvfo.imode) {
			xmlvfo.imode = imode;
			xmlvfo_changed = true;
		}
	}
}

static void check_for_bandwidth_change(const XmlRpcValue& new_bw)
{
	string sbw = new_bw;
	if (sbw != szBW[vfoA.iBW]) {
		int ibw = 0;
		const char **bwt = &szBW[0];
		while ((bwt[ibw] != NULL) && (sbw != bwt[ibw])) ibw++;
		if (bwt[ibw] != NULL && ibw != xmlvfo.iBW) {
			xmlvfo.iBW = ibw;
			xmlvfo_changed = true;
		}
	}
}

void set_fldigi_connect (void *d)
{
//	bool b = reinterpret_cast<long>(d);
//	box_fldigi_connect->color(b ? FL_RED : FL_BACKGROUND2_COLOR);
//	box_fldigi_connect->redraw();
}

static void send_rig_info()
{
	XmlRpcValue res;
	try {
		execute(rig_take_control, XmlRpcValue(), res);

		send_name();
		send_modes();
		send_bandwidths();

		xmlvfo = vfoA;

		send_new_mode(xmlvfo.imode);

		send_new_bandwidth(xmlvfo.iBW);

		send_sideband();

		send_new_freq(xmlvfo.freq);

		fldigi_online = true;
		rig_reset = false;
		Fl::awake(set_fldigi_connect, (void *)1);
	} catch (...) {
		throw;
	}
}

void send_no_rig()
{
	XmlRpcValue res;
	try {
		execute(rig_set_name, "No rig", res);
		send_bandwidths();
		send_modes();
		execute(rig_set_mode, "USB", res);
		XmlRpcValue sideband("USB");
		execute(main_set_wf_sideband, sideband, res);
		execute(rig_release_control, XmlRpcValue(), res);
	} catch (...) {
		printf("Fldigi not running!\n");
	}
}

static void get_fldigi_status()
{
	XmlRpcValue status;
	try {
		execute("system.multicall", *status_query, status);
		if (ignore) {
			ignore = false;
			return;
		}
		check_for_ptt_change(status[0][0]);
		if (!ptt_on) {
			xmlvfo.src = XML;
			xmlvfo_changed = false;
			check_for_mode_change(status[2][0]);
			check_for_bandwidth_change(status[3][0]);
			check_for_frequency_change(status[1][0]);
			if (xmlvfo_changed)
				Fl::awake(set_xml_values);
		}
	} catch (...) {
		throw;
	}
}

void * xmlrpc_thread_loop(void *d)
{
	int try_count = CHECK_UPDATE_COUNT;
	for (;;) {
		MilliSleep(RIG_UPDATE_INTERVAL);
		if (exit_xmlrpc) break;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (rig_reset || (!fldigi_online && (--try_count == 0)))
				send_rig_info();
			else if (fldigi_online) {
				send_queue();
				get_fldigi_status();
			}
		} catch (const XmlRpc::XmlRpcException& e) {
			if (XML_DEBUG)
				LOG_WARN("%s", e.getMessage().c_str());
			fldigi_online = false;
			rig_reset = false;
			try_count = CHECK_UPDATE_COUNT;
			Fl::awake(set_fldigi_connect, (void *)0);
		}
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
	return NULL;
}

