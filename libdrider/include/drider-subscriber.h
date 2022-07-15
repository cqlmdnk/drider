#ifndef DRIDER_SUBSCRIBER_H
#define DRIDER_SUBSCRIBER_H
#include <message.h>

#include <string>
#include <sys/un.h>
namespace drider {
class DriderSubscriber {
      private:
	std::string name;
	char *path;
	std::string bin_name_;
	void *callback_loop();
	int process_request(std::string topic_name, int type);

      public:
	int sock_fd;
	sockaddr_un *_sub_addr;
	DriderSubscriber();
	DriderSubscriber(std::string topic_name, std::string bin_name);
	~DriderSubscriber();

	int subscribe_topic(std::string topic_name);
	int unsubscribe_topic(std::string topic_name);
	void set_socket(sockaddr_un *addr);
	int start();
	virtual void *recv_callback(char *data)
	{
		return nullptr;
	}

	std::string &bin_name()
	{
		return bin_name_;
	}
	const std::string &bin_name() const
	{
		return bin_name_;
	}
};

} // namespace drider
#endif
