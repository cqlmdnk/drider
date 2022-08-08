#ifndef DRIDER_PUBLISHER_INTERNAL_H
#define DRIDER_PUBLISHER_INTERNAL_H
#include <drider-types.h>
#include <message.h>
#include <sys/un.h>

#include <vector>
namespace drider {
class DriderPublisherInt {
      private:
	std::string name;
	char *path;
	std::string bin_name_;
	int _is_deleted = 0;

      protected:
      public:
	int sock_fd;
	sockaddr_un *_pub_addr;

	DriderPublisherInt(/* args */);
	DriderPublisherInt(std::string topic_name, std::string bin_name);
	virtual ~DriderPublisherInt();

	int deleted();
	void delete_it();
	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);
};

} // namespace drider

#endif