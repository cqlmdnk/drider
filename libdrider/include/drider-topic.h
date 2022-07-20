#ifndef DRIDER_TOPIC_H
#define DRIDER_TOPIC_H
#include <drider-publisher.h>
#include <drider-subscriber.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
namespace drider {
class DriderTopic {
      private:
	/* data */
	std::string name_;

      public:
	std::vector<DriderPublisher *> publishers;
	std::vector<DriderSubscriber *> subscribers;

	DriderTopic(std::string name);
	~DriderTopic();

	std::string &name();
	const std::string &name() const;

	sockaddr_un *construct_new_addr(std::string bin_name);
	DriderPublisher *add_new_pub_to_topic(std::string bin_name);
	void add_new_sub_to_topic(std::string bin_name);
};

} // namespace drider
#endif