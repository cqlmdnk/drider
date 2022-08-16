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

#include <drider-main.h>

#include <drider-publisher-internal.h>
#include <drider-subscriber-internal.h>
#include <drider-types.h>
#include <main.h>
#include <register-handler.h>
#include <spdlog/spdlog.h>
#include <topics.h>

#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

//#define POLICY_CUT_THE_CORD
//#define POLICY_RETRY

pthread_mutex_t lock;
dbroker::RegisterHandler register_handler;
std::vector<drider::DriderTopic *> topic_vec;

void *publisher_loop_func(drider::DriderPublisherInt *publisher, std::vector<drider::DriderSubscriberInt *> *subscribers)
{

	char buffer[BUF_SIZE_1K];

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(publisher->sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
	SPDLOG_INFO("In publisher loop func init.");
	if (bind(publisher->sock_fd, (struct sockaddr *)publisher->_pub_addr, sizeof(struct sockaddr_un))) {
		SPDLOG_ERROR("binding name to datagram socket");
		exit(1);
	}
	/* Read from the socket */
	ssize_t n_read = 0;

	while (1) {
		n_read = recv(publisher->sock_fd, buffer, BUF_SIZE_1K, 0);
		if (n_read == -1) {
			if (((errno == EAGAIN) || (errno == EWOULDBLOCK)) && publisher->deleted()) {
				delete publisher;
				return nullptr;
			} else {
				continue;
			}
		}

		pthread_mutex_lock(&lock);
		std::vector<drider::DriderSubscriberInt *>::iterator it = subscribers->begin();

		for (; it != subscribers->end(); it++) {
			ssize_t n_sent;
			if ((n_sent = sendto(publisher->sock_fd, buffer, n_read, 0, (struct sockaddr *)(*it)->_sub_addr, sizeof(struct sockaddr_un))) < 0) {

				SPDLOG_ERROR("cant sent to subscriber");
				(*it)->delete_it();
#if defined(POLICY_CUT_THE_CORD)
				SPDLOG_ERROR("omit&flush");
				// TODO
				// will be defined
				(*it)->
#elif defined(POLICY_RETRY)
				SPDLOG_ERROR("retrying");
				// TODO
				// will be defined
#else
				SPDLOG_ERROR("moving on");
				// will do nothing
#endif
			}
		}
		pthread_mutex_unlock(&lock);
	}

	return nullptr;
}

void *topic_start_func(drider::DriderPublisherInt *pub_param, std::vector<drider::DriderSubscriberInt *> *subs_param)
{
	SPDLOG_INFO("drider broker - topic loop func started");
	SPDLOG_INFO("subs_param size = {}", subs_param->size());

	std::thread pub_thread(publisher_loop_func, pub_param, subs_param);
	pub_thread.detach();

	return nullptr;
}

void *listener(void *param)
{
	int sock = 0, ret = 0;
	struct sockaddr_un *addr = (struct sockaddr_un *)calloc(1, sizeof(struct sockaddr_un));
	struct sockaddr_un *cli_addr = (struct sockaddr_un *)calloc(1, sizeof(struct sockaddr_un));
	drider::RegisterMessage *reg_msg;
	char buffer[reg_msg->get_size_of_vars()];
	const char *path_ = BROKER_PATH;
	SPDLOG_INFO("drider-broker - register socket is open");

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		SPDLOG_ERROR("opening datagram socket");
		exit(1);
	}
	addr->sun_family = AF_UNIX;
	snprintf(addr->sun_path, (strlen(path_) + 1), "%s", path_);
	addr->sun_path[0] = '\0';
	SPDLOG_INFO("drider-broker - register socket binding name to datagram socket");
	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_un))) {
		SPDLOG_ERROR("binding name to datagram socket");
		exit(1);
	}
	/* Read from the socket */
	int n_read = 0;
	socklen_t sock_len = sizeof(struct sockaddr_un);
	SPDLOG_INFO("drider-broker - started to receive from register socket");

	while ((n_read = recvfrom(sock, buffer, reg_msg->get_size_of_vars(), 0, (struct sockaddr *)cli_addr, &sock_len)) > 0) {

		// char *buffer_ptr;
		// buffer_ptr = buffer;
		// for (int len = 0; len < reg_msg->get_size_of_vars(); len++) {
		// 	printf("%02x", *(buffer_ptr+len));
		// }
		// printf("\n");
		// printf("bytes sent : %d\n", n_read);

		reg_msg = new drider::RegisterMessage();
		reg_msg->deserialize(buffer);
		printf("%s\n", reg_msg->get_bin_name());
		pthread_mutex_lock(&lock);
		// add clie address to topic vector
		SPDLOG_INFO("drider-broker - register request is received, executing request");

		ret = register_handler.execute_request(reg_msg, topic_vec, topic_start_func);
		if (ret == -1) {
			SPDLOG_ERROR("drider-broker - error while executing request");
			reg_msg->set_type(drider::PAC_TYPE::ERR);
		} else {
			reg_msg->set_type(drider::PAC_TYPE::ACK);
		}
		reg_msg->serialize(buffer);
		if (sendto(sock, buffer, reg_msg->get_size_of_vars(), 0, (struct sockaddr *)cli_addr, sock_len) < 0) {
			SPDLOG_ERROR("drider-broker - error while sending response message to client");
			perror("cant send data\n");
		}
		pthread_mutex_unlock(&lock);
		SPDLOG_INFO("{}\n", reg_msg->get_bin_name());
		delete reg_msg;
	}

	close(sock);

	return nullptr;
}

int main(int argc, char **argv)
{
	int param = 0;
	if (pthread_mutex_init(&lock, NULL) != 0) {
		SPDLOG_ERROR("mutex init failed");
		return 1;
	}
	register_handler = dbroker::RegisterHandler();
	listener((void *)&param);

	return 0;
}