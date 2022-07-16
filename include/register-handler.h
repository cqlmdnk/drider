#ifndef REGISTER_HANDLER_H
#define REGISTER_HANDLER_H
#include <register-message.h>
#include <vector>
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
	int execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_loop_func)(drider::DriderPublisher *, std::vector<drider::DriderSubscriber *> *));
};

} // namespace dbroker

#endif