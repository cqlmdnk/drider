#include <drider-subscriber-internal.h>
#include <register-message.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

namespace drider {

DriderSubscriberInt::DriderSubscriberInt()
{
}

DriderSubscriberInt::DriderSubscriberInt(std::string topic_name, std::string bin_name)
{
	SPDLOG_INFO("drider-subscriber - init object , topic:{} - bin:{}", topic_name.c_str(), bin_name.c_str());

	this->bin_name() = bin_name;
	this->sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		SPDLOG_ERROR("socket initialization failed");
	}
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(this->sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

	SPDLOG_INFO("drider-subscriber - socket is initialized : {}", sock_fd);

	_sub_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (_sub_addr == NULL) {
		SPDLOG_ERROR("socket address allocation failed");
	}
	_sub_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_sub_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_sub_addr->sun_path[0] = '\0';
	SPDLOG_INFO("drider-subscriber - binding address to socket");
#ifndef BROKER
	if (bind(sock_fd, (struct sockaddr *)_sub_addr, sizeof(struct sockaddr_un)) < 0) {
		SPDLOG_ERROR("socket bind failed");
	}
#endif
	SPDLOG_INFO("drider-subscriber - object is successfully initiliazed");
}

DriderSubscriberInt::~DriderSubscriberInt()
{
	free(path);
	unlink(_sub_addr->sun_path);
	free(_sub_addr);
	close(sock_fd);
}

int DriderSubscriberInt::deleted()
{
	return _is_deleted;
}
void DriderSubscriberInt::delete_it()
{
	_is_deleted = 1;
}
std::string &DriderSubscriberInt::bin_name()
{
	return bin_name_;
}
const std::string &DriderSubscriberInt::bin_name() const
{
	return bin_name_;
}

void DriderSubscriberInt::set_socket(sockaddr_un *addr)
{
	this->_sub_addr = addr;
}

} // namespace drider