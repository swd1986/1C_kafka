#include <iostream>
#include <csignal>
#include <stdexcept>
#include <vector>
#include <librdkafka/rdkafkacpp.h>

static bool run = true;

static void sigterm(int sig) {
	run = false;
}

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
public:
	void rebalance_cb(RdKafka::KafkaConsumer* consumer,
		RdKafka::ErrorCode err,
		std::vector<RdKafka::TopicPartition*>& partitions) {
		std::cerr << "Rebalance callback: " << RdKafka::err2str(err) << std::endl;
		consumer->assign(partitions);
	}
};

int main() {
	try {

		std::string brokers = "DC1TMSGBRKR01:9092,DC2TMSGBRKR01:9092,DC3TMSGBRKR01:9092"; // Kafka broker address
		std::string topic = "test";
		std::string group = "test3";
		std::string username = "svc.mdm";
		std::string password = "pahNg0uv0xohgh6k";

		std::string errstr;
		RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
		RdKafka::Conf* tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

		// Set bootstrap servers
		if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error("Error setting bootstrap servers: " + errstr);
		}

		// Set consumer group
		if (conf->set("group.id", group, errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error("Error setting group ID: " + errstr);
		}

		if (conf->set("security.protocol", "SASL_PLAINTEXT", errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error(errstr);
		}

		if (conf->set("sasl.mechanism", "PLAIN", errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error(errstr);
		}

		if (conf->set("sasl.username", username, errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error(errstr);
		}

		if (conf->set("sasl.password", password, errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error(errstr);
		}
		
		// Set rebalance callback (optional)
		ExampleRebalanceCb ex_rebalance_cb;
		if (conf->set("rebalance_cb", &ex_rebalance_cb, errstr) != RdKafka::Conf::CONF_OK) {
			throw std::runtime_error("Error setting rebalance callback: " + errstr);
		}

		// Create consumer instance
		RdKafka::KafkaConsumer* consumer = RdKafka::KafkaConsumer::create(conf, errstr);
		if (!consumer) {
			throw std::runtime_error("Failed to create consumer: " + errstr);
		}

		std::vector<std::string> topics = { topic };
		RdKafka::ErrorCode err = consumer->subscribe(topics);
		if (err) {
			throw std::runtime_error("Failed to subscribe to " + topic + ": " + RdKafka::err2str(err));
		}

		std::cout << "Hello, World!" << std::endl;
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return 1;
	}
}
