#ifndef REGISTER_HANDLER_H
#define REGISTER_HANDLER_H
#include <drider-topic.h>
#include <register-message.h>
#include <vector>
#include <log-abs.h>
namespace dbroker {
class RegisterHandler {
      private:
	/* data */
      public:
	RegisterHandler(/* args */)
	{
	}
	~RegisterHandler()
	{
	}
	int execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_loop_func)(drider::DriderPublisherInt *, std::vector<drider::DriderSubscriberInt *> *));
};

} // namespace dbroker

#endif