#include <register-handler.h>
#include <types.h>

#include <thread>
namespace dbroker {
int RegisterHandler::execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_start_func)(drider::DriderPublisher *, std::vector<drider::DriderSubscriber *> *))
{
	drider::DriderTopic *topic;
	std::vector<drider::DriderTopic *>::iterator it;
	std::string topic_name = std::string(msg->get_topic_name());
	std::string msg_bin_name = std::string(msg->get_bin_name());

	switch (msg->type()) {
	case drider::PAC_TYPE::REGS:
		/*
		 * Search through topics if it already exist
		 * If exists add new pub
		 * If not construct a new topic and add to vector
		 *
		 */
		// printf("register-handler - Adding to regs\nTopic name : %s\n", msg->get_topic_name());
		printf("register-handler -  REG request for = %s\n", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			printf("register-handler - Adding to regs\nTopic name : %s\n", (*it)->name().c_str());
			printf("register-handler -  Topic is found\n size of subs = %ld\n", (*it)->subscribers.size());
			// found
			drider::DriderPublisher *pub = (*it)->add_new_pub_to_topic(std::string(msg->get_bin_name()));
			std::thread topic_thread(topic_start_func, pub, &((*it)->subscribers));
			topic_thread.join();
			printf("register-handler -  Publlisher count on this topic : %ld\n", (*it)->publishers.size());
		} else {
			// not found
			printf("register-handler -  Topic is not found\n");
			topic = new drider::DriderTopic(std::string(msg->get_topic_name()));
			topic_vec.push_back(topic);

			drider::DriderPublisher *pub = topic->add_new_pub_to_topic(std::string(msg->get_bin_name()));

			std::thread topic_thread(topic_start_func, pub, &(topic->subscribers));
			topic_thread.join();

			printf("Publlisher count on this topic : %ld\n", topic->publishers.size());
		}
		printf("register-handler -  Topic Count : %ld\n", topic_vec.size());

		break;

	case drider::PAC_TYPE::SUBS:
		/*
		 * Search through topics if it exists
		 * If exists add sub to sub list
		 * If not do nothing
		 *
		 */
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});
		printf("%s\n", topic_name.c_str());

		if (it != topic_vec.end()) {
			// found
			printf("pubs count in this topic = %ld\n", (*it)->publishers.size());
			(*it)->add_new_sub_to_topic(std::string(msg->get_bin_name()));
		}
		/* code */
		break;
	case drider::PAC_TYPE::UNREG:
		/*
		 * Search through topics if it exists
		 * If exists, check pub list
		 * If pub list becomes zero with removal destroy topic
		 * If not remove pub from pub list
		 *
		 */
		printf("register-handler -  UNREG request for = %s\n", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			printf("register-handler -  Found topic for publisher removal. Looking for publisher in vector...\n");

			std::vector<drider::DriderPublisher *>::iterator it_pub;

			it_pub = find_if((*it)->publishers.begin(), (*it)->publishers.end(), [&msg_bin_name](const drider::DriderPublisher *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_pub != (*it)->publishers.end()) {
				printf("register-handler -  Found publisher, deleting...\n");
				(*it)->publishers.erase(std::remove((*it)->publishers.begin(), (*it)->publishers.end(), (*it_pub)), (*it)->publishers.end());
				(*it_pub)->delete_it();
			}
			printf("register-handler -  Publlisher count on this topic : %ld\n", (*it)->publishers.size());
		}

		break;
	case drider::PAC_TYPE::UNSUB:
		printf("register-handler -  UNREG request for = %s\n", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			printf("register-handler -  Found topic for publisher removal. Looking for publisher in vector...\n");

			std::vector<drider::DriderSubscriber *>::iterator it_sub;

			it_sub = find_if((*it)->subscribers.begin(), (*it)->subscribers.end(), [&msg_bin_name](const drider::DriderSubscriber *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_sub != (*it)->subscribers.end()) {
				printf("register-handler -  Found publisher, deleting...\n");
				(*it)->subscribers.erase(std::remove((*it)->subscribers.begin(), (*it)->subscribers.end(), (*it_sub)), (*it)->subscribers.end());
				(*it_sub)->delete_it();
			}
			printf("register-handler -  Subscribers count on this topic : %ld\n", (*it)->subscribers.size());
			/*
			 * Search through sub in topic
			 * Remove sub from list
			 *
			 */
			/* code */
		}
		break;

	default:
		break;
	}
	return 0;
}
} // namespace dbroker
  // namespace dbroker
