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
	}
	addr->sun_family = AF_UNIX;
	std::string path = std::string(bin_name) + this->name();
	snprintf(addr->sun_path, (strlen(path.c_str()) + 1), "%s", path.c_str());
	addr->sun_path[0] = '\0';
	return addr;
}

DriderPublisher *DriderTopic::add_new_pub_to_topic(std::string bin_name)
{
	DriderPublisher *publisher = new DriderPublisher(this->name(), bin_name);
	publisher->set_socket(construct_new_addr(bin_name));
	this->publishers.push_back(publisher);
	return publisher;
}

void DriderTopic::add_new_sub_to_topic(std::string bin_name)
{
	DriderSubscriber *subscriber = new DriderSubscriber(this->name(), bin_name);
	subscriber->set_socket(construct_new_addr(bin_name));
	this->subscribers.push_back(subscriber);
}
} // namespace drider