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

	LOG_INFO("drider-subscriber - socket is initialized : {}", sock_fd);

	if (bind(sock_fd, (struct sockaddr *)_sock_addr, sizeof(struct sockaddr_un)) < 0) {
		LOG_ERROR("{}", "socket bind failed");
	}

	LOG_INFO("{}", "drider-subscriber - object is successfully initiliazed");
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
				LOG_INFO("{}", "drider-subscriber - errno == EAGAIN) || (errno == EWOULDBLOCK)");
				// delete this;
				return nullptr;
			} else {
				continue;
			}
		}
		this->recv_callback(buffer);
		LOG_INFO("drider-subscriber - read {} bytes from socket", n_read);
	}
	return nullptr;
}

} // namespace drider
