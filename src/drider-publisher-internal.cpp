#include <drider-publisher-internal.h>
#include <register-message.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace drider {

DriderPublisherInt::DriderPublisherInt(std::string topic_name, std::string bin_name)
{
	SPDLOG_INFO("drider-publisher - init object , topic:{} - bin:{}", topic_name.c_str(), bin_name.c_str());
	this->bin_name() = bin_name;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		SPDLOG_ERROR("socket initialization failed");
	}
	SPDLOG_INFO("drider-publisher - socket is initialized : {}", sock_fd);
	_pub_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (_pub_addr == NULL) {
		SPDLOG_ERROR("socket address allocation failed");
	}
	_pub_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_pub_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_pub_addr->sun_path[0] = '\0';
	SPDLOG_INFO("drider-publisher - object is successfully initiliazed");
}

DriderPublisherInt::DriderPublisherInt()
{
}
DriderPublisherInt::~DriderPublisherInt()
{
	// std::vector<DriderTopic>::iterator ptr;
	//  for (ptr = this->topics.begin(); ptr < this->topics.end(); ptr++) {
	//  	this->unregister_topic((*ptr).name());
	//  }
	free(path);
	unlink(_pub_addr->sun_path);
	free(_pub_addr);
	close(sock_fd);
}

void DriderPublisherInt::set_socket(sockaddr_un *addr)
{
	this->_pub_addr = addr;
}

int DriderPublisherInt::deleted()
{
	return _is_deleted;
}

void DriderPublisherInt::delete_it()
{
	_is_deleted = 1;
}

std::string &DriderPublisherInt::bin_name()
{
	return bin_name_;
}
const std::string &DriderPublisherInt::bin_name() const
{
	return bin_name_;
}

} // namespace drider