#include "stdafx.h"

#if defined( __linux__ ) || defined(__APPLE__)
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include <time.h>
#include <errno.h>
#include <iconv.h>
#include <sys/time.h>
#include <locale.h>
#include <string>
#include <stdexcept>
#include <librdkafka/rdkafka.h>
#include <codecvt>
#include <locale>
#endif

#include <librdkafka/rdkafka.h>
#include <stdio.h>
#include <wchar.h>
#include "1C_kafka.h"
#include <string>
#include <locale.h>
#include "version.h"
#include <stdexcept>
#include <codecvt>
#include <locale>

#define TIME_LEN 65

#define BASE_ERRNO     7

#ifdef WIN32
#pragma setlocale("ru-RU" )
#endif

static const wchar_t* g_PropNames[] = {
	L"Message",
	L"Key"
};

static const wchar_t* g_PropNamesRu[] = {
	L"Message",
	L"Key"
};

//swd методы EN
static const wchar_t* g_MethodNames[] = {
	L"GetVersion",
	L"SendProducer",
	L"Consume",
	L""
};

//swd методы RU
static const wchar_t* g_MethodNamesRu[] = {
	L"ПолучитьВерсию",
	L"SendProducer",
	L"Consume",
	L""
};

static const wchar_t g_kClassNames[] = L"CKAFKA"; //"|OtherClass1|OtherClass2";
static IAddInDefBase* pAsyncEvent = NULL;

//swd INIT KAFKA-------------------------------------------------------------//
static volatile sig_atomic_t run = 1;
static void sigterm(int sig) {
	run = 0;
}
static void stop(int sig)
{
	run = 0;
	fclose(stdin); /* abort fgets() */
}
static void
dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque)
{
	// if (rkmessage->err)
	// fprintf(stderr, "%% Message delivery failed: %s\n",
	//		rd_kafka_err2str(rkmessage->err));
	// else
	// fprintf(stderr,
	//		"%% Message delivered (%zd bytes, "
	//		"partition %" PRId32 ")\n",
	//		rkmessage->len, rkmessage->partition);
}
std::string g_key = "";
std::string g_message = "";


uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static WcharWrapper s_names(g_kClassNames);
//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new CKAFKA;
		return (long)*pInterface;
	}
	return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	g_capabilities = capabilities;
	return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
	if (!*pIntf)
		return -1;

	delete* pIntf;
	*pIntf = 0;
	return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
	return s_names;
}
//---------------------------------------------------------------------------//
#if !defined( __linux__ ) && !defined(__APPLE__)
VOID CALLBACK MyTimerProc(PVOID lpParam, BOOLEAN TimerOrWaitFired);
#else
static void MyTimerProc(int sig);
#endif //__linux__

// CKAFKA
//---------------------------------------------------------------------------//
CKAFKA::CKAFKA()
{
	m_iMemory = 0;
	m_iConnect = 0;
#if !defined( __linux__ ) && !defined(__APPLE__)
	m_hTimerQueue = 0;
#endif //__linux__
}
//---------------------------------------------------------------------------//
CKAFKA::~CKAFKA()
{
}
//---------------------------------------------------------------------------//
bool CKAFKA::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CKAFKA::GetInfo()
{
	// Component should put supported component technology version 
	// This component supports 2.0 version
	return 2000;
}

//---------------------------------------------------------------------------//
void CKAFKA::Done()
{
#if !defined( __linux__ ) && !defined(__APPLE__)
	if (m_hTimerQueue)
	{
		DeleteTimerQueue(m_hTimerQueue);
		m_hTimerQueue = 0;
	}
#endif //__linux__
}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CKAFKA::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	const wchar_t* wsExtension = L"AddInNativeExtension";
	int iActualSize = ::wcslen(wsExtension) + 1;
	WCHAR_T* dest = 0;

	if (m_iMemory)
	{
		if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CKAFKA::GetNProps()
{
	// You may delete next lines and add your own implementation code here
	return ePropLast;
}
//---------------------------------------------------------------------------//
long CKAFKA::FindProp(const WCHAR_T* wsPropName)
{
	long plPropNum = -1;
	wchar_t* propName = 0;

	::convFromShortWchar(&propName, wsPropName);
	plPropNum = findName(g_PropNames, propName, ePropLast);

	if (plPropNum == -1)
		plPropNum = findName(g_PropNamesRu, propName, ePropLast);

	delete[] propName;

	return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CKAFKA::GetPropName(long lPropNum, long lPropAlias)
{
	if (lPropNum >= ePropLast)
		return NULL;

	wchar_t* wsCurrentName = NULL;
	WCHAR_T* wsPropName = NULL;
	int iActualSize = 0;

	switch (lPropAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}
//swd GetPropVal--------------------------------------------------------------------//
bool CKAFKA::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	switch (lPropNum)
	{
	case e_message:
		TV_VT(pvarPropVal) = VTYPE_PSTR;
		string_to_tVariant(g_message, pvarPropVal);
		break;
	case e_key:
		TV_VT(pvarPropVal) = VTYPE_PSTR;
		string_to_tVariant(g_key, pvarPropVal);
		break;
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CKAFKA::SetPropVal(const long lPropNum, tVariant* varPropVal)
{
	switch (lPropNum)
	{
	case e_message:
		return false;
		break;
	case e_key:
		return false;
		break;
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CKAFKA::IsPropReadable(const long lPropNum)
{
	switch (lPropNum)
	{
	case e_message:
		return true;
	case e_key:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CKAFKA::IsPropWritable(const long lPropNum)
{
	switch (lPropNum)
	{
	case e_message:
		return false;
	case e_key:
		return false;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CKAFKA::GetNMethods()
{
	return eMethLast;
}
//---------------------------------------------------------------------------//
long CKAFKA::FindMethod(const WCHAR_T* wsMethodName)
{
	long plMethodNum = -1;
	wchar_t* name = 0;

	::convFromShortWchar(&name, wsMethodName);

	plMethodNum = findName(g_MethodNames, name, eMethLast);

	if (plMethodNum == -1)
		plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

	delete[] name;

	return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CKAFKA::GetMethodName(const long lMethodNum, const long lMethodAlias)
{
	if (lMethodNum >= eMethLast)
		return NULL;

	wchar_t* wsCurrentName = NULL;
	WCHAR_T* wsMethodName = NULL;
	int iActualSize = 0;

	switch (lMethodAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}
//swd Параметры-----------------------------------------------------------//
long CKAFKA::GetNParams(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eGetInfo:
		return 0;
	case eProduce:
		return 6;
	case eConsume:
		return 5;
	default:
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------//
bool CKAFKA::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant* pvarParamDefValue)
{
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
	case eGetInfo:
		return true;
	case eProduce:
		return true;
	case eConsume:
		return true;
	default:
		return false;
	}

	return false;
}
//swd HasRetVal
//---------------------------------------------------------------------------//
bool CKAFKA::HasRetVal(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eGetInfo:
		return true;
	case eProduce:
		return true;
	case eConsume:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CKAFKA::CallAsProc(const long lMethodNum,
	tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
std::string uint16ToString(const uint16_t* text, size_t length) {
	// Create a wstring from the uint16_t array
	std::wstring wstr(text, text + length);

	// Create a converter from UTF-16 to UTF-8
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	// Convert the wstring to a UTF-8 string
	return converter.to_bytes(wstr);
}
//---------------------------------------------------------------------------//
// swd Helper function to convert wchar_t* to std::string
std::string CKAFKA::wstringToString(const wchar_t* wstr) {
	if (wstr == nullptr)
		return "";

	// Determine the size needed for the conversion
	std::size_t len = std::wcslen(wstr) + 1; // +1 for null terminator
	char* buffer = new char[len];

	// Convert wchar_t* to char*
	std::wcstombs(buffer, wstr, len);

	// Create std::string from char*
	std::string result(buffer);

	delete[] buffer; // Clean up allocated memory

	return result;
}
//---------------------------------------------------------------------------//
bool CKAFKA::string_to_tVariant(const std::string& str, tVariant* val) {
	char* t1;
	TV_VT(val) = VTYPE_PSTR;  // Assuming this means string

	// Allocate memory for the string
	m_iMemory->AllocMemory((void**)&t1, (str.length() + 1) * sizeof(char));

	// Copy the string (including the null terminator)
	strcpy(t1, str.c_str());

	// Set the return value
	val->pstrVal = t1;
	val->strLen = str.length();

	return true;
}

//swd consume---------------------------------------------------------------//
bool CKAFKA::consume(tVariant* paParams)
{

	try {

		g_message = "";
		g_key = "";

		std::string p_brokers = uint16ToString((paParams)->pwstrVal, (paParams)->strLen);
		std::string p_topic = uint16ToString((paParams + 1)->pwstrVal, (paParams + 1)->strLen);
		std::string p_group = uint16ToString((paParams + 2)->pwstrVal, (paParams + 2)->strLen);
		std::string p_username = uint16ToString((paParams + 3)->pwstrVal, (paParams + 3)->strLen);
		std::string p_password = uint16ToString((paParams + 4)->pwstrVal, (paParams + 4)->strLen);

		rd_kafka_t* rk;
		rd_kafka_conf_t* conf;
		char errstr[512];

		conf = rd_kafka_conf_new();

		if (rd_kafka_conf_set(conf, "bootstrap.servers", p_brokers.c_str(), errstr,
			sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("Error setting rebalance callback: ");
		}

		if (rd_kafka_conf_set(conf, "group.id", p_group.c_str(), errstr,
			sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error group.id");
		}

		if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr,
			sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error auto.offset.reset");
		}

		if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error security.protocol");
		}

		if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.mechanism");
		}

		if (rd_kafka_conf_set(conf, "sasl.username", p_username.c_str(),
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.username");
		}

		if (rd_kafka_conf_set(conf, "sasl.password", p_password.c_str(),
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.password");
		}

		rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
		if (!rk)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error Failed to create new consumer");
		}

		conf = NULL;
		rd_kafka_poll_set_consumer(rk);
		rd_kafka_topic_partition_list_t* subscription;
		subscription = rd_kafka_topic_partition_list_new(1);

		rd_kafka_topic_partition_list_add(subscription,
			p_topic.c_str(),
			/* the partition is ignored
			 * by subscribe() */
			RD_KAFKA_PARTITION_UA);
		rd_kafka_resp_err_t err;
		err = rd_kafka_subscribe(rk, subscription);
		if (err)
		{
			rd_kafka_topic_partition_list_destroy(subscription);
			rd_kafka_destroy(rk);
			throw std::runtime_error("error Failed to subscribe to topic");
		}

		rd_kafka_message_t* rkm = rd_kafka_consumer_poll(rk, 5000);
		if (!rkm)
			return false; // timeout: no message

		if (rkm->err)
		{
			rd_kafka_message_destroy(rkm);
			throw std::runtime_error("Consumer error"); // timeout: no message
		}
		rd_kafka_message_destroy(rkm);
		rd_kafka_consumer_close(rk);
		rd_kafka_destroy(rk);

		g_message = (const char*)rkm->payload;
		g_key = (const char*)rkm->key;

		return true;

	}
	catch (const std::exception& e) {
		//std::cerr << "Exception caught: " << e.what() << std::endl;
		return false;
	}
}
//swd produce--------------------------------------------------------------//
std::string CKAFKA::produce(tVariant* paParams)
{
	//brokers
	//#ifdef __linux__
	try {
		// Cast uint16_t* to wchar_t*
		std::string p_brokers = uint16ToString((paParams)->pwstrVal, (paParams)->strLen);
		std::string p_topic = uint16ToString((paParams + 1)->pwstrVal, (paParams + 1)->strLen);
		std::string p_username = uint16ToString((paParams + 2)->pwstrVal, (paParams + 2)->strLen);
		std::string p_password = uint16ToString((paParams + 3)->pwstrVal, (paParams + 3)->strLen);
		std::string p_key = uint16ToString((paParams + 4)->pwstrVal, (paParams + 4)->strLen);
		std::string p_message = uint16ToString((paParams + 5)->pwstrVal, (paParams + 5)->strLen);

		rd_kafka_t* rk;
		rd_kafka_conf_t* conf;
		char errstr[512];

		conf = rd_kafka_conf_new();

		if (rd_kafka_conf_set(conf, "bootstrap.servers", p_brokers.c_str(), errstr,
			sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error bootstrap.servers");
		}

		if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error security.protocol");
		}

		if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.mechanism");
		}

		if (rd_kafka_conf_set(conf, "sasl.username", p_username.c_str(),
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.username");
		}

		if (rd_kafka_conf_set(conf, "sasl.password", p_password.c_str(),
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error sasl.password");
		}

		rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
		if (!rk)
		{
			rd_kafka_conf_destroy(conf);
			throw std::runtime_error("error Failed to create new producer");
		}

		rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

		signal(SIGINT, stop);

		rd_kafka_resp_err_t err;

		err = rd_kafka_producev(
			rk,
			RD_KAFKA_V_TOPIC(p_topic.c_str()),
			RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
			RD_KAFKA_V_KEY(const_cast<char*>(p_key.c_str()), p_key.size()),
			RD_KAFKA_V_VALUE(const_cast<char*>(p_message.c_str()), p_message.size()),
			RD_KAFKA_V_OPAQUE(NULL),
			RD_KAFKA_V_END);

		if (err)
		{
			/*
			 * Failed to *enqueue* message for producing.
			 */
			if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL)
			{
				rd_kafka_poll(rk,
					1000 /*block for max 1000ms*/);
			}
			throw std::runtime_error("Failed to produce to topic");
		}
		
		rd_kafka_poll(rk, 0 /*non-blocking*/);
		rd_kafka_flush(rk, 1000 /* wait for max 10 seconds */);
		rd_kafka_destroy(rk);

		return "Persisted";

	}

	catch (const std::exception& ex) {
		// Handle standard exceptions
		return "Exception: " + std::string(ex.what());
	}
	catch (...) {
		// Handle any other type of exceptions
		return "Unknown exception occurred";
	}
	//#endif
}

//swd CallAsFunc-----------------------------------------------------------------------//
bool CKAFKA::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
	case eGetInfo:
		TV_VT(pvarRetValue) = VTYPE_PSTR;
		string_to_tVariant(VERSION_STRING, pvarRetValue);
		return true;

	case eProduce:
		TV_VT(pvarRetValue) = VTYPE_PSTR;
		string_to_tVariant(produce(paParams), pvarRetValue);
		return true;

	case eConsume:
		TV_VT(pvarRetValue) = VTYPE_BOOL;
		pvarRetValue->bVal = consume(paParams);
		return true;

	default:
		return false;

	}
}
//---------------------------------------------------------------------------//
void CKAFKA::SetLocale(const WCHAR_T* loc)
{
#if !defined( __linux__ ) && !defined(__APPLE__)
	_wsetlocale(LC_ALL, loc);
#else
	//We convert in char* char_locale
	//also we establish locale
	//setlocale(LC_ALL, char_locale);
#endif
}
/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool CKAFKA::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
void CKAFKA::addError(uint32_t wcode, const wchar_t* source,
	const wchar_t* descriptor, long code)
{
	if (m_iConnect)
	{
		WCHAR_T* err = 0;
		WCHAR_T* descr = 0;

		::convToShortWchar(&err, source);
		::convToShortWchar(&descr, descriptor);

		m_iConnect->AddError(wcode, err, descr, code);
		delete[] err;
		delete[] descr;
	}
}
//---------------------------------------------------------------------------//
long CKAFKA::findName(const wchar_t* names[], const wchar_t* name,
	const uint32_t size) const
{
	long ret = -1;
	for (uint32_t i = 0; i < size; i++)
	{
		if (!wcscmp(names[i], name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
	if (!len)
		len = ::wcslen(Source) + 1;

	if (!*Dest)
		*Dest = new WCHAR_T[len];

	WCHAR_T* tmpShort = *Dest;
	wchar_t* tmpWChar = (wchar_t*)Source;
	uint32_t res = 0;

	::memset(*Dest, 0, len * sizeof(WCHAR_T));
#ifdef __linux__
	size_t succeed = (size_t)-1;
	size_t f = len * sizeof(wchar_t), t = len * sizeof(WCHAR_T);
	const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
	iconv_t cd = iconv_open("UTF-16LE", fromCode);
	if (cd != (iconv_t)-1)
	{
		succeed = iconv(cd, (char**)&tmpWChar, &f, (char**)&tmpShort, &t);
		iconv_close(cd);
		if (succeed != (size_t)-1)
			return (uint32_t)succeed;
	}
#endif //__linux__
	for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
	{
		*tmpShort = (WCHAR_T)*tmpWChar;
	}

	return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
	if (!len)
		len = getLenShortWcharStr(Source) + 1;

	if (!*Dest)
		*Dest = new wchar_t[len];

	wchar_t* tmpWChar = *Dest;
	WCHAR_T* tmpShort = (WCHAR_T*)Source;
	uint32_t res = 0;

	::memset(*Dest, 0, len * sizeof(wchar_t));
#ifdef __linux__
	size_t succeed = (size_t)-1;
	const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
	size_t f = len * sizeof(WCHAR_T), t = len * sizeof(wchar_t);
	iconv_t cd = iconv_open("UTF-32LE", fromCode);
	if (cd != (iconv_t)-1)
	{
		succeed = iconv(cd, (char**)&tmpShort, &f, (char**)&tmpWChar, &t);
		iconv_close(cd);
		if (succeed != (size_t)-1)
			return (uint32_t)succeed;
	}
#endif //__linux__
	for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
	{
		*tmpWChar = (wchar_t)*tmpShort;
	}

	return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
	uint32_t res = 0;
	WCHAR_T* tmpShort = (WCHAR_T*)Source;

	while (*tmpShort++)
		++res;

	return res;
}
//---------------------------------------------------------------------------//

#ifdef LINUX_OR_MACOS
WcharWrapper::WcharWrapper(const WCHAR_T* str) : m_str_WCHAR(NULL),
m_str_wchar(NULL)
{
	if (str)
	{
		int len = getLenShortWcharStr(str);
		m_str_WCHAR = new WCHAR_T[len + 1];
		memset(m_str_WCHAR, 0, sizeof(WCHAR_T) * (len + 1));
		memcpy(m_str_WCHAR, str, sizeof(WCHAR_T) * len);
		::convFromShortWchar(&m_str_wchar, m_str_WCHAR);
	}
}
#endif
//---------------------------------------------------------------------------//
WcharWrapper::WcharWrapper(const wchar_t* str) :
#ifdef LINUX_OR_MACOS
	m_str_WCHAR(NULL),
#endif 
	m_str_wchar(NULL)
{
	if (str)
	{
		int len = wcslen(str);
		m_str_wchar = new wchar_t[len + 1];
		memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
		memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
#ifdef LINUX_OR_MACOS
		::convToShortWchar(&m_str_WCHAR, m_str_wchar);
#endif
	}

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
#ifdef LINUX_OR_MACOS
	if (m_str_WCHAR)
	{
		delete[] m_str_WCHAR;
		m_str_WCHAR = NULL;
	}
#endif
	if (m_str_wchar)
	{
		delete[] m_str_wchar;
		m_str_wchar = NULL;
	}
}
//---------------------------------------------------------------------------//
