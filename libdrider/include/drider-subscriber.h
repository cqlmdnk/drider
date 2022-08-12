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
	DriderSubscriber(const std::string &topic_name, const std::string &bin_name);
	virtual ~DriderSubscriber();

	int subscribe_topic(const std::string &topic_name);
	int unsubscribe_topic(const std::string &topic_name);
	int start();

	virtual void *recv_callback(char *data);
};

} // namespace drider
#endif
