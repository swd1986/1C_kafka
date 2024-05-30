#ifdef _WIN32
#include <Winsock2.h>
#endif
#include "stdafx.h"

#if defined( __linux__ ) || defined(__APPLE__)
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <iconv.h>
#include <sys/time.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include "kafka_connect_native.h"
#include <string>
#include <clocale>
#include <iostream> 
#include <algorithm>
#include <iterator>
#include <locale>
#include <codecvt>
#ifdef _WIN32
#include <comdef.h>
#endif
#include "librdkafka/rdkafka.h"
#include <signal.h>

//log
#include <ios>
#include <fstream>
#include <vector>

using std::string;

#define		TIME_LEN				65
wstring		version					= L"MDM. 1C Connection Kafka (Native C++). TOO BI-Innovations. 0.51";

rd_kafka_t*	rk_producer;

uint32_t	m_ui_producer_offset	= -1;
uint32_t	m_ui_producer_partition = -1;
uint32_t	m_ui_consumer_offset	= -1;
uint32_t	m_ui_consumer_partition = -1;

wstring		m_ui_consumer_value		= L"";
wstring		m_ui_consumer_key		= L"";

wstring		m_ui_consumer_timestamp = L"";
wstring		m_ui_consumer_header	= L"";
wstring		m_ui_consumer_topic		= L"";
wstring		m_ui_consumer_id		= L"";

static volatile sig_atomic_t run	= 1;

//---------------------------------------------------------------------------//
wstring utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}
//---------------------------------------------------------------------------//

//int add_to_log(string message) {
//
//	time_t now = time(NULL);
//	//char *str = asctime(localtime(&now));
//	tm now_tm = {};
//	char str[26] = {};
//	localtime_s(&now_tm, &now);
//	asctime_s(str, 26, &now_tm);
//
//	std::ofstream log("C:/kafka_log.txt", std::ios_base::app | std::ios_base::out);
//
//	log << str << message << "\n";
//	return 0;
//}

static void stop(int sig) {
	run = 0;
	fclose(stdin); /* abort fgets() */
}
static void dr_msg_cb(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage, void* opaque) {
	if (rkmessage->err){
		//fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
	}
	else {
		//fprintf(stderr, "%% Message delivered (%zd bytes, partition %i, offset %i)\n", rkmessage->len, rkmessage->partition, rkmessage->offset);
		m_ui_producer_offset	= rkmessage->offset;
		m_ui_producer_partition	= rkmessage->partition;

		//add_to_log( 
		//	"produced message: producer_offset:" + std::to_string(m_ui_producer_offset) +
		//	" -> producer_partition:" + std::to_string(m_ui_producer_partition)
		//);

	}
}

class Kafka {
public:
	rd_kafka_conf_t* conf;
	rd_kafka_t* rk;
	rd_kafka_topic_partition_list_t* subscription;
	rd_kafka_resp_err_t err;
	wstring	id;
	wstring topic;
	wstring message;
	wstring kafka_key;
	uint32_t partition;
	uint32_t offset;

