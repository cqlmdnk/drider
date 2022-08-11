#ifndef DRIDER_AGENT_H
#define DRIDER_AGENT_H
#include <register-message.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
namespace drider {
class DriderAgent {
      protected:
	std::string name;
	char *path;
	std::string bin_name_;

      public:
	int sock_fd;
	sockaddr_un *_sock_addr;

	DriderAgent();
	DriderAgent(std::string topic_name, std::string bin_name);
	virtual ~DriderAgent();

	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);

	int process_request(std::string topic_name, int type);
};
} // namespace drider
#endif