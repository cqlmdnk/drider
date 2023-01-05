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

#include <drider-publisher.h>

namespace drider {

DriderPublisher::DriderPublisher(const std::string &topic_name, const std::string &bin_name)
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
	LOG_INFO("{}", "Trying to send from dispatcher.");
	if ((n_sent = sendto(sock_fd, msg_buffer, msg_buffer_size, 0,
			     (sockaddr *)_sock_addr, sizeof(struct sockaddr_un))) < 0) {
		LOG_ERROR("{}", "sending datagram message");
	}
	LOG_INFO("{}", "Sent from dispatcher.");
	/*
	 *Send given buffer to brokers reserved socket path
	 */
	// char buffer[msg.get_size_of_vars()];
	// msg.serialize(buffer);
}

int DriderPublisher::register_topic(const std::string &topic_name)
{
	return this->process_request(topic_name, 1);
}

int DriderPublisher::unregister_topic(const std::string &topic_name)
{
	return this->process_request(topic_name, 3);
}

} // namespace drider
