#ifndef DRIDER_SUBSCRIBER_INTERNAL_H
#define DRIDER_SUBSCRIBER_INTERNAL_H
#include <message.h>
#include <log-abs.h>
#include <string>
#include <sys/un.h>
namespace drider {
class DriderSubscriberInt {
      private:
	std::string name;
	char *path;
	std::string bin_name_;
	int _is_deleted = 0;

      public:
	int sock_fd;
	sockaddr_un *_sub_addr;

	DriderSubscriberInt();
	DriderSubscriberInt(std::string topic_name, std::string bin_name);
	virtual ~DriderSubscriberInt();

	int deleted();
	void delete_it();
	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);
	int start();
};

} // namespace drider
#endif
