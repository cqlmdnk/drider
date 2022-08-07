#ifndef DRIDER_PUBLISHER_H
#define DRIDER_PUBLISHER_H
#include <drider-types.h>
#include <message.h>
#include <sys/un.h>

#include <vector>
namespace drider {
class DriderPublisher {
      private:
	std::string name;
	char *path;
	std::string bin_name_;
	int _is_deleted = 0;

      protected:
	void dispatcher(char *msg_buffer, size_t msg_buffer_size);

      public:
	int sock_fd;
	sockaddr_un *_pub_addr;

	DriderPublisher(/* args */);
	DriderPublisher(std::string topic_name, std::string bin_name);
	virtual ~DriderPublisher();

	int deleted();
	void delete_it();
	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);

	int register_topic(std::string topic_name);
	int unregister_topic(std::string topic_name);
	int process_request(std::string topic_name, int type);
};

} // namespace drider

#endif