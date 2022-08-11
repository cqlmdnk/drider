#ifndef DRIDER_SUBSCRIBER_H
#define DRIDER_SUBSCRIBER_H
#include <drider-agent.h>
#include <message.h>

#include <string>
#include <sys/un.h>
namespace drider {
class DriderSubscriber : public DriderAgent {
      private:
      public:
	void *callback_loop();

	DriderSubscriber();
	DriderSubscriber(std::string topic_name, std::string bin_name);
	virtual ~DriderSubscriber();

	int subscribe_topic(std::string topic_name);
	int unsubscribe_topic(std::string topic_name);
	int start();

	virtual void *recv_callback(char *data);
};

} // namespace drider
#endif
