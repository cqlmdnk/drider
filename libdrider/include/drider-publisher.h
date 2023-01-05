#ifndef DRIDER_PUBLISHER_H
#define DRIDER_PUBLISHER_H
#include <drider-agent.h>
#include <drider-types.h>
#include <message.h>
#include <sys/un.h>
#include <log-abs.h>

#include <vector>
namespace drider {
class DriderPublisher : public DriderAgent {
      private:
      protected:
	void dispatcher(char *msg_buffer, size_t msg_buffer_size);

      public:
	DriderPublisher(/* args */);
	DriderPublisher(const std::string &topic_name, const std::string &bin_name);
	virtual ~DriderPublisher();

	int register_topic(const std::string &topic_name);
	int unregister_topic(const std::string &topic_name);
};

} // namespace drider

#endif