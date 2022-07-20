#include <drider-publisher.h>
#include <register-message.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace drider {

DriderPublisher::DriderPublisher(std::string topic_name, std::string bin_name)
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

DriderPublisher::DriderPublisher()
{
}
DriderPublisher::~DriderPublisher()
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

void DriderPublisher::set_socket(sockaddr_un *addr)
{
	this->_pub_addr = addr;
}

void DriderPublisher::dispatcher(char *msg_buffer, size_t msg_buffer_size)
{
	int n_sent = 0;
	if ((n_sent = sendto(sock_fd, msg_buffer, msg_buffer_size, 0,
			     (sockaddr *)_pub_addr, sizeof(struct sockaddr_un))) < 0) {
		SPDLOG_ERROR("sending datagram message");
	}
	/*
	 *Send given buffer to brokers reserved socket path
	 */
	// char buffer[msg.get_size_of_vars()];
	// msg.serialize(buffer);
}

int DriderPublisher::process_request(std::string topic_name, int type)
{
	struct sockaddr_un *serv_addr = (struct sockaddr_un *)calloc(1, sizeof(struct sockaddr_un));
	if (serv_addr == NULL) {
		SPDLOG_ERROR("serv_addr allocation failed");
	}

	int broker_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (broker_socket < 0) {
		SPDLOG_ERROR("broker_socket initialization failed");
	}
	RegisterMessage *reg_msg = new RegisterMessage((char *)bin_name_.c_str(), type, (char *)topic_name.c_str());
	char buffer[reg_msg->get_size_of_vars()];
	reg_msg->set_bin_name(this->bin_name().c_str());
	printf("%s\n", reg_msg->get_bin_name());
	reg_msg->serialize(buffer);
	const char *path_ = BROKER_PATH;

	serv_addr->sun_family = AF_UNIX;
	snprintf(serv_addr->sun_path, (strlen(path_) + 1), "%s", path_);
	serv_addr->sun_path[0] = '\0';
	printf("%s\n", reg_msg->get_bin_name());

	// char *buffer_ptr;
	// buffer_ptr = buffer;
	// for (int len = 0; len < reg_msg->get_size_of_vars(); len++) {
	// 	printf("%02x", *(buffer_ptr+len));
	// }
	// printf("\n");

	int n_sent = 0;
	if ((n_sent = sendto(broker_socket, buffer, reg_msg->get_size_of_vars(), 0,
			     (sockaddr *)serv_addr, sizeof(struct sockaddr_un))) < 0) {
		SPDLOG_ERROR("sending datagram message");
	}
	// printf("bytes sent : %d\n", n_sent);
	free(serv_addr);
	close(broker_socket);
	return 0;
}

int DriderPublisher::register_topic(std::string topic_name)
{
	return this->process_request(topic_name, 1);
}

int DriderPublisher::unregister_topic(std::string topic_name)
{
	return this->process_request(topic_name, 3);
}

int DriderPublisher::deleted()
{
	return _is_deleted;
}

void DriderPublisher::delete_it()
{
	_is_deleted = 1;
}

std::string &DriderPublisher::bin_name()
{
	return bin_name_;
}
const std::string &DriderPublisher::bin_name() const
{
	return bin_name_;
}

} // namespace drider