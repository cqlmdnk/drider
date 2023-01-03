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
	uint8_t _num_of_variable_sized_vars;

      public:
	Message(/* args */)
	{
	}
	virtual ~Message() = default;
	virtual void serialize(char *buffer) = 0;
	virtual void deserialize(char *buffer) = 0;
	void num_of_variable_sized_vars(const uint8_t num_of_variable_sized_vars) { _num_of_variable_sized_vars = num_of_variable_sized_vars;}
	uint8_t num_of_variable_sized_vars() { return _num_of_variable_sized_vars;}
};

} // namespace drider
#endif