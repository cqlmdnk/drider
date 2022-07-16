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
	DriderTopic(std::string name)
	{
		this->name() = name;
	}
	~DriderTopic()
	{
	}
	std::string &name()
	{
		return name_;
	}
	const std::string &name() const
	{
		return name_;
	}
	sockaddr_un *construct_new_addr(std::string bin_name)
	{
		sockaddr_un *addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
		addr->sun_family = AF_UNIX;
		std::string path = std::string(bin_name) + this->name();
		snprintf(addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
		addr->sun_path[0] = '\0';
		return addr;
	}
	DriderPublisher *add_new_pub_to_topic(std::string bin_name)
	{
		DriderPublisher *publisher = new DriderPublisher(this->name(), bin_name);
		publisher->set_socket(construct_new_addr(bin_name));
		this->publishers.push_back(publisher);
		return publisher;
	}
	void add_new_sub_to_topic(std::string bin_name)
	{
		DriderSubscriber *subscriber = new DriderSubscriber(this->name(), bin_name);
		subscriber->set_socket(construct_new_addr(bin_name));
		this->subscribers.push_back(subscriber);
	}
};

} // namespace drider
#endif