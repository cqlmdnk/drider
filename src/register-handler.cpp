#include <register-handler.h>
#include <drider-publisher-internal.h>
#include <drider-subscriber-internal.h>
#include <spdlog/spdlog.h>
#include <drider-types.h>

#include <thread>
namespace dbroker {
int RegisterHandler::execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_start_func)(drider::DriderPublisherInt *, std::vector<drider::DriderSubscriberInt *> *))
{
	int ret = 0;
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
		SPDLOG_INFO("register-handler -  REG request for = {}", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			SPDLOG_INFO("register-handler - Adding to regs\nTopic name : {}", (*it)->name().c_str());
			SPDLOG_INFO("register-handler -  Topic is found\n size of subs = {}", (*it)->subscribers.size());
			// found
			drider::DriderPublisherInt *pub = (*it)->add_new_pub_to_topic(std::string(msg->get_bin_name()));
			std::thread topic_thread(topic_start_func, pub, &((*it)->subscribers));
			topic_thread.join();
			SPDLOG_INFO("register-handler -  Publlisher count on this topic : {}", (*it)->publishers.size());
		} else {
			// not found
			SPDLOG_INFO("register-handler -  Topic is not found");
			topic = new drider::DriderTopic(std::string(msg->get_topic_name()));
			topic_vec.push_back(topic);

			drider::DriderPublisherInt *pub = topic->add_new_pub_to_topic(std::string(msg->get_bin_name()));
			if (pub == nullptr) {
				ret = -1;
				SPDLOG_ERROR("Couldn't add new publisher to topic : {}", msg->get_bin_name());
				return ret;
			}
			std::thread topic_thread(topic_start_func, pub, &(topic->subscribers));
			topic_thread.join();

			SPDLOG_INFO("Publlisher count on this topic : {}", topic->publishers.size());
		}
		SPDLOG_INFO("register-handler -  Topic Count : {}", topic_vec.size());

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
			SPDLOG_INFO("pubs count in this topic = {}", (*it)->publishers.size());
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
		SPDLOG_INFO("register-handler -  UNREG request for = {}", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			SPDLOG_INFO("register-handler -  Found topic for publisher removal. Looking for publisher in vector...");

			std::vector<drider::DriderPublisherInt *>::iterator it_pub;

			it_pub = find_if((*it)->publishers.begin(), (*it)->publishers.end(), [&msg_bin_name](const drider::DriderPublisherInt *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_pub != (*it)->publishers.end()) {
				SPDLOG_INFO("register-handler -  Found publisher, deleting...");
				(*it)->publishers.erase(std::remove((*it)->publishers.begin(), (*it)->publishers.end(), (*it_pub)), (*it)->publishers.end());
				(*it_pub)->delete_it();
			}
			SPDLOG_INFO("register-handler -  Publlisher count on this topic : {}", (*it)->publishers.size());
		}

		break;
	case drider::PAC_TYPE::UNSUB:
		SPDLOG_INFO("register-handler -  UNREG request for = {}", msg->get_bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			SPDLOG_INFO("register-handler -  Found topic for publisher removal. Looking for publisher in vector...");

			std::vector<drider::DriderSubscriberInt *>::iterator it_sub;

			it_sub = find_if((*it)->subscribers.begin(), (*it)->subscribers.end(), [&msg_bin_name](const drider::DriderSubscriberInt *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_sub != (*it)->subscribers.end()) {
				SPDLOG_INFO("register-handler -  Found publisher, deleting...");
				(*it)->subscribers.erase(std::remove((*it)->subscribers.begin(), (*it)->subscribers.end(), (*it_sub)), (*it)->subscribers.end());
				(*it_sub)->delete_it();
			}
			SPDLOG_INFO("register-handler -  Subscribers count on this topic : {}", (*it)->subscribers.size());
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
