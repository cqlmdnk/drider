/*
Copyright (c) 2022 Berkan Baris Tezcan (berkan.baris.tezcan@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <drider-agent.h>
namespace drider {
DriderAgent::DriderAgent()
{
}
DriderAgent::DriderAgent(const std::string &topic_name, const std::string &bin_name)
{
	LOG_INFO("drider-agent - init object , topic:{} - bin:{}", topic_name.c_str(), bin_name.c_str());
	this->bin_name() = bin_name;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		LOG_ERROR("{}", "socket initialization failed");
	}
	LOG_INFO("drider-agent - socket is initialized : {}", sock_fd);
	_sock_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (_sock_addr == NULL) {
		LOG_ERROR("{}", "socket address allocation failed");
	}
	_sock_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_sock_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_sock_addr->sun_path[0] = '\0';
	LOG_INFO("{}", "drider-agent - object is successfully initiliazed");
}

DriderAgent::~DriderAgent()
{
	unlink(_sock_addr->sun_path);
	free(_sock_addr);
	close(sock_fd);
}

int DriderAgent::process_request(const std::string &topic_name, const int &type)
{
	int ret = 0;
	struct sockaddr_un serv_addr;
	struct sockaddr_un cli_addr;
	;

	int broker_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (broker_socket < 0) {
		LOG_ERROR("{}", "broker_socket initialization failed");
		ret = -1;
		return ret;
	}
	RegisterMessage reg_msg = RegisterMessage(type, (char *)bin_name_.c_str(), (char *)topic_name.c_str());
	char buffer[reg_msg.get_size_of_vars()];
	reg_msg.bin_name(this->bin_name().c_str());
	LOG_INFO("%s\n", reg_msg.bin_name());
	reg_msg.serialize(buffer);
	const char *path_ = BROKER_PATH;

	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_UNIX;
	snprintf(serv_addr.sun_path, (strlen(path_) + 1), "%s", path_);
	serv_addr.sun_path[0] = '\0';
	LOG_INFO("%s\n", reg_msg.bin_name());

	memset(&cli_addr, 0, sizeof(struct sockaddr_un));
	cli_addr.sun_family = AF_UNIX;
	snprintf(cli_addr.sun_path, (strlen(this->bin_name().c_str()) + 1), "%s", this->bin_name().c_str());
	cli_addr.sun_path[0] = '\0';

	// char *buffer_ptr;
	// buffer_ptr = buffer;
	// for (int len = 0; len < reg_msg->get_size_of_vars(); len++) {
	// 	printf("%02x", *(buffer_ptr+len));
	// }
	// printf("\n");

	if (bind(broker_socket, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr_un)) < 0) {
		LOG_ERROR("{}", "socket bind failed");
	}

	int n_sent = 0;
	if ((n_sent = sendto(broker_socket, buffer, reg_msg.get_size_of_vars(), 0,
			     (sockaddr *)&serv_addr, sizeof(struct sockaddr_un))) < 0) {
		LOG_ERROR("{}", "sending datagram message");
		ret = -1;
	}
	int n_recv = 0;
	if ((n_recv = read(broker_socket, buffer, reg_msg.get_size_of_vars())) < 0) {
		LOG_ERROR("{}", "receiving datagram message");
		ret = -1;
	}
	reg_msg.deserialize(buffer);
	if (reg_msg.type() == drider::PAC_TYPE::ACK) {
		ret = 0;
	} else {
		ret = -1;
	}

	// printf("bytes sent : %d\n", n_sent);
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
