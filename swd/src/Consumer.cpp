#include "Consumer.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <librdkafka/rdkafka.h>
#include <iostream>
#include <ctime>
#include <locale>
#include <codecvt>

static volatile sig_atomic_t run = 1;

using std::string;

std::vector<std::u16string> Consumer::names = {
	AddComponent(u"AddInNative", []()
				 { return new Consumer; })
};

Consumer::Consumer()
{
	AddFunction(u"Consume", u"Consume", [&](VH p_brokers, VH p_topic, VH p_username, VH p_password, VH p_key, VH p_message)
				{ this->result = this->Consume(p_brokers, p_topic, p_username, p_password, p_key, p_message); });
}

/**
 * @brief Signal termination of program
 */
static void stop(int sig)
{
	run = 0;
	fclose(stdin); /* abort fgets() */
}

static void
dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
{
	// if (rkmessage->err)
	// fprintf(stderr, "%% Message delivery failed: %s\n",
	//		rd_kafka_err2str(rkmessage->err));
	// else
	// fprintf(stderr,
	//		"%% Message delivered (%zd bytes, "
	//		"partition %" PRId32 ")\n",
	//		rkmessage->len, rkmessage->partition);

	/* The rkmessage is destroyed automatically by librdkafka */
}

std::string Consumer::u16string_to_string(const std::u16string& u16str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.to_bytes(u16str);
}

std::u16string Consumer::Consume( const std::u16string &p_brokers,
											const std::u16string &p_topic,
											const std::u16string &p_username,
											const std::u16string &p_password,
											const std::u16string &p_key,	
											const std::u16string &p_message
											)
{
	rd_kafka_t *rk;		   /* Producer instance handle */
	rd_kafka_conf_t *conf; /* Temporary configuration object */
	char errstr[512];	   /* librdkafka API error reporting buffer */
	
	string l_brokers = u16string_to_string(p_brokers);
	string l_topic = u16string_to_string(p_topic);
	string l_username = u16string_to_string(p_username);
	string l_password = u16string_to_string(p_password);
	
	conf = rd_kafka_conf_new();

	if (rd_kafka_conf_set(conf, "bootstrap.servers", l_brokers.c_str(), errstr,
						  sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error bootstrap.servers");
	}

	if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
						  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error security.protocol");
	}

	if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
						  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error sasl.mechanism");
	}

	if (rd_kafka_conf_set(conf, "sasl.username", l_username.c_str(),
						  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error sasl.username");
	}

	if (rd_kafka_conf_set(conf, "sasl.password", l_password.c_str(),
						  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error sasl.password");
	}

	rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	if (!rk)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error Failed to create new producer");
	}

	rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

	signal(SIGINT, stop);

	rd_kafka_resp_err_t err;

	string l_key = u16string_to_string(p_key);
	string l_message = u16string_to_string(p_message);
	err = rd_kafka_producev(
		rk,
		RD_KAFKA_V_TOPIC(l_topic.c_str()),
		RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
		RD_KAFKA_V_KEY(const_cast<char *>(l_key.c_str()), l_key.size()),
		RD_KAFKA_V_VALUE(const_cast<char *>(l_message.c_str()), l_message.size()),
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
		return text + MB2WCHAR("Failed to produce to topic");
	}
	else
	{
		return text + MB2WCHAR("Persisted");
	}

	rd_kafka_poll(rk, 0 /*non-blocking*/);
	rd_kafka_flush(rk, 1000 /* wait for max 10 seconds */);
	rd_kafka_destroy(rk);

	return text + MB2WCHAR("Persisted");
}