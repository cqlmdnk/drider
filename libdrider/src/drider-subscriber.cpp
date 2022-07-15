#include <drider-subscriber.h>
#include <register-message.h>
#include <topics.h>

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
	printf("drider-subscriber - init object , topic:%s - bin:%s\n", topic_name.c_str(), bin_name.c_str());

	this->bin_name() = bin_name;
	this->sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

	printf("drider-subscriber - socket is initialized : %d\n", sock_fd);

	_sub_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	_sub_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_sub_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_sub_addr->sun_path[0] = '\0';
	printf("drider-subscriber - binding address to socket\n");
	bind(sock_fd, (struct sockaddr *)_sub_addr, sizeof(struct sockaddr_un));
	printf("drider-subscriber - object is successfully initiliazed\n");
}

DriderSubscriber::~DriderSubscriber()
{
	free(path);
	free(_sub_addr);
	close(sock_fd);
}

int DriderSubscriber::process_request(std::string topic_name, int type)
{
	struct sockaddr_un *serv_addr = (struct sockaddr_un *)malloc(sizeof(struct sockaddr_un));

	int broker_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	RegisterMessage *reg_msg = new RegisterMessage((char *)bin_name_.c_str(), type, (char *)topic_name.c_str());
	char buffer[reg_msg->get_size_of_vars()];
	reg_msg->set_bin_name(this->bin_name().c_str());
	printf("%s\n", reg_msg->get_bin_name());
	reg_msg->serialize(buffer);
	const char *path_ = BROKER_PATH;
	if (broker_socket == -1) {
		printf("SOCKET ERROR: Couldn't open broker socket\n");
		return -1;
	}

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
		perror("sending datagram message");
	}
	// printf("bytes sent : %d\n", n_sent);
	free(serv_addr);
	close(broker_socket);

	return 0;
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

void *DriderSubscriber::callback_loop()
{
	ssize_t n_read;
	char buffer[BUF_SIZE_1K];
	while ((n_read = recv(this->sock_fd, buffer, BUF_SIZE_1K, 0)) > 0) {
		this->recv_callback(buffer);
		printf("drider-subscriber - read %ld bytes from socket\n", n_read);
	}
	return nullptr;
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
} // namespace drider
