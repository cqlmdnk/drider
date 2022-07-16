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

	int subscribe_topic(std::string topic_name);
	int unsubscribe_topic(std::string topic_name);
	void set_socket(sockaddr_un *addr);
	int start();

	virtual void *recv_callback(char *data)
	{
		return nullptr;
	}
	int deleted()
	{
		return _is_deleted;
	}
	void delete_it()
	{
		_is_deleted = 1;
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
