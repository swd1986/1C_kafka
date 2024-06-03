#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <librdkafka/rdkafka.h>
static volatile sig_atomic_t run = 1;

using std::string;

// static void stop(int sig)
// {
// 	run = 0;
// 	fclose(stdin); /* abort fgets() */
// }

// static void
// dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
// {
// 	if (rkmessage->err)
// 		fprintf(stderr, "%% Message delivery failed: %s\n",
// 				rd_kafka_err2str(rkmessage->err));
// 	else
// 		fprintf(stderr,
// 				"%% Message delivered (%zd bytes, "
// 				"partition %" PRId32 ")\n",
// 				rkmessage->len, rkmessage->partition);

// 	/* The rkmessage is destroyed automatically by librdkafka */
// }

int main()
{
	rd_kafka_t *rk;		   /* Producer instance handle */
	rd_kafka_conf_t *conf; /* Temporary configuration object */
	char errstr[512];	   /* librdkafka API error reporting buffer */
	char buf[512];		   /* Message value temporary buffer */
	const char *brokers;   /* Argument: broker list */
	const char *topic;	   /* Argument: topic to produce to */

	brokers = "DC1TMSGBRKR01:9092,DC2TMSGBRKR01:9092,DC3TMSGBRKR01:9092";
	topic = "test";

	conf = rd_kafka_conf_new();

	//     if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr,
	//                               sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	//         fprintf(stderr, "%s\n", errstr);
	//         return 1;
	//     }

	//     if (rd_kafka_conf_set(conf, "security.protocol", "SASL_PLAINTEXT",
	// 		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	// 		//fprintf(stderr, "%% %s\n", errstr);
	// 		//exit(1);
	// 	    return false;
	// 	}

	// 	if (rd_kafka_conf_set(conf, "sasl.mechanism", "PLAIN",
	// 		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	// 		//fprintf(stderr, "%% %s\n", errstr);
	// 		//exit(1);
	// 		return false;
	// 	}

	// 	if (rd_kafka_conf_set(conf, "sasl.username", "svc.mdm",
	// 		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	// 		//fprintf(stderr, "%% %s\n", errstr);
	// 		//exit(1);
	// 		return false;
	// 	}

	// 	if (rd_kafka_conf_set(conf, "sasl.password", "pahNg0uv0xohgh6k",
	// 		errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
	// 		//fprintf(stderr, "%% %s\n", errstr);
	// 		//exit(1);
	// 		return false;
	// 	}

	//     rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	//         if (!rk) {
	//                 fprintf(stderr, "%% Failed to create new producer: %s\n",
	//                         errstr);
	//                 return 1;
	//     }

	//     rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

	//     rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	//         if (!rk) {
	//                 fprintf(stderr, "%% Failed to create new producer: %s\n",
	//                         errstr);
	//                 return 1;
	//     }

	//         /* Signal handler for clean shutdown */
	//     signal(SIGINT, stop);

	//     rd_kafka_resp_err_t err;

	//     string t = "fuck";

	//   err = rd_kafka_producev(
	// 				rk,
	// 				RD_KAFKA_V_TOPIC(topic),
	// 				RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
	// 				RD_KAFKA_V_KEY(const_cast<char *>(t.c_str()), t.size()),
	// 				RD_KAFKA_V_VALUE(const_cast<char *>(t.c_str()), t.size()),
	// 				RD_KAFKA_V_OPAQUE(NULL),
	// 				RD_KAFKA_V_END);

	// 			if (err) {
	// 				//fprintf(stderr,	"%% Failed to produce to topic %s: %s\n", topic, rd_kafka_err2str(err));
	// 				if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
	// 					return false;
	// 					//rd_kafka_poll(rk_producer, 1000/*block for max 1000ms*/);
	// 					//goto retry;
	// 				}
	// 			}
	// 			else {
	// 				//fprintf(stderr, "%% Enqueued message (%zd bytes) ""for topic %s\n",	len, topic);
	// 			}

	// 			rd_kafka_poll(rk, 500/*non-blocking*/);

	// 			//fprintf(stderr, "%% Flushing final messages..\n");
	// 			rd_kafka_flush(rk, 1000 /* wait for max 10 seconds */);

	// 			//if (rd_kafka_outq_len(rk_producer) > 0) return false; //fprintf(stderr, "%% %d message(s) were not delivered\n", rd_kafka_outq_len(rk_consumer));

	// 			rd_kafka_destroy(rk);

	return 0;
}