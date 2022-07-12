#ifndef DRIDER_SUBSCRIBER_H
#define DRIDER_SUBSCRIBER_H
#include <message.h>

#include <string>
#include <sys/un.h>
namespace drider {
class DriderSubscriber {
      private:
	std::string name;
	char *path;

      public:
	sockaddr_un *_sub_addr;
	DriderSubscriber(/* args */);
	~DriderSubscriber();
	void set_socket(sockaddr_un *addr);
	// virtual void dispatch_callback(Message msg) = 0;
};

} // namespace drider
#endif
