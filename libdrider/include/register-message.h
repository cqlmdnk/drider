#ifndef REGISTER_MESSAGE_H
#define REGISTER_MESSAGE_H
#include <drider-topic.h>
#include <message.h>
#include <string.h>
#include <types.h>

namespace drider {
class RegisterMessage : public Message {
      private:
	int type_;
	char bin_name_[BUF_SIZE_16];
	char topic_name_[BUF_SIZE_16];

      public:
	RegisterMessage();
	RegisterMessage(char *bin_name, int type, char *topic_name);
	~RegisterMessage();
	int &type();
	const int &type() const;

	char *get_bin_name();
	void set_bin_name(const char *bin_name) const;

	char *get_topic_name();
	void set_topic_name(const char *topic_name) const;

	static int get_size_of_vars();

	void serialize(char *buffer);
	void deserialize(char *buffer);
};
} // namespace drider
#endif