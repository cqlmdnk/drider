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
	RegisterMessage()
	{
	}
	RegisterMessage(char *bin_name, int type, char *topic_name)
	{
		strcpy(this->bin_name_, bin_name);
		this->type_ = type;
		strcpy(this->topic_name_, topic_name);
	}
	~RegisterMessage()
	{
	}
	int &type()
	{
		return type_;
	}
	const int &type() const
	{
		return type_;
	}

	char *get_bin_name()
	{
		return bin_name_;
	}
	void set_bin_name(const char *bin_name) const
	{
		strcpy((char *)bin_name_, bin_name);
	}

	char *get_topic_name()
	{
		return topic_name_;
	}
	void set_topic_name(const char *topic_name) const
	{
		strcpy((char *)topic_name_, topic_name);
	}

	static int get_size_of_vars()
	{
		return sizeof(int) + (2 * BUF_SIZE_16);
	}

	void serialize(char *buffer)
	{
		memcpy(buffer, &this->type_, sizeof(int));
		memcpy(buffer + sizeof(int), &this->bin_name_, BUF_SIZE_16);
		memcpy(buffer + sizeof(int) + BUF_SIZE_16, &this->topic_name_, BUF_SIZE_16);
	}
	void deserialize(char *buffer)
	{
		memcpy(&this->type_, buffer, sizeof(int));
		memcpy(this->bin_name_, buffer + sizeof(int), BUF_SIZE_16);
		memcpy(&this->topic_name_, buffer + sizeof(int) + BUF_SIZE_16, BUF_SIZE_16);
	}
};
} // namespace drider
#endif