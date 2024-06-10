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
	AddFunction(u"Consume", u"Consume", [&](VH p_brokers, VH p_topic,VH p_group, VH p_username, VH p_password, VH p_key, VH p_message)
				{ this->result = this->Consume(p_brokers, p_topic, p_group, p_username, p_password, p_key, p_message); });
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

std::u16string Consumer::Consume( 			const std::u16string &p_brokers,
											const std::u16string &p_topic,
											const std::u16string &p_group,
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
	string l_group = u16string_to_string(p_group);
	string l_username = u16string_to_string(p_username);
	string l_password = u16string_to_string(p_password);
	
	conf = rd_kafka_conf_new();

	if (rd_kafka_conf_set(conf, "bootstrap.servers", l_brokers.c_str(), errstr,
						  sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error bootstrap.servers");
	}

	if (rd_kafka_conf_set(conf, "group.id", l_group.c_str(), errstr,
                              sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error group.id");
    }

	if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr,
                              sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error auto.offset.reset");
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

	//swd
	
	return text + MB2WCHAR(l_brokers);
}