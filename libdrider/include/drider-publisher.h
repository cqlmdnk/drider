#ifndef DRIDER_PUBLISHER_H
#define DRIDER_PUBLISHER_H
#include <message.h>
#include <sys/un.h>
#include <types.h>

#include <vector>
namespace drider {
class DriderPublisher {
      private:
	std::string name;
	char *path;
	std::string bin_name_;

      protected:
	void dispatcher(char *msg_buffer, size_t msg_buffer_size);

      public:
	int sock_fd;
	sockaddr_un *_pub_addr;

	DriderPublisher(/* args */);
	DriderPublisher(std::string topic_name, std::string bin_name);
	~DriderPublisher();

	int register_topic(std::string topic_name);
	int unregister_topic(std::string topic_name);
	int process_request(std::string topic_name, int type);
	void set_socket(sockaddr_un *addr);

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