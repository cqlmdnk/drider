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
Supports containers (list, vector, queue)
Will be extended
1-) std::vector

*/
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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
	bool is_container = false;
	std::string container_type;
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
const char *ws = " \t\n\r\f\v";

inline std::string &rtrim(std::string &s, const char *t = ws)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
inline std::string &ltrim(std::string &s, const char *t = ws)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

inline std::string &trim(std::string &s, const char *t = ws)
{
	return ltrim(rtrim(s, t), t);
}

int main(int argc, char **argv)
{
	std::string look_path = argv[1];
	std::string put_path = argv[2];
	std::vector<std::string> included_type_header;
	std::vector<std::string> var_sized_vars; 
	if (argc != 3) {
		fprintf(stderr, "Usage ./mg <look_path_for message definitions (.idl files)> <output dir for headers (.h files)>");
	}

	// *.idl parsing
	for (const auto &entry : fs::directory_iterator(look_path)) {
		var_sized_vars.clear();
		std::string _path = entry.path();
		if (_path.substr(trim(_path, "\"").find_last_of(".") + 1) != "idl") {
			continue;
		}
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

container_case:
			consume_whitespace_till_non_whitespace();
			_attr.type = consume_till_whitespace();
			if (!(_attr.is_container) && (_attr.type.compare("list") == 0 || _attr.type.compare("vector") == 0 || _attr.type.compare("queue") == 0)) {
				if (std::find(included_type_header.begin(), included_type_header.end(), _attr.type) == included_type_header.end()){
					included_type_header.push_back(_attr.type);
				}
				_attr.is_container = true;
				_attr.container_type = _attr.type;
				goto container_case;
			}
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
			if (_attr.length.empty() && _attr.is_container) {
				var_sized_vars.push_back(_attr.name);
				fprintf(stderr, "pushed name is %s\n", _attr.name.c_str());
			}
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

		for (auto header : included_type_header) {
			myheader << "#include <" << header << ">" << std::endl;
		}

		myheader << std::endl
			 << "namespace drider {" << std::endl;
		myheader << "class " << class_name << " : public Message {" << std::endl;

		myheader << "\t  private:" << std::endl;

		std::vector<struct attribute>::iterator it = attributes.begin();
		for (; it < attributes.end(); it++) {
			std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->is_container)
				myheader << "\tstd::" << it->container_type << "<" << it->type << "> _" << it->name << ";" << std::endl;
			else if (it->length.empty())
				myheader << "\t" << it->type << " _" << it->name << ";" << std::endl;
			else
				myheader << "\t" << it->type << " _" << it->name << "[" << it->length << "];" << std::endl;
		}

		myheader << "\t  public:" << std::endl;

		std::string constructor_params;
		std::string constructor_block;
		std::string constructor_block_def;
		it = attributes.begin();
		for (; it < attributes.end(); it++) {

			std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->length.empty()) {
				if (it->is_container)
					constructor_params += "std::" + it->container_type + "<" + it->type + "> " + it->name;
				else
					constructor_params += it->type + " " + it->name;
				constructor_block += "\t\t_" + it->name + "=" + it->name + ";\n";
				if (!(it->is_container))
					constructor_block_def += "\t\t_" + it->name + "=0;\n";
			} else {
				constructor_params += it->type + "* " + it->name;
				constructor_block += "\t\tmemcpy(_" + it->name + ", " + it->name + ", " + "sizeof(" + it->type + ") * " + it->length + ");\n";
			}
			if (it + 1 != attributes.end())
				constructor_params += ", ";
		}

		myheader << "\t" << class_name << "()\n\t{\n" + constructor_block_def + "\t}" << std::endl;

		myheader << "\t" << class_name << "(" << constructor_params << ") {\n";
		myheader << constructor_block;
		myheader << "\t}" << std::endl;

		myheader << "\t~" << class_name << "(){}" << std::endl;

		// getters&setters
		/*
		X x() const { return x_; }
		void x(X x) { memcpy(x_, x, size * len); }
		*/
		it = attributes.begin();
		for (; it < attributes.end(); it++) {
			// std::cout << it->name << " len " << it->name.length() << std::endl;
			if (it->length.empty()) {
				if (it->is_container)
					myheader << "\tstd::" << it->container_type << "<" << it->type << "> & " << it->name << "() { return _" << it->name << ";}" << std::endl;
				else
					myheader << "\t" << it->type << " " << it->name << "() { return _" << it->name << ";}" << std::endl;
			} else
				myheader << "\t" << it->type << " *" << it->name << "() { return _" << it->name << ";}" << std::endl;
			if (it->length.empty()) {
				if (it->is_container)
					myheader << "\tvoid " << it->name << "(std::" << it->container_type << "<" << it->type << "> " << it->name << ") { _" << it->name << " = " << it->name << ";}" << std::endl;
				else
					myheader << "\tvoid " << it->name << "(const " << it->type << " " << it->name << ") { _" << it->name << " = " << it->name << ";}" << std::endl;
			} else
				myheader << "\tvoid " << it->name << "(const " << it->type << " *" << it->name << ") { memcpy(_" << it->name << ", " << it->name << ", "
					 << "sizeof(" << it->type << ") * " << it->length << "); }" << std::endl;
		}

		myheader << "\tvoid serialize(char *buffer)" << std::endl
			 << "\t{" << std::endl;
		std::string offset;
		it = attributes.begin();
		std::vector<std::string>::iterator var_sized_var_it = var_sized_vars.begin();
		myheader << "\t\tuint8_t num_of_var_sized_vars = " + std::to_string(var_sized_vars.size()) + ";" << std::endl;
		myheader << "\t\tmemcpy(buffer, &num_of_var_sized_vars, sizeof(uint8_t));" << std::endl;
		int offset_var_sized = 0;
		for (size_t i = 0; i < var_sized_vars.size(); i++)
		{
			myheader << "\t\tuint32_t var_size_" + *var_sized_var_it + " = _" + *var_sized_var_it + ".size();" << std::endl;
			myheader << "\t\tmemcpy(buffer + sizeof(uint8_t) + " + std::to_string(i) + " * sizeof(uint32_t), &var_size_" + *var_sized_var_it + ", sizeof(uint32_t));" << std::endl;
			var_sized_var_it++;
			offset_var_sized++;
		}
		if (var_sized_vars.empty()) {
			myheader << "\t\tUNUSED(num_of_var_sized_vars);" << std::endl;
		}
		offset += " + sizeof(uint8_t)";
		if (offset_var_sized != 0)
			offset += " + " + std::to_string(offset_var_sized) + " * sizeof(uint32_t)";
		
		for (; it < attributes.end(); it++) {
			std::string offset_inc;
			if (it->length.empty()) {
				if (it->is_container) {
					myheader << "\t\tstd::" + it->container_type + "<" + it->type + ">::iterator " + it->name + "_it = _" + it->name + ".begin();" << std::endl;
					myheader << "\t\tlong " + it->name + "_container_offset = 0;" << std::endl;
					myheader << "\t\tfor (; " + it->name + "_it != _" + it->name + ".end(); " + it->name + "_it++) {" << std::endl;
					myheader << "\t\t\tmemcpy(buffer" + offset + " + sizeof(" + it->type + ") * " + it->name + "_container_offset, &(*" + it->name + "_it), sizeof(" + it->type + "));" << std::endl;
					myheader << "\t\t\t" + it->name + "_container_offset++;" << std::endl;
					myheader << "\t\t}" << std::endl;
					offset_inc = "sizeof(" + it->type + ") * " + it->name + "_container_offset";
				} else {
					myheader << "\t\tmemcpy(buffer" + offset + ", &_" << it->name << ", sizeof(" << it->type << "));" << std::endl;
					offset_inc = "sizeof(" + it->type + ")";
				}
			} else {
				myheader << "\t\tmemcpy(buffer" + offset + ", _" << it->name << ", sizeof(" << it->type << ") * " << it->length << ");" << std::endl;
				offset_inc = "sizeof(" + it->type + ") * " + it->length;
			}
			offset += " + " + offset_inc;
			offset_inc.clear();
		}
		myheader << "\t}" << std::endl;
		myheader << "\tvoid deserialize(char *buffer)" << std::endl
			 << "\t{" << std::endl;
		offset.clear();
		it = attributes.begin();
		
			myheader << "\t\tuint8_t num_of_var_sized_vars = (uint8_t) buffer[0];" << std::endl;
			myheader << "\t\tbuffer += sizeof(uint8_t);" << std::endl;
		if (!var_sized_vars.empty()) {
			myheader << "\t\tuint32_t i;" << std::endl;
			myheader << "\t\tstd::list<uint32_t> sizes_of_vars;" << std::endl;
			myheader << "\t\tfor (i = 0; i < num_of_var_sized_vars; i++) {" << std::endl;
			myheader << "\t\t\tuint32_t size_of_var = 0;" << std::endl;
			myheader << "\t\t\tmemcpy(&size_of_var, buffer + i * sizeof(uint32_t), sizeof(uint32_t));" << std::endl;
			myheader << "\t\t\tsizes_of_vars.push_back(size_of_var);" << std::endl;
			myheader << "\t\t\tbuffer += i * sizeof(uint32_t);" << std::endl;
			myheader << "\t\t}" << std::endl;
			myheader << "\t\tstd::list<uint32_t>::iterator it = sizes_of_vars.begin();" << std::endl;
		} else {
			myheader << "\t\tUNUSED(num_of_var_sized_vars);" << std::endl;
		}
		
		for (; it < attributes.end(); it++) {
			std::string offset_inc;
			std::string offset_inc_not_cont;
			if (it->length.empty()) {
				if (it->is_container) {
					myheader << "\t\tfor (i = 0; i < *it; i++) {" << std::endl;
					
					myheader << "\t\t\t" + it->type + " tmp_" + it->name + "_elem;" << std::endl;
					myheader << "\t\t\tmemcpy(&tmp_" + it->name + "_elem, buffer" << (offset.empty() ? "" : " + ") << offset << " + i * sizeof(" + it->type + "), sizeof(" << it->type << "));" << std::endl;
					myheader << "\t\t\t_" + it->name + ".push_back(tmp_" + it->name + "_elem);" << std::endl;
						
					myheader << "\t\t}" << std::endl;
					myheader << "\t\tit++;" << std::endl;
					offset_inc = "_" + it->name + ".size() * sizeof(" + it->type + ")";
				} else {
					myheader << "\t\tmemcpy(&_" << it->name << ", buffer" << (offset.empty() ? "" : " + ") << offset << ", sizeof(" << it->type << "));" << std::endl;
					offset_inc = "sizeof(" + it->type + ")";
				}

			} else {
				myheader << "\t\tmemcpy(_" << it->name << ", buffer" << (offset.empty() ? "" : " + ") << offset << ", sizeof(" << it->type << ") * " << it->length << ");" << std::endl;
				offset_inc = "sizeof(" + it->type + ") * " + it->length;
			}
			if (it != attributes.begin())
				offset += " + ";
			offset += offset_inc;
			offset_inc.clear();
		}
		myheader << "\t}" << std::endl;

		myheader << "\t" + (var_sized_vars.empty() == true ? std::string("static ") : std::string("")) + "size_t get_size_of_vars()" << std::endl
			 << "\t{" << std::endl;
		myheader << "\t\t return sizeof(uint8_t) + " << offset << ";" << std::endl;
		myheader << "\t}" << std::endl;

		myheader << "};\n}" << std::endl;
		myheader << "#endif" << std::endl;
		myheader.close();
		// execute clang-format after file is created
		index = 0;
	}

	return 0;
}
