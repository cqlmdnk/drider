#ifndef EXAMPLE_MESSAGE_H
#define EXAMPLE_MESSAGE_H
#include <message.h>
#include <string.h>
#include <drider-types.h>
/*
 *
 *	Example basis message for development
 *	Message objects are header only except internal messages
 *
 */
namespace drider {
class ExampleMessage : public Message {
      private:
	/* data */
	double x;
	double y;
	double z;
	double yaw;
	double roll;
	double pitch;

      public:
	ExampleMessage(/* args */)
	{
		z = y = x = pitch = roll = yaw = 0.0;
	}
	ExampleMessage(char *bin_name, char *topic_name)
	{
	}
	~ExampleMessage()
	{
	}

	static int get_size_of_vars()
	{
		return 6 * sizeof(double);
	}

	void serialize(char *buffer)
	{
		memcpy(buffer + sizeof(double), &this->x, sizeof(double));
		memcpy(buffer + sizeof(double), &this->y, sizeof(double));
		memcpy(buffer + sizeof(double) * 2, &this->z, sizeof(double));
		memcpy(&this->yaw, buffer + sizeof(double) * 3, sizeof(double));
		memcpy(&this->roll, buffer + sizeof(double) * 4, sizeof(double));
		memcpy(&this->pitch, buffer + sizeof(double) * 5, sizeof(double));
	}
	void deserialize(char *buffer)
	{
		memcpy(&this->x, buffer, sizeof(double));
		memcpy(&this->y, buffer + sizeof(double), sizeof(double));
		memcpy(&this->z, buffer + sizeof(double) * 2, sizeof(double));
		memcpy(&this->yaw, buffer + sizeof(double) * 3, sizeof(double));
		memcpy(&this->roll, buffer + sizeof(double) * 4, sizeof(double));
		memcpy(&this->pitch, buffer + sizeof(double) * 5, sizeof(double));
	}
};
} // namespace drider
#endif