#include <drider-agent.h>
namespace drider {
DriderAgent::DriderAgent()
{
}
DriderAgent::DriderAgent(std::string topic_name, std::string bin_name)
{
	SPDLOG_INFO("drider-agent - init object , topic:{} - bin:{}", topic_name.c_str(), bin_name.c_str());
	this->bin_name() = bin_name;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		SPDLOG_ERROR("socket initialization failed");
	}
	SPDLOG_INFO("drider-agent - socket is initialized : {}", sock_fd);
	_sock_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (_sock_addr == NULL) {
		SPDLOG_ERROR("socket address allocation failed");
	}
	_sock_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_sock_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_sock_addr->sun_path[0] = '\0';
	SPDLOG_INFO("drider-agent - object is successfully initiliazed");
}

DriderAgent::~DriderAgent()
{
	free(path);
	unlink(_sock_addr->sun_path);
	free(_sock_addr);
	close(sock_fd);
}

int DriderAgent::process_request(std::string topic_name, int type)
{
	int ret = 0;
	struct sockaddr_un *serv_addr = (struct sockaddr_un *)calloc(1, sizeof(struct sockaddr_un));
	if (serv_addr == NULL) {
		SPDLOG_ERROR("serv_addr allocation failed");
	}

	int broker_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (broker_socket < 0) {
		SPDLOG_ERROR("broker_socket initialization failed");
		free(serv_addr);
		ret = -1;
		return ret;
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
		ret = -1;
	}
	// int n_recv = 0;
	// if ((n_recv = read(broker_socket, buffer, reg_msg->get_size_of_vars())) < 0) {
	// 	SPDLOG_ERROR("receiving datagram message");
	// 	ret = -1;
	// }
	// reg_msg->deserialize(buffer);
	// if (reg_msg->type() == drider::PAC_TYPE::ACK) {
	// 	ret = 0;
	// } else {
	// 	ret = -1;
	// }

	// printf("bytes sent : %d\n", n_sent);
	delete reg_msg;
	free(serv_addr);
	close(broker_socket);
	return ret;
}

std::string &DriderAgent::bin_name()
{
	return bin_name_;
}
const std::string &DriderAgent::bin_name() const
{
	return bin_name_;
}

void DriderAgent::set_socket(sockaddr_un *addr)
{
	this->_sock_addr = addr;
}
} // namespace drider
