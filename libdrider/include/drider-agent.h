#ifndef DRIDER_AGENT_H
#define DRIDER_AGENT_H
#include <drider-helpers.h>
#include <register-message.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
namespace drider {
class DriderAgent {
      protected:
	std::string name;
	std::string bin_name_;

      public:
	int sock_fd;
	sockaddr_un *_sock_addr;

	DriderAgent();
	DriderAgent(const std::string &topic_name, const std::string &bin_name);
	virtual ~DriderAgent();

	std::string &bin_name();
	const std::string &bin_name() const;
	void set_socket(sockaddr_un *addr);

	int process_request(const std::string &topic_name, const int &type);
};
} // namespace drider
#endif