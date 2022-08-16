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

/*
Supports int, char, char*, bool, float, double. long/short and
Does not support unsigned, STL objs, containers, wrappers
Will be extended
1-) unsigned variables
2-) std::vector

*/
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT_FORMAT_ERROR(var)                                    \
	do {                                                        \
		fprintf(stderr, "Assertion is reached. %s\n", var); \
		exit(-1);                                           \
	} while (0)

namespace fs = std::filesystem;

struct attribute {
	std::string type;
	std::string name;
	std::string length;
};

// PascalCase to kebab-case
std::string pascal_case_to_kebab_case(std::string class_name)
{
	size_t _idx = 0;
	if (!std::isupper(class_name[_idx]))
		ASSERT_FORMAT_ERROR("Class name is not PascalCase.");
	class_name[_idx] = std::tolower(class_name[_idx]);
	_idx++;
	while (_idx < class_name.size()) {
		// if(class_name[_idx] == 'M')
		//	break;
		if (std::islower(class_name[_idx])) {
			while (std::islower(class_name[_idx])) {
				_idx++;
			}
			if (_idx >= class_name.size())
				break;
			class_name.insert(_idx++, "-");
			class_name[_idx] = std::tolower(class_name[_idx]);
		}
		_idx++;
	}
	return class_name;
}

