#include <drider-publisher.h>

namespace drider {

DriderPublisher::DriderPublisher(std::string topic_name, std::string bin_name)
    : DriderAgent::DriderAgent(topic_name, bin_name)
{
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
}

void DriderPublisher::dispatcher(char *msg_buffer, size_t msg_buffer_size)
{
	int n_sent = 0;
	SPDLOG_INFO("Trying to send from dispatcher.");
	if ((n_sent = sendto(sock_fd, msg_buffer, msg_buffer_size, 0,
			     (sockaddr *)_sock_addr, sizeof(struct sockaddr_un))) < 0) {
		SPDLOG_ERROR("sending datagram message");
	}
	SPDLOG_INFO("Sent from dispatcher.");
	/*
	 *Send given buffer to brokers reserved socket path
	 */
	// char buffer[msg.get_size_of_vars()];
	// msg.serialize(buffer);
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
