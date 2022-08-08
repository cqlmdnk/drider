#include <drider-subscriber.h>
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

DriderSubscriber::DriderSubscriber()
{
}

DriderSubscriber::DriderSubscriber(std::string topic_name, std::string bin_name)
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

DriderSubscriber::~DriderSubscriber()
{
	free(path);
	unlink(_sub_addr->sun_path);
	free(_sub_addr);
	close(sock_fd);
}

int DriderSubscriber::start()
{
	int ret = 0;
	std::thread callback_loop_t([this] {
		callback_loop();
	});
	callback_loop_t.detach();
	return ret;
}
void *DriderSubscriber::recv_callback(char *data)
{
	return nullptr;
}
std::string &DriderSubscriber::bin_name()
{
	return bin_name_;
}
const std::string &DriderSubscriber::bin_name() const
{
	return bin_name_;
}

int DriderSubscriber::subscribe_topic(std::string topic_name)
{
	return this->process_request(topic_name, 0);
}

int DriderSubscriber::unsubscribe_topic(std::string topic_name)
{
	return this->process_request(topic_name, 2);
}

void DriderSubscriber::set_socket(sockaddr_un *addr)
{
	this->_sub_addr = addr;
}

int DriderSubscriber::process_request(std::string topic_name, int type)
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
	SPDLOG_ERROR("size of class vars {}", reg_msg->get_size_of_vars());
	int n_sent = 0;
	if ((n_sent = sendto(broker_socket, buffer, reg_msg->get_size_of_vars(), 0,
			     (sockaddr *)serv_addr, sizeof(struct sockaddr_un))) < 0) {
		printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
		perror("sending datagram message");
	}
	// printf("bytes sent : %d\n", n_sent);
	free(serv_addr);
	close(broker_socket);

	return 0;
}

void *DriderSubscriber::callback_loop()
{
	ssize_t n_read;
	char buffer[BUF_SIZE_1K];

	while (1) {

		n_read = recv(this->sock_fd, buffer, BUF_SIZE_1K, 0);
		if (n_read == -1) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				delete this;
				return nullptr;
			} else {
				continue;
			}
		}
		this->recv_callback(buffer);
		SPDLOG_INFO("drider-subscriber - read {} bytes from socket\n", n_read);
	}
	return nullptr;
}

} // namespace drider
