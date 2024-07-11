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
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    
    delete conf;
    delete tconf;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
