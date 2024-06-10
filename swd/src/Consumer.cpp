﻿#include "Consumer.h"
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
	AddComponent(u"Consumer", []()
				 { return new Consumer; }),
};

Consumer::Consumer()
{
	AddProperty(
		u"get_message", u"get_message",
		[&](VH var)
		{ var = this->get_message(); });
	AddFunction(u"Consume", u"Consume", [&](VH p_brokers, VH p_topic, VH p_group, VH p_username, VH p_password)
				{ this->result = this->Consume(p_brokers, p_topic, p_group, p_username, p_password); });
}

std::string Consumer::get_message()
{
	return MB2WCHAR(this->message);
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

std::string Consumer::u16string_to_string(const std::u16string &u16str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	return converter.to_bytes(u16str);
}

std::u16string Consumer::Consume(const std::u16string &p_brokers,
								 const std::u16string &p_topic,
								 const std::u16string &p_group,
								 const std::u16string &p_username,
								 const std::u16string &p_password)
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
						  sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error group.id");
	}

	if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr,
						  sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
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

	rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
	if (!rk)
	{
		rd_kafka_conf_destroy(conf);
		return text + MB2WCHAR("error Failed to create new consumer");
	}

	conf = NULL;
	rd_kafka_poll_set_consumer(rk);
	rd_kafka_topic_partition_list_t *subscription;
	subscription = rd_kafka_topic_partition_list_new(1);

	rd_kafka_topic_partition_list_add(subscription,
									  l_topic.c_str(),
									  /* the partition is ignored
									   * by subscribe() */
									  RD_KAFKA_PARTITION_UA);
	rd_kafka_resp_err_t err;
	err = rd_kafka_subscribe(rk, subscription);
	if (err)
	{
		rd_kafka_topic_partition_list_destroy(subscription);
		rd_kafka_destroy(rk);
		return text + MB2WCHAR("error Failed to subscribe to topic");
	}

	rd_kafka_message_t *rkm = rd_kafka_consumer_poll(rk, 5000);
	if (!rkm)
		return MB2WCHAR("timeout"); // timeout: no message

	if (rkm->err)
	{
		rd_kafka_message_destroy(rkm);
		return MB2WCHAR("Consumer error"); // timeout: no message
	}
	rd_kafka_message_destroy(rkm);
	rd_kafka_consumer_close(rk);
	rd_kafka_destroy(rk);



	return text + MB2WCHAR("true");
}