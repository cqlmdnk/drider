#include <drider-publisher.h>
#include <register-message.h>
#include <topics.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace drider {

DriderPublisher::DriderPublisher(/* args */)
{
	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
}
DriderPublisher::~DriderPublisher()
{
	// std::vector<DriderTopic>::iterator ptr;
	//  for (ptr = this->topics.begin(); ptr < this->topics.end(); ptr++) {
	//  	this->unregister_topic((*ptr).name());
	//  }
	free(path);
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
		perror("sending datagram message");
	}
	/*
	 *Send given buffer to brokers reserved socket path
	 */
	// char buffer[msg.get_size_of_vars()];
	// msg.serialize(buffer);
}

int DriderPublisher::process_request(std::string topic_name, int type)
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
} // namespace drider