#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT_FORMAT_ERROR(var)                                   \
	do {                                                   \
		if (1) {                                       \
			fprintf(stderr, "Assertion is reached. %s\n", var); \
			exit(-1);                              \
		}                                              \
	} while (0)



namespace fs = std::filesystem;

struct attribute
{
	std::string type;
	std::string name;
	std::string length;
};

int main(int argc, char **argv)
{

	std::string path = argv[1];
	for (const auto &entry : fs::directory_iterator(path)) {
		std::cout << "Parsing : " << entry.path() << std::endl;
		std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		size_t index = 0;
		size_t buffer_size = size;
		std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		auto peek = [&] {
			return buffer[index];
		};

		auto consume = [&] {
			if (index < buffer_size)
				return buffer[index++];
			else
				ASSERT_FORMAT_ERROR("consume") ;
		};

		auto consume_string = [&](std::string food) {
			size_t food_index = 0;
			while (food_index < food.size() ) {
				if (food[food_index++] != consume()) {
					ASSERT_FORMAT_ERROR("consume string");
				}
			}
			if (food_index != food.size())
				ASSERT_FORMAT_ERROR("out of index consume string") ;
			return true;
		};

		auto consume_whitespace_till_non_whitespace = [&] {
			while (peek() == ' ' || peek() == '\n') {
				consume();
			}
		};

		auto consume_till_char = [&] (char till) {
			std::string word;
			while (peek() != till) {
				word += consume();
			}
			return word;
		};

		auto consume_till_whitespace = [&] {
			std::string word;
			while (peek() != ' ') {
				word += consume();
			}
			return word;
		};

	
		consume_whitespace_till_non_whitespace();
		consume_string("interface");
		consume_whitespace_till_non_whitespace();
		auto class_name = consume_till_whitespace();
		consume_whitespace_till_non_whitespace();
		if (consume() != '{')
			ASSERT_FORMAT_ERROR("{");
		std::vector<struct attribute> attributes;
		int count = 0;
		while (1) {
			bool has_length = false;
			struct attribute _attr;
			consume_whitespace_till_non_whitespace();
			consume_string("attribute");
			

			consume_whitespace_till_non_whitespace();
			_attr.type = consume_till_whitespace();
			if (_attr.type.compare("char*") == 0)
				has_length = true;
			consume_whitespace_till_non_whitespace();
			if(has_length)
				_attr.name = consume_till_char(' ');
			else
				_attr.name = consume_till_char(';');
			consume_whitespace_till_non_whitespace();
			if (has_length) {
				if (consume() != '[') 
					ASSERT_FORMAT_ERROR("["); 
				
				_attr.length = consume_till_char(']');
				if (consume() != ']')
					ASSERT_FORMAT_ERROR("]") ;

			}
			consume_whitespace_till_non_whitespace();
			if (consume() != ';') //error
				ASSERT_FORMAT_ERROR(";") ;
			consume_whitespace_till_non_whitespace();
			attributes.emplace_back(_attr);
			count++;
			if (peek() == '}')
				break;
		}
		std::cout << "Total attributes : " << count << std::endl;
		

			// std::cout << buffer[0] << buffer[1];
			// std::ofstream myfile;
			// myfile.open("example.txt");
			// myfile << buffer.data();
			// myfile.close();
		
		index = 0;
	}

	return 0;
}