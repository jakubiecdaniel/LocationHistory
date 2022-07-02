#include <iostream>
#include <vector>
#include <variant>
#include <map>

class JSON_VALUE;
#define JSON std::map<std::string, JSON_VALUE>
#define JSON_ARRAY std::vector<JSON_VALUE>
#define JSON_OBJECT std::variant<std::string, int, float, bool, JSON, JSON_ARRAY>

class JSON_VALUE {
public:
	JSON_OBJECT json_value;
};

#define JSON_TOKEN std::variant<char,bool,float,int,std::string>
#define TOKEN_VECTOR std::vector<JSON_TOKEN>

class JSONParser {

	const char JSON_SYNTAX[6] = { ',', ':', '{', '}', '[', ']' };

	const char JSON_QUOTE = '"';
	const char JSON_WHITESPACE = ' ';
	const char JSON_TAB = '\t';

	const char JSON_COMMA = ',';
	const char JSON_COLON = ':';

	const char JSON_LEFTBRACKET = '[';
	const char JSON_LEFTBRACE = '{';
	const char JSON_RIGHTBRACKET = ']';
	const char JSON_RIGHTBRACE = '}';

	const size_t TRUE_LEN = strlen("true");
	const size_t FALSE_LEN = strlen("false");
	const size_t NULL_LEN = strlen("NULL");

public:
	std::string lex_string(std::string& str);
	std::variant<float, int> lex_number(std::string& str);
	bool lex_bool(std::string& str);
	bool lex_null(std::string& str);
	TOKEN_VECTOR lex(std::string& str);

	JSON_VALUE
		parse_object(TOKEN_VECTOR& tokens);

	JSON_VALUE
		parse_array(TOKEN_VECTOR& tokens);

	JSON_VALUE
		parse(TOKEN_VECTOR& tokens);

	JSON string_to_json(std::string json);

};

