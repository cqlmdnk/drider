/*
Copyright (c) 2022 Berkan Baris Tezcan (berkan.baris.tezcan@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <drider-publisher-internal.h>
#include <drider-subscriber-internal.h>
#include <drider-types.h>
#include <register-handler.h>

#include <thread>
namespace dbroker {
int RegisterHandler::execute_request(drider::RegisterMessage *msg, std::vector<drider::DriderTopic *> &topic_vec, void *(*topic_start_func)(drider::DriderPublisherInt *, std::vector<drider::DriderSubscriberInt *> *))
{
	int ret = 0;
	drider::DriderTopic *topic;
	std::vector<drider::DriderTopic *>::iterator it;
	std::string topic_name = std::string(msg->topic_name());
	std::string msg_bin_name = std::string(msg->bin_name());

	switch (msg->type()) {
	case drider::PAC_TYPE::REGS:
		/*
		 * Search through topics if it already exist
		 * If exists add new pub
		 * If not construct a new topic and add to vector
		 *
		 */
		// printf("register-handler - Adding to regs\nTopic name : %s\n", msg->get_topic_name());
		LOG_INFO("register-handler -  REG request for = {}", msg->bin_name());
		it = std::find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			LOG_INFO("register-handler - Adding to regs\nTopic name : {}", (*it)->name().c_str());
			LOG_INFO("register-handler -  Topic is found\n size of subs = {}", (*it)->subscribers.size());
			// found
			if ((*it)->is_pub_exist(std::string(msg->bin_name()))) {
				LOG_WARN("Publisher exists in this topic : {}", msg->bin_name());
				return ret;
			}
			drider::DriderPublisherInt *pub = (*it)->add_new_pub_to_topic(std::string(msg->bin_name()));
			std::thread topic_thread(topic_start_func, pub, &((*it)->subscribers));
			topic_thread.join();
			LOG_INFO("register-handler -  Publlisher count on this topic : {}", (*it)->publishers.size());
		} else {
			// not found
			LOG_INFO("{}", "register-handler -  Topic is not found");
			topic = new drider::DriderTopic(std::string(msg->topic_name()));
			topic_vec.push_back(topic);

			drider::DriderPublisherInt *pub = topic->add_new_pub_to_topic(std::string(msg->bin_name()));
			if (pub == nullptr) {
				ret = -1;
				LOG_ERROR("Couldn't add new publisher to topic : {}", msg->bin_name());
				return ret;
			}
			std::thread topic_thread(topic_start_func, pub, &(topic->subscribers));
			topic_thread.join();

			LOG_INFO("Publlisher count on this topic : {}", topic->publishers.size());
		}
		LOG_INFO("register-handler -  Topic Count : {}", topic_vec.size());

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
			LOG_INFO("pubs count in this topic = {}", (*it)->publishers.size());
			(*it)->add_new_sub_to_topic(std::string(msg->bin_name()));
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
		LOG_INFO("register-handler -  UNREG request for = {}", msg->bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			LOG_INFO("{}", "register-handler -  Found topic for publisher removal. Looking for publisher in vector...");

			std::vector<drider::DriderPublisherInt *>::iterator it_pub;

			it_pub = find_if((*it)->publishers.begin(), (*it)->publishers.end(), [&msg_bin_name](const drider::DriderPublisherInt *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_pub != (*it)->publishers.end()) {
				LOG_INFO("{}", "register-handler -  Found publisher, deleting...");
				(*it)->publishers.erase(std::remove((*it)->publishers.begin(), (*it)->publishers.end(), (*it_pub)), (*it)->publishers.end());
				(*it_pub)->delete_it();
			}
			LOG_INFO("register-handler -  Publlisher count on this topic : {}", (*it)->publishers.size());
		}

		break;
	case drider::PAC_TYPE::UNSUB:
		LOG_INFO("register-handler -  UNREG request for = {}", msg->bin_name());
		it = find_if(topic_vec.begin(), topic_vec.end(), [&topic_name](const drider::DriderTopic *obj) {
			return obj->name() == topic_name;
		});

		if (it != topic_vec.end()) {
			LOG_INFO("{}", "register-handler -  Found topic for publisher removal. Looking for publisher in vector...");

			std::vector<drider::DriderSubscriberInt *>::iterator it_sub;

			it_sub = find_if((*it)->subscribers.begin(), (*it)->subscribers.end(), [&msg_bin_name](const drider::DriderSubscriberInt *obj) {
				return obj->bin_name() == msg_bin_name;
			});

			if (it_sub != (*it)->subscribers.end()) {
				LOG_INFO("{}", "register-handler -  Found publisher, deleting...");
				(*it)->subscribers.erase(std::remove((*it)->subscribers.begin(), (*it)->subscribers.end(), (*it_sub)), (*it)->subscribers.end());
				(*it_sub)->delete_it();
			}
			LOG_INFO("register-handler -  Subscribers count on this topic : {}", (*it)->subscribers.size());
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
