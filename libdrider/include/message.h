#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
#define UNUSED(x) [&x]{}()
namespace drider {
class Message {
      private:
	/* data */
	int id = 0;
	int time_stamp = 0;

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