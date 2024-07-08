#include <iostream>
#include <librdkafka/rdkafkacpp.h>


int main() {
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    
    delete conf;
    delete tconf;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
