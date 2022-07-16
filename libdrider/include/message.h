#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
namespace drider {
class Message {
      private:
	/* data */
	int id;
	int time_stamp;

      public:
	Message(/* args */)
	{
	}
	virtual ~Message() = default;
	virtual void serialize(char *buffer) = 0;
	virtual void deserialize(char *buffer) = 0;
};

} // namespace drider
#endif