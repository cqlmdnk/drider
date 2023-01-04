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

#include <drider-topic.h>
#include <spdlog/spdlog.h>
namespace drider {
DriderTopic::DriderTopic(std::string name)
{
	this->name() = name;
}
DriderTopic::~DriderTopic()
{
}
std::string &DriderTopic::name()
{
	return name_;
}
const std::string &DriderTopic::name() const
{
	return name_;
}

sockaddr_un *DriderTopic::construct_new_addr(std::string bin_name)
{
	sockaddr_un *addr = (sockaddr_un *)calloc(1, sizeof(sockaddr_un));
	if (addr == NULL) {
		SPDLOG_ERROR("socket address allocation failed");
		return NULL;
	}
	addr->sun_family = AF_UNIX;
	std::string path = std::string(bin_name) + this->name();
	snprintf(addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	addr->sun_path[0] = '\0';
	return addr;
}

DriderPublisherInt *DriderTopic::add_new_pub_to_topic(std::string bin_name)
{
	DriderPublisherInt *publisher = new DriderPublisherInt(this->name(), bin_name);
	sockaddr_un *addr = construct_new_addr(bin_name);
	if (addr == NULL) {
		return nullptr;
	}
	publisher->set_socket(addr);
	this->publishers.push_back(publisher);
	return publisher;
}

void DriderTopic::add_new_sub_to_topic(std::string bin_name)
{
	DriderSubscriberInt *subscriber = new DriderSubscriberInt(this->name(), bin_name);
	subscriber->set_socket(construct_new_addr(bin_name));
	this->subscribers.push_back(subscriber);
}

bool DriderTopic::is_pub_exist(std::string bin_name)
{
	return this->publishers.end() != std::find_if(std::begin(this->publishers), std::end(this->publishers), [&] (DriderPublisherInt * const& p) { return p->bin_name().compare(bin_name) == 0; });
}
} // namespace drider