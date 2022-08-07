#include <register-message.h>
namespace drider {

RegisterMessage::RegisterMessage()
{
}
RegisterMessage::RegisterMessage(char *bin_name, int type, char *topic_name)
{
	strcpy(this->bin_name_, bin_name);
	this->type_ = type;
	strcpy(this->topic_name_, topic_name);
}
RegisterMessage::~RegisterMessage()
{
}
void RegisterMessage::set_type(int type)
{
	this->type_ = type;
}
const int &RegisterMessage::type() const
{
	return type_;
}

char *RegisterMessage::get_bin_name()
{
	return bin_name_;
}
void RegisterMessage::set_bin_name(const char *bin_name) const
{
	strcpy((char *)bin_name_, bin_name);
}

char *RegisterMessage::get_topic_name()
{
	return topic_name_;
}
void RegisterMessage::set_topic_name(const char *topic_name) const
{
	strcpy((char *)topic_name_, topic_name);
}

int RegisterMessage::get_size_of_vars()
{
	return sizeof(int) + (2 * BUF_SIZE_16);
}

void RegisterMessage::serialize(char *buffer)
{
	memcpy(buffer, &this->type_, sizeof(int));
	memcpy(buffer + sizeof(int), &this->bin_name_, BUF_SIZE_16);
	memcpy(buffer + sizeof(int) + BUF_SIZE_16, &this->topic_name_, BUF_SIZE_16);
}
void RegisterMessage::deserialize(char *buffer)
{
	memcpy(&this->type_, buffer, sizeof(int));
	memcpy(this->bin_name_, buffer + sizeof(int), BUF_SIZE_16);
	memcpy(&this->topic_name_, buffer + sizeof(int) + BUF_SIZE_16, BUF_SIZE_16);
}
} // namespace drider
