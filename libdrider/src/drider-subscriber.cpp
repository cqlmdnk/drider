#include <drider-subscriber.h>

#include <thread>

namespace drider {

DriderSubscriber::DriderSubscriber()
{
}

DriderSubscriber::DriderSubscriber(const std::string &topic_name, const std::string &bin_name)
    : DriderAgent::DriderAgent(topic_name, bin_name)
{

	// struct timeval tv;
	// tv.tv_sec = 2;
	// tv.tv_usec = 0;
	// setsockopt(this->sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

	SPDLOG_INFO("drider-subscriber - socket is initialized : {}", sock_fd);

	if (bind(sock_fd, (struct sockaddr *)_sock_addr, sizeof(struct sockaddr_un)) < 0) {
		SPDLOG_ERROR("socket bind failed");
	}

	SPDLOG_INFO("drider-subscriber - object is successfully initiliazed");
}

DriderSubscriber::~DriderSubscriber()
{
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
	ASSERT_NOT_REACHED();
	return nullptr;
}

int DriderSubscriber::subscribe_topic(const std::string &topic_name)
{
	return this->process_request(topic_name, 0);
}

int DriderSubscriber::unsubscribe_topic(const std::string &topic_name)
{
	return this->process_request(topic_name, 2);
}

void *DriderSubscriber::callback_loop()
{
	ssize_t n_read;
	char buffer[BUF_SIZE_1K];

	while (1) {

		n_read = recv(this->sock_fd, buffer, BUF_SIZE_1K, 0);
		if (n_read == -1) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				SPDLOG_INFO("drider-subscriber - errno == EAGAIN) || (errno == EWOULDBLOCK)");
				// delete this;
				return nullptr;
			} else {
				continue;
			}
		}
		this->recv_callback(buffer);
		SPDLOG_INFO("drider-subscriber - read {} bytes from socket", n_read);
	}
	return nullptr;
}

} // namespace drider
