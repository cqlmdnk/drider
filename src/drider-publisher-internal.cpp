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

#include <drider-publisher-internal.h>
#include <register-message.h>
#include <topics.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace drider {

DriderPublisherInt::DriderPublisherInt(std::string topic_name, std::string bin_name)
{
	LOG_INFO("drider-publisher - init object , topic:{} - bin:{}", topic_name.c_str(), bin_name.c_str());
	this->bin_name() = bin_name;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		LOG_ERROR("{}", "socket initialization failed");
	}
	LOG_INFO("drider-publisher - socket is initialized : {}", sock_fd);
	_pub_addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (_pub_addr == NULL) {
		LOG_ERROR("{}", "socket address allocation failed");
	}
	_pub_addr->sun_family = AF_UNIX;
	std::string path = std::string(this->bin_name()) + topic_name;
	snprintf(_pub_addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	_pub_addr->sun_path[0] = '\0';
	LOG_INFO("{}", "drider-publisher - object is successfully initiliazed");
}

DriderPublisherInt::DriderPublisherInt()
{
}
DriderPublisherInt::~DriderPublisherInt()
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

void DriderPublisherInt::set_socket(sockaddr_un *addr)
{
	this->_pub_addr = addr;
}

int DriderPublisherInt::deleted()
{
	return _is_deleted;
}

void DriderPublisherInt::delete_it()
{
	_is_deleted = 1;
}

std::string &DriderPublisherInt::bin_name()
{
	return bin_name_;
}
const std::string &DriderPublisherInt::bin_name() const
{
	return bin_name_;
}

} // namespace drider