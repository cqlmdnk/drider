#ifndef DRIDER_TOPIC_H
#define DRIDER_TOPIC_H
#include <drider-publisher-internal.h>
#include <drider-subscriber-internal.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include <log-abs.h>

namespace drider {
class DriderTopic {
      private:
	/* data */
	std::string name_;

      public:
	std::vector<DriderPublisherInt *> publishers;
	std::vector<DriderSubscriberInt *> subscribers;

	DriderTopic(std::string name);
	~DriderTopic();

	std::string &name();
	const std::string &name() const;

	sockaddr_un *construct_new_addr(std::string bin_name);
	DriderPublisherInt *add_new_pub_to_topic(std::string bin_name);
	void add_new_sub_to_topic(std::string bin_name);
	bool is_pub_exist(std::string bin_name);
};

} // namespace drider
#endif