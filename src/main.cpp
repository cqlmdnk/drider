#include <drider-main.h>
#include <main.h>
#include <register-handler.h>
#include <topics.h>

#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>

pthread_mutex_t lock;
dbroker::RegisterHandler register_handler;
std::vector<drider::DriderTopic *> topic_vec;

void *publisher_loop_func(drider::DriderPublisher *publisher, std::vector<drider::DriderSubscriber *> *subscribers)
{

	char buffer[BUF_SIZE_1K];
	if (bind(publisher->sock_fd, (struct sockaddr *)publisher->_pub_addr, sizeof(struct sockaddr_un))) {
		perror("binding name to datagram socket");
		exit(1);
	}
	/* Read from the socket */
	ssize_t n_read = 0;

	while ((n_read = recv(publisher->sock_fd, buffer, BUF_SIZE_1K, 0)) > 0) {
		std::vector<drider::DriderSubscriber *>::iterator it = subscribers->begin();

		for (; it != subscribers->end(); it++) {
			ssize_t n_sent;
			if ((n_sent = sendto(publisher->sock_fd, buffer, n_read, 0, (struct sockaddr *)(*it)->_sub_addr, sizeof(struct sockaddr_un))) < 0) {

				perror("cant sent to subscriber ");
			}
		}
	}

	return nullptr;
}

void *topic_start_func(void *param)
{
	printf("drider broker - topic loop func started\n");
	drider::DriderTopic *topic = (drider::DriderTopic *)param;
	std::vector<drider::DriderPublisher *>::iterator it = topic->publishers.begin();
	printf("drider broker - iterator is initiliazed\n");
	for (; it != topic->publishers.end(); it++) {

		std::thread pub_thread(publisher_loop_func, (*it), &topic->subscribers);
		pub_thread.detach();
	}
	return nullptr;
}

void *listener(void *param)
{
	int sock = 0;
	struct sockaddr_un *addr = (struct sockaddr_un *)malloc(sizeof(struct sockaddr_un));
	struct sockaddr_un *cli_addr = (struct sockaddr_un *)malloc(sizeof(struct sockaddr_un));
	drider::RegisterMessage *reg_msg;
	char buffer[reg_msg->get_size_of_vars()];
	const char *path_ = BROKER_PATH;
	printf("drider-broker - register socket is open\n");

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("opening datagram socket");
		exit(1);
	}
	addr->sun_family = AF_UNIX;
	snprintf(addr->sun_path, (strlen(path_) + 1), "%s", path_);
	addr->sun_path[0] = '\0';
	printf("drider-broker - register socket binding name to datagram socket\n");
	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr_un))) {
		perror("binding name to datagram socket");
		exit(1);
	}
	/* Read from the socket */
	int n_read = 0;
	socklen_t sock_len = sizeof(struct sockaddr_un);
	printf("drider-broker - started to receive from register socket\n");

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
		printf("drider-broker - register request is received, executing request\n");

		register_handler.execute_request(reg_msg, topic_vec, topic_start_func);
		pthread_mutex_unlock(&lock);
		printf("%s\n", reg_msg->get_bin_name());
	}

	close(sock);

	return nullptr;
}

int main(int argc, char **argv)
{
	int param = 0;
	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("\n mutex init failed\n");
		return 1;
	}
	register_handler = dbroker::RegisterHandler();
	listener((void *)&param);

	return 0;
}