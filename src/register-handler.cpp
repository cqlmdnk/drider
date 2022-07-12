#include <register-handler.h>
#include <types.h>

#include <thread>
namespace dbroker {
int RegisterHandler::execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_loop_func)(void *))
{
	drider::DriderTopic *topic;
	std::vector<drider::DriderTopic *>::iterator it;
	std::string topic_name = std::string(msg->get_topic_name());

	switch (msg->type()) {
	case drider::PAC_TYPE::REGS:
		/*
		 * Search through topics if it already exist
		 * If exists add new server
		 * If not construct a new topic and add to vector
		 *
		 */
		printf("Adding to regs\nTopic name : %s\n", msg->get_topic_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});
		if (it != topic_vec.end()) {
			printf("Topic is found\n");
			// found
			(*it)->add_new_pub_to_topic(std::string(msg->get_bin_name()));
			printf("Publlisher count on this topic : %ld\n", (*it)->publishers.size());
		} else {
			// not found
			printf("Topic is not found\n");
			topic = new drider::DriderTopic(std::string(msg->get_topic_name()));
			topic_vec.push_back(topic);
			std::thread topic_thread(topic_loop_func, (void *)topic);
			topic->add_new_pub_to_topic(std::string(msg->get_bin_name()));
			topic_thread.detach();
			printf("Publlisher count on this topic : %ld\n", topic->publishers.size());
		}
		printf("Topic Count : %ld\n", topic_vec.size());

		/* code */
		break;

	case drider::PAC_TYPE::SUBS:
		/*
		 * Search through topics if it exists
		 * If exists add client to client list
		 * If not do nothing
		 *
		 */
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});
		if (it != topic_vec.end()) {
			// found
			(*it)->add_new_sub_to_topic(std::string(msg->get_bin_name()));
		}
		/* code */
		break;
	case drider::PAC_TYPE::UNREG:
		/*
		 * Search through topics if it exists
		 * If exists, check server list
		 * If server list becomes zero with removal destroy topic
		 * If not remove server from server list
		 *
		 */
		/* code */
		break;
	case drider::PAC_TYPE::UNSUB:
		/*
		 * Search through client in topic
		 * Remove client from list
		 *
		 */
		/* code */
		break;

	default:
		break;
	}
	return 0;
}
} // namespace dbroker
