#include <drider-subscriber.h>
namespace drider {

DriderSubscriber::DriderSubscriber(/* args */)
{
}

DriderSubscriber::~DriderSubscriber()
{
}

void DriderSubscriber::set_socket(sockaddr_un *addr)
{
	this->_sub_addr = addr;
}
} // namespace drider