	int init(char* servers, char* client_id, char* group_id, char* username, char* password) {  // Method/function defined inside the class
		char errstr[512];
		conf = rd_kafka_conf_new();

		if (rd_kafka_conf_set(conf, "bootstrap.servers", servers,
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%s\n", errstr);
			rd_kafka_conf_destroy(conf);
			return 1;
		}

		if (rd_kafka_conf_set(conf, "client.id", client_id,
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "group.id", group_id,
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "sasl.username", username,
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (rd_kafka_conf_set(conf, "sasl.password", password,
			errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
			fprintf(stderr, "%% %s\n", errstr);
			exit(1);
		}

		if (!(rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf,
			errstr, sizeof(errstr)))) {
			fprintf(stderr, "%% Failed to create new consumer: %s\n", errstr);
			exit(1);
		}
	}

	int init_kafka(char* topic) {
		rd_kafka_poll_set_consumer(rk);

		subscription = rd_kafka_topic_partition_list_new(1);

		rd_kafka_topic_partition_list_add(subscription,
			topic,
			/* the partition is ignored
			 * by subscribe() */
			RD_KAFKA_PARTITION_UA);


		err = rd_kafka_subscribe(rk, subscription);
		if (err) {
			fprintf(stderr,
				"%% Failed to subscribe to %d topics: %s\n",
				subscription->cnt, rd_kafka_err2str(err));
			rd_kafka_topic_partition_list_destroy(subscription);
			rd_kafka_destroy(rk);
			return 1;
		}
	}

	boolean get_message() {
		topic		= L"";
		message		= L"";
		kafka_key	= L"";
		partition	= -1;
		offset		= -1;

		rd_kafka_message_t* rkm = rd_kafka_consumer_poll(rk, 1000);
		if (!rkm) return false; // timeout: no message

		message = utf8_decode((char*)rkm->payload);
		string key = (char*)rkm->key;
		string sub_key = key.substr(0, rkm->key_len);
		kafka_key = utf8_decode(sub_key);

		string s_topic = rd_kafka_topic_name(rkm->rkt);

		topic = utf8_decode(s_topic);
		partition = rkm->partition;
		offset = rkm->offset;

		return true;
	}
};

std::vector< Kafka > arr;

boolean add_kafka_object(wstring id, char* servers, char* client_id, char* group_id, char* topic, char* username, char* password) {
	
	int pos = 0;
	for (Kafka& element : arr) // access by reference to avoid copying
	{	
		if (element.id == id) {
			arr.erase(arr.begin() + pos);
		}
		pos = pos + 1;
	}
	
	Kafka KafkaObj;
	KafkaObj.id = id;
	KafkaObj.init(servers, client_id, group_id, username, password);
	KafkaObj.init_kafka(topic);

	arr.push_back(KafkaObj);

	//KafkaObj;

	return true;
}

//�������� ������� in English
static const wchar_t* g_PropNames[] = {
	L"version", 
	L"milliseconds", 
	L"param_number", 
	L"param_string", 
	L"producer_offset",
	L"producer_partition",
	L"consumer_offset",
	L"consumer_partition",
	L"consumer_value",
	L"consumer_key",
	L"consumer_topic",
	L"consumer_timestamp",
	L"consumer_id"
};

//�������� ������� � ������� ������
static const wchar_t* g_PropNamesRu[] = {
	L"������", 
	L"�����������", 
	L"�����", 
	L"������", 
	L"producer_�����_���������",
	L"producer_��������_���������",
	L"consumer_�����_���������", 
	L"consumer_��������_���������", 
	L"consumer_�����_���������",
	L"consumer_����_���������",
	L"consumer_�����",
	L"consumer_����_���������",
	L"consumer_�������������"
};

//�������� ������� in English
static const wchar_t* g_MethodNames[] = {
	L"Delay", L"GetInfo", L"create_producer", L"create_consumer", L"produce", L"Consume", L"test", L"ProducerCreate", L"SendProducer", L"ConsumerCreate"};

//�������� ������� � ������� ������
static const wchar_t* g_MethodNamesRu[] = {
	L"Delay", L"GetInfo", L"����������������������", L"����������������������", L"��������������", L"�������������", L"test", L"ProducerCreate", L"SendProducer", L"ConsumerCreate"};

static const wchar_t g_kClassNames[]	= L"kafka"; 
static IAddInDefBase* pAsyncEvent		= NULL;

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
		*pInterface = new Ckafka_connect_native;
		return (long)* pInterface;
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

	for (Kafka& element : arr) // access by reference to avoid copying
	{
		if (element.rk) {
			/* Destroy the consumer */
			rd_kafka_consumer_close(element.rk);
			rd_kafka_destroy(element.rk);
		}
	}

	arr.resize(0);
	arr.shrink_to_fit();

	if (rk_producer) {
		/* Destroy the consumer */
		rd_kafka_destroy(rk_producer);
	}

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

// CAddInNative
//---------------------------------------------------------------------------//
Ckafka_connect_native::Ckafka_connect_native()
{
	m_iMemory = 0;
	m_iConnect = 0;
#if !defined( __linux__ ) && !defined(__APPLE__)
	m_hTimerQueue = 0;
#endif //__linux__
}
//---------------------------------------------------------------------------//
Ckafka_connect_native::~Ckafka_connect_native()
{
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long Ckafka_connect_native::GetInfo()
{
	// Component should put supported component technology version 
	// This component supports 2.0 version
	return 2000;
}
//---------------------------------------------------------------------------//
void Ckafka_connect_native::Done()
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
bool Ckafka_connect_native::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	const wchar_t* wsExtension = g_kClassNames;
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
long Ckafka_connect_native::GetNProps()
{
	// You may delete next lines and add your own implementation code here
	return ePropLast;
}
//---------------------------------------------------------------------------//
long Ckafka_connect_native::FindProp(const WCHAR_T* wsPropName)
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
const WCHAR_T* Ckafka_connect_native::GetPropName(long lPropNum, long lPropAlias)
{
	if (lPropNum >= ePropLast)
		return NULL;

	wchar_t*	wsCurrentName = NULL;
	WCHAR_T*	wsPropName = NULL;
	int			iActualSize = 0;

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
		if (m_iMemory->AllocMemory((void**)& wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::wstring_to_p(std::wstring str, tVariant* val) {
	char* t1;
	TV_VT(val) = VTYPE_PWSTR;
	m_iMemory->AllocMemory((void**)&t1, (str.length() + 1) * sizeof(WCHAR_T));
	memcpy(t1, str.c_str(), (str.length() + 1) * sizeof(WCHAR_T));
	val->pstrVal = t1;
	val->strLen = str.length();
	return true;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	switch (lPropNum)
	{
	case ePropVersion:
		wstring_to_p(version, pvarPropVal);
		break;
	case ePropMilliseconds:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_uiMilliseconds;
		break;
	case ePropNumber:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_ui_param_number;
		break;
	case ePropString:
		wstring_to_p(m_ui_param_string, pvarPropVal);
		break;
	case ePropConsumerValue:
		wstring_to_p(m_ui_consumer_value, pvarPropVal);
		break;	
	case ePropProducerOffset:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_ui_producer_offset;
		break;
	case ePropProducerPartition:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_ui_producer_partition;
		break;
	case ePropConsumerOffset:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_ui_consumer_offset;
		break;
	case ePropConsumerPartition:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_I4(pvarPropVal) = m_ui_consumer_partition;
		break;	
	case ePropConsumerKey:
		wstring_to_p(m_ui_consumer_key, pvarPropVal);
		break;
	case ePropConsumerTimeStamp:
		wstring_to_p(m_ui_consumer_timestamp, pvarPropVal);
		break;
	case ePropConsumerTopic:
		wstring_to_p(m_ui_consumer_topic, pvarPropVal);
		break;
	case ePropConsumerID:
		wstring_to_p(m_ui_consumer_id, pvarPropVal);
		break;
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::SetPropVal(const long lPropNum, tVariant* varPropVal)
{

	//auto test = TV_VT(varPropVal);
	wchar_t* wsTmp = nullptr;

	switch (lPropNum)
	{
	case ePropMilliseconds:
		if (TV_VT(varPropVal) != VTYPE_I4)
			return false;
		m_uiMilliseconds = TV_I4(varPropVal);
		break;
	case ePropNumber:
		if (TV_VT(varPropVal) != VTYPE_I4)
			return false;
		m_ui_param_number = TV_I4(varPropVal);
		break;
	case ePropString:
		if (TV_VT(varPropVal) != VTYPE_PWSTR)
			return false;
		::convFromShortWchar(&wsTmp, TV_WSTR(varPropVal));
		m_ui_param_string = wsTmp;
		break;
	case ePropConsumerValue:
		return false;
		break;
	case ePropConsumerKey:
		return false;
		break;
	case ePropProducerOffset:
		return false;
		break;
	case ePropProducerPartition:
		return false;
		break;
	case ePropConsumerOffset:
		return false;
		break;
	case ePropConsumerPartition:
		return false;
		break;
	case ePropConsumerTimeStamp:
		return false;
		break;
	case ePropConsumerTopic:
		return false;
		break;
	case ePropConsumerID:
		return false;
		break;
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::IsPropReadable(const long lPropNum)
{
	switch (lPropNum)
	{
	case ePropVersion:
		return true;
	case ePropMilliseconds:
		return true;
	case ePropNumber:
		return true;
	case ePropString:
		return true;	
	case ePropConsumerValue:
		return true;
	case ePropConsumerOffset:
		return true;
	case ePropConsumerPartition:
		return true;
	case ePropProducerOffset:
		return true;
	case ePropProducerPartition:
		return true;	
	case ePropConsumerKey:
		return true;
	case ePropConsumerTimeStamp:
		return true;
	case ePropConsumerTopic:
		return true;
	case ePropConsumerID:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::IsPropWritable(const long lPropNum)
{
	switch (lPropNum)
	{
	case ePropVersion:
		return false;
	case ePropMilliseconds:
		return true;
	case ePropNumber:
		return true;
	case ePropString:
		return true;
	case ePropConsumerValue:
		return false;	
	case ePropConsumerOffset:
		return false;	
	case ePropProducerOffset:
		return false;
	case ePropConsumerPartition:
		return false;
	case ePropProducerPartition:
		return false;
	case ePropConsumerKey:
		return false;	
	case ePropConsumerTopic:
		return false;	
	case ePropConsumerID:
		return false;
	case ePropConsumerTimeStamp:
		return false;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
long Ckafka_connect_native::GetNMethods()
{
	return eMethLast;
}
//---------------------------------------------------------------------------//
long Ckafka_connect_native::FindMethod(const WCHAR_T* wsMethodName)
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
const WCHAR_T* Ckafka_connect_native::GetMethodName(const long lMethodNum, const long lMethodAlias)
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
		if (m_iMemory->AllocMemory((void**)& wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}
//-----------------���������� ���������� �������-------------------------//
long Ckafka_connect_native::GetNParams(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eMethCreateProducer:
		return 3;
	case eMethProduce:
		return 3;
	case eMethCreateConsumer:
		return 6;
	case eMethConsume:
		return 1;
	case eMethSleep:
		return 1;
	case eMethTest:
		return 1;	
	case eMethProducerCreate:
		return 3;
	case eMethConsumerCreate:
		return 7;
	case eMethSendProducer:
		return 3;
	case eMethGetInfo:
		return 0;
	default:
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant* pvarParamDefValue)
{
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
		case eMethCreateProducer:
			return true;
			break;		
		case eMethCreateConsumer:
			return true;
			break;
		case eMethProduce:
			return true;
			break;
		case eMethConsume:
			return false;
			break;
		case eMethGetInfo:
			return false;
			break;
		case eMethSleep:
			return true;
			break;
		case eMethTest:
			return true;
			break;
		case eMethProducerCreate:
			return true;
			break;
		case eMethSendProducer:
			return true;
			break;
		case eMethConsumerCreate:
			return true;
			break;
		default:
			return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::HasRetVal(const long lMethodNum)
{
	switch (lMethodNum)
	{
		case eMethCreateProducer:
			return true;
		case eMethCreateConsumer:
			return true;
		case eMethProduce:
			return true;
		case eMethConsume:
			return false;
		case eMethGetInfo:
			return true;
		case eMethTest:
			return false;
		case eMethSleep:
			return false;
		case eMethProducerCreate:
			return false;
		case eMethSendProducer:
			return false;		
		case eMethConsumerCreate:
			return false;
		default:
			return false;
	}
	return false;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::CallAsProc(const long lMethodNum,tVariant* paParams, const long lSizeArray)
{
	wchar_t* cons_id		= L"";
	wchar_t* cons_clientid	= L"";
	wchar_t* cons_servers	= L"";
	wchar_t* cons_topic		= L"";
	wchar_t* coms_group_id	= L"";
	wchar_t* cons_login		= L"";
	wchar_t* cons_pass		= L"";
	wchar_t* cons_key		= L"";
	wchar_t* cons_message	= L"";
	bool result;

	switch (lMethodNum)
	{
	case eMethSleep: {
		tVariant& pParam0 = paParams[0];
		int amount = pParam0.intVal;
		::Sleep(amount);
		m_uiMilliseconds = amount;
		result = true;
		break;
	}
	case eMethProducerCreate: {

		if (!lSizeArray || !paParams) return false;

		cons_servers = (paParams)->pwstrVal;
		cons_login = (paParams + 1)->pwstrVal;
		cons_pass = (paParams + 2)->pwstrVal;

		result = swd_create_producer(_bstr_t(cons_servers), _bstr_t(cons_login), _bstr_t(cons_pass));

		//add_to_log("producer created -> login:" + ws2s(cons_login) +
		//	" -> pass:" + ws2s(cons_pass)
		//);

		break;
	}
	case eMethConsumerCreate: {

		if (!lSizeArray || !paParams) return false;
		
		cons_id						= (paParams)->pwstrVal;
		cons_clientid				= (paParams + 1)->pwstrVal;
		cons_servers				= (paParams + 2)->pwstrVal;
		cons_topic					= (paParams + 3)->pwstrVal;
		coms_group_id				= (paParams + 4)->pwstrVal;
		cons_login					= (paParams + 5)->pwstrVal;
		cons_pass					= (paParams + 6)->pwstrVal;

		//add_kafka_object(wchar_t* id, char* servers, char* client_id, char* group_id, char* topic, char* username, char* password) {
		result = add_kafka_object(cons_id, _bstr_t(cons_servers), _bstr_t(cons_clientid), _bstr_t(coms_group_id), _bstr_t(cons_topic), _bstr_t(cons_login), _bstr_t(cons_pass));
		//result = true;
		
		//add_to_log("consumer created -> clientid:" + ws2s(cons_clientid) + 
		//	" -> topic:" + ws2s(cons_topic) + 
		//	" -> group_id:" + ws2s(coms_group_id) + 
		//	" -> login:" + ws2s(cons_login) +
		//	" -> pass:" + ws2s(cons_pass)
		//);
		
		break;
	}
	case eMethSendProducer: {
		if (!lSizeArray || !paParams) return false;

		cons_topic = (paParams)->pwstrVal;
		cons_key = (paParams + 1)->pwstrVal;
		cons_message = (paParams + 2)->pwstrVal;

		//add_to_log("send producer -> cons_topic:" + ws2s(cons_topic) +
		//	" -> key:" + ws2s(cons_key) +
		//	" -> message:" + ws2s(cons_message)
		//);

		result = swd_produce(_bstr_t(cons_topic), cons_key, cons_message);

		break;
	}
	case eMethConsume: {
		if (!lSizeArray || !paParams) return false;

		cons_id = (paParams)->pwstrVal;

		result = swd_consume(cons_id);
		break;
	}
	default:
		return false;
	}
	return true;
	//return result;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	bool ret = false;

	wchar_t* cons_clientid	= L"";
	wchar_t* cons_servers	= L"";
	wchar_t* cons_topic		= L"";
	wchar_t* coms_group_id	= L"";
	wchar_t* cons_login		= L"";
	wchar_t* cons_pass		= L"";
	wchar_t* cons_key		= L"";
	wchar_t* cons_message	= L"";

	bool result;

	switch (lMethodNum)
	{
		case eMethCreateProducer:
			if (!lSizeArray || !paParams) return false;

			cons_servers	= (paParams)->pwstrVal;
			cons_login		= (paParams + 1)->pwstrVal;
			cons_pass		= (paParams + 2)->pwstrVal;

			result = swd_create_producer(_bstr_t(cons_servers),_bstr_t(cons_login),_bstr_t(cons_pass) );
			TV_VT(pvarRetValue) = VTYPE_BOOL;
			TV_BOOL(pvarRetValue) = result;

			ret = true;
			break;

		case eMethProduce:
			if (!lSizeArray || !paParams) return false;
			
			cons_topic		= (paParams)->pwstrVal;
			cons_key		= (paParams + 1)->pwstrVal;
			cons_message	= (paParams + 2)->pwstrVal;
			
			result = swd_produce(_bstr_t(cons_topic), cons_key, cons_message);
			TV_VT(pvarRetValue) = VTYPE_BOOL;
			TV_BOOL(pvarRetValue) = result;
			
			ret = true;
			break;

		case eMethGetInfo:
			//if (!lSizeArray || !paParams) return false;

			TV_VT(pvarRetValue)		= VTYPE_PWSTR;
			wstring_to_p(version, pvarRetValue);

			ret = true;
			break;

		//case eMethTest:
		//	//if (!lSizeArray || !paParams) return false;

		//	//result = swd_consume();
		//	TV_VT(pvarRetValue) = VTYPE_BOOL;
		//	TV_BOOL(pvarRetValue) = true;

		//	ret = true;
		//	break;
	}
	return ret;
}
//---------------------------------------------------------------------------//
// Convert a wide Unicode string to an UTF8 string
string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::swd_produce(char* topic, wchar_t* key, wchar_t* message) {
	signal(SIGINT, stop);
	rd_kafka_resp_err_t err;

	string enc_message	= utf8_encode(message);
	string enc_key		= utf8_encode(key);

retry:

	m_ui_producer_offset = -1;
	m_ui_producer_partition = -1;

	if (!rk_producer) {
		return false;
	}

	err = rd_kafka_producev(
		rk_producer,
		RD_KAFKA_V_TOPIC(topic),
		RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
		RD_KAFKA_V_KEY(enc_key.c_str(), enc_key.length()),
		RD_KAFKA_V_VALUE(enc_message.c_str(), enc_message.length()),
		RD_KAFKA_V_OPAQUE(NULL),
		RD_KAFKA_V_END);

	if (err) {
		//fprintf(stderr,	"%% Failed to produce to topic %s: %s\n", topic, rd_kafka_err2str(err));
		if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
			rd_kafka_poll(rk_producer, 1000/*block for max 1000ms*/);
			goto retry;
		}
	}
	else {
		//fprintf(stderr, "%% Enqueued message (%zd bytes) ""for topic %s\n",	len, topic);
	}

	rd_kafka_poll(rk_producer, 0/*non-blocking*/);

	//fprintf(stderr, "%% Flushing final messages..\n");
	rd_kafka_flush(rk_producer, 10 * 1000 /* wait for max 10 seconds */);

	if (rd_kafka_outq_len(rk_producer) > 0) return false; //fprintf(stderr, "%% %d message(s) were not delivered\n", rd_kafka_outq_len(rk_consumer));

	return true;
};
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::swd_create_producer(char* server, char* login, char* pass) {

	rd_kafka_conf_t* conf;
	char errstr[512];

	conf = rd_kafka_conf_new();
	if (rd_kafka_conf_set(conf, "bootstrap.servers", server,
		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		//fprintf(stderr, "%s\n", errstr);
		//return 1;
		return false;
	}

	if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		//fprintf(stderr, "%% %s\n", errstr);
		//exit(1);
		return false;
	}

	if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		//fprintf(stderr, "%% %s\n", errstr);
		//exit(1);
		return false;
	}

	if (rd_kafka_conf_set(conf, "sasl.username", login,
		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		//fprintf(stderr, "%% %s\n", errstr);
		//exit(1);
		return false;
	}

	if (rd_kafka_conf_set(conf, "sasl.password", pass,
		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
		//fprintf(stderr, "%% %s\n", errstr);
		//exit(1);
		return false;
	}

	rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

	rk_producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	if (!rk_producer) {
		//fprintf(stderr,
		//	"%% Failed to create new producer: %s\n", errstr);
		return false;
	}

	return true;
};
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::swd_create_consumer(char* client_id, char* server, char* topic, char* group_id, char* login, char* pass) {
	return true;
}
//---------------------------------------------------------------------------//
bool Ckafka_connect_native::swd_consume(wstring id) {

	m_ui_consumer_key		= L"";
	m_ui_consumer_value		= L"";
	m_ui_consumer_offset	= -1;
	m_ui_consumer_partition = -1;
	m_ui_consumer_topic		= L"";
	m_ui_consumer_id		= L"";

	for (Kafka& element : arr) // access by reference to avoid copying
	{
		if (element.id == id)
		{
			if (element.get_message()) {
				m_ui_consumer_key = element.kafka_key;
				m_ui_consumer_value = element.message;
				m_ui_consumer_offset = element.offset;
				m_ui_consumer_partition = element.partition;
				m_ui_consumer_topic = element.topic;
				m_ui_consumer_id = element.id;
			}
		}
	}

	return true;
};
//---------------------------------------------------------------------------//
wstring Ckafka_connect_native::s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

string Ckafka_connect_native::ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}
//---------------------------------------------------------------------------//
void Ckafka_connect_native::int64ToChar(char a[], int64_t* n) {
	memcpy(a, &n, 8);
}
//---------------------------------------------------------------------------//
// This code will work only on the client!
#if !defined( __linux__ ) && !defined(__APPLE__)
VOID CALLBACK MyTimerProc(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (!pAsyncEvent)
		return;
	DWORD dwTime = 0;
	wchar_t* who = L"ComponentNative", * what = L"Timer";

	wchar_t* wstime = new wchar_t[TIME_LEN];
	if (wstime)
	{
		wmemset(wstime, 0, TIME_LEN);
		time_t vtime;
		time(&vtime);
		::_ui64tow_s(vtime, wstime, TIME_LEN, 10);
		pAsyncEvent->ExternalEvent(who, what, wstime);
		delete[] wstime;
	}
}
#else
void MyTimerProc(int sig)
{
	if (!pAsyncEvent)
		return;

	WCHAR_T* who = 0, *what = 0, *wdata = 0;
	wchar_t* data = 0;
	time_t dwTime = time(NULL);

	data = new wchar_t[TIME_LEN];

	if (data)
	{
		wmemset(data, 0, TIME_LEN);
		swprintf(data, TIME_LEN, L"%ul", dwTime);
		::convToShortWchar(&who, L"ComponentNative");
		::convToShortWchar(&what, L"Timer");
		::convToShortWchar(&wdata, data);

		pAsyncEvent->ExternalEvent(who, what, wdata);

		delete[] who;
		delete[] what;
		delete[] wdata;
		delete[] data;
	}
}
#endif
//---------------------------------------------------------------------------//
void Ckafka_connect_native::SetLocale(const WCHAR_T* loc)
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
bool Ckafka_connect_native::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
void Ckafka_connect_native::addError(uint32_t wcode, const wchar_t* source,
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
long Ckafka_connect_native::findName(const wchar_t* names[], const wchar_t* name,
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
		* Dest = new WCHAR_T[len];

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
		succeed = iconv(cd, (char**)& tmpWChar, &f, (char**)& tmpShort, &t);
		iconv_close(cd);
		if (succeed != (size_t)-1)
			return (uint32_t)succeed;
	}
#endif //__linux__
	for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
	{
		*tmpShort = (WCHAR_T)* tmpWChar;
	}

	return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
	if (!len)
		len = getLenShortWcharStr(Source) + 1;

	if (!*Dest)
		* Dest = new wchar_t[len];

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
		succeed = iconv(cd, (char**)& tmpShort, &f, (char**)& tmpWChar, &t);
		iconv_close(cd);
		if (succeed != (size_t)-1)
			return (uint32_t)succeed;
	}
#endif //__linux__
	for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
	{
		*tmpWChar = (wchar_t)* tmpShort;
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
