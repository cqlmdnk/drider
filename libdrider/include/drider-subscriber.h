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
	int _is_deleted = 0;

	void *callback_loop();
	int process_request(std::string topic_name, int type);

      public:
	int sock_fd;
	sockaddr_un *_sub_addr;

	DriderSubscriber();
	DriderSubscriber(std::string topic_name, std::string bin_name);
	virtual ~DriderSubscriber();

	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);

	int subscribe_topic(std::string topic_name);
	int unsubscribe_topic(std::string topic_name);
	int start();

	virtual void *recv_callback(char *data);
};

} // namespace drider
#endif