int main(int argc, char **argv)
{
	std::string look_path = argv[1];
	std::string put_path = argv[1];

	// *.idl parsing
	for (const auto &entry : fs::directory_iterator(look_path)) {
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
				ASSERT_FORMAT_ERROR("consume");
		};

		auto consume_string = [&](std::string food) {
			size_t food_index = 0;
			while (food_index < food.size()) {
				if (food[food_index++] != consume()) {
					ASSERT_FORMAT_ERROR("consume string");
				}
			}
			if (food_index != food.size())
				ASSERT_FORMAT_ERROR("out of index consume string");
			return true;
		};

		auto consume_whitespace_till_non_whitespace = [&] {
			while (peek() == ' ' || peek() == '\n' || peek() == '\t') {
				consume();
			}
		};

		auto consume_till_char = [&](char till) {
			std::string word;
			while (peek() != till) {
				word += consume();
			}
			return word;
		};

		auto consume_till_char_or_char = [&](char till1, char till2) {
			std::string word;
			while (peek() != till1 && peek() != till2) {
				word += consume();
			}
			return word;
		};

		auto consume_till_whitespace = [&] {
			std::string word;
			while (peek() != ' ' && peek() != '\t') {
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
			struct attribute _attr;
			consume_whitespace_till_non_whitespace();
			consume_string("attribute");

			consume_whitespace_till_non_whitespace();
			_attr.type = consume_till_whitespace();

			consume_whitespace_till_non_whitespace();

			_attr.name = consume_till_char_or_char(' ', ';');
			fprintf(stderr, "name is %s\n", _attr.name.c_str());
			consume_whitespace_till_non_whitespace();
			if (peek() == '[') {
				fprintf(stderr, "has length if \n");
				if (consume() != '[')
					ASSERT_FORMAT_ERROR("[");

				_attr.length = consume_till_char(']');
				if (consume() != ']')
					ASSERT_FORMAT_ERROR("]");
			} else
				_attr.length = "";
			fprintf(stderr, "type is %s\n", _attr.type.c_str());
			consume_whitespace_till_non_whitespace();
			if (consume() != ';') // error
				ASSERT_FORMAT_ERROR(";");
			consume_whitespace_till_non_whitespace();
			fprintf(stderr, "type is %s\n", _attr.type.c_str());

			attributes.emplace_back(_attr);
			count++;
			fprintf(stderr, "count is %d\n", count);
			if (peek() == '}')
				break;
		}
		std::cout << "Total attributes : " << count << std::endl;

		// Header file forming
		std::ofstream myheader;
		std::string myheader_file_name = pascal_case_to_kebab_case(class_name) + ".h";
		std::cout << myheader_file_name << std::endl;
		myheader.open(put_path + "/" + myheader_file_name);
		std::string include_guard = myheader_file_name;
		for (auto &c : include_guard) {
			if (c == '-' || c == '.')
				c = '_';
			else
				c = toupper(c);
		}
		myheader << "#ifndef " + include_guard << std::endl;
		myheader << "#define " + include_guard << std::endl;

		// Need dynamic include with respect to attributes
		myheader << "#include <drider-types.h>" << std::endl;
		myheader << "#include <message.h>" << std::endl;

		myheader << "#include <string.h>" << std::endl;

		myheader << std::endl
			 << "namespace drider {" << std::endl;
		myheader << "class " << class_name << " : public Message {" << std::endl;

		myheader << "\t  private:" << std::endl;

		std::vector<struct attribute>::iterator it = attributes.begin();
		for (; it < attributes.end(); it++) {
			std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->length.empty())
				myheader << "\t" << it->type << " _" << it->name << ";" << std::endl;
			else
				myheader << "\t" << it->type << " _" << it->name << "[" << it->length << "];" << std::endl;
		}

		myheader << "\t  public:" << std::endl;
		myheader << "\t" << class_name << "();" << std::endl;

		std::string constructor_params;
		it = attributes.begin();
		for (; it < attributes.end(); it++) {
			std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->length.empty())
				constructor_params += it->type + " " + it->name;
			else
				constructor_params += it->type + "* " + it->name;
			if (it + 1 != attributes.end())
				constructor_params += ", ";
		}
		myheader << "\t" << class_name << "(" << constructor_params << ");" << std::endl;

		myheader << "\t~" << class_name << "();" << std::endl;

		// getters&setters
		/*
		X x() const { return x_; }
		void x(X x) { memcpy(x_, x, size * len); }
		*/
		it = attributes.begin();
		for (; it < attributes.end(); it++) {
			// std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->length.empty())
				myheader << "\t" << it->type << " " << it->name << "() const { return _" << it->name << ";}" << std::endl;
			else
				myheader << "\t" << it->type << " *" << it->name << "() const { return _" << it->name << ";}" << std::endl;
			if (it->length.empty())
				myheader << "\tvoid " << it->name << "(" << it->type << " " << it->name << ") { _" << it->name << " = " << it->name << ";}" << std::endl;
			else
				myheader << "\tvoid " << it->name << "(" << it->type << " *" << it->name << ") { memcpy(_" << it->name << ", " << it->name << ", "
					 << "sizeof(" << it->type << ") * " << it->length << ");}" << std::endl;
		}

		myheader << "\tvoid serialize(char *buffer)" << std::endl
			 << "{" << std::endl;
		std::string offset;
		it = attributes.begin();
		for (; it < attributes.end(); it++) {
			std::string offset_inc;
			if (it->length.empty()) {
				myheader << "\t\tmemcpy(buffer" + offset + ", &_" << it->name << ", sizeof(" << it->type << "));";
				offset_inc = "sizeof(" + it->type + ")";
			} else {
				myheader << "\t\tmemcpy(buffer" + offset + ", _" << it->name << ", sizeof(" << it->type << ") * " << it->length << ");";
				offset_inc = "sizeof(" + it->type + ") * " + it->length;
			}
			offset += "+" + offset_inc;
			offset_inc.clear();
		}
		myheader << "}" << std::endl;
		myheader << "\tvoid deserialize(char *buffer)" << std::endl
			 << "{" << std::endl;
		offset.clear();
		it = attributes.begin();
		for (; it < attributes.end(); it++) {
			std::string offset_inc;
			if (it->length.empty()) {
				myheader << "\t\tmemcpy(&_" << it->name << ", buffer" << offset << ", sizeof(" << it->type << "));";
				offset_inc = "sizeof(" + it->type + ")";
			} else {
				myheader << "\t\tmemcpy(_" << it->name << ", buffer" << offset << ", sizeof(" << it->type << ") * " << it->length << ");";
				offset_inc = "sizeof(" + it->type + ") * " + it->length;
			}
			offset += "+" + offset_inc;
			offset_inc.clear();
		}
		myheader << "}" << std::endl;

		myheader << "}\n}" << std::endl;
		myheader << "#endif" << std::endl;
		myheader.close();
		// execute clang-format after file is created
		index = 0;
	}

	return 0;
}
