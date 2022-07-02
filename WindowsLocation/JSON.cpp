#include "JSON.h"
#include <stdexcept>

std::string JSONParser::lex_string(std::string& str) {

	std::string json_string = "";

	if (str[0] == JSON_QUOTE) {
		str = str.substr(1);
	}
	else {
		return "";
	}

	for (const auto& c : str) {
		if (c == JSON_QUOTE) {
			str = str.substr(json_string.length() + 1);
			return json_string;
		}
		else {
			json_string += c;
		}
	}
}

std::variant<float, int> JSONParser::lex_number(std::string& str) {
	bool isFloat = false;
	std::variant<float, int> ret;

	std::string json_number = "";

	char number_characters[] = { '0','1','2','3','4','5','6','7','8','9','-','e','.' };

	for (const auto& c : str) {
		if (std::find(std::begin(number_characters), std::end(number_characters), c) != std::end(number_characters)) {
			if (c == '.')
				isFloat = true;
			json_number += c;
		}
		else {
			break;
		}

	}

	if (json_number.length() == 0) {
		throw 50;
	}

	str = str.substr(json_number.length());

	if (isFloat) {
		ret = static_cast<float>(atof(json_number.c_str()));
	}
	else {
		ret = static_cast<int>(atoi(json_number.c_str()));
	}

	return ret;
}


bool JSONParser::lex_bool(std::string& str) {

	size_t len = str.length();
	if (len >= TRUE_LEN && str.substr(0, TRUE_LEN) == "true") {
		str = str.substr(TRUE_LEN);
		return true;
	}
	else if (len >= FALSE_LEN && str.substr(0, FALSE_LEN) == "false") {
		str = str.substr(FALSE_LEN);
		return false;
	}

	throw 50;

}

bool JSONParser::lex_null(std::string& str) {

	size_t len = str.length();
	if (len >= NULL_LEN && str.substr(0, NULL_LEN) == "null") {
		str = str.substr(TRUE_LEN);
		return true;
	}
	else
		return false;


}

TOKEN_VECTOR JSONParser::lex(std::string& str) {

	std::vector<std::variant<char, bool, float, int, std::string>> tokens;

	while (str.length() > 0) {

		std::string json_string = lex_string(str);
		if (json_string.length() > 0) {
			tokens.push_back(json_string);
		}

		try {
			std::variant<float, int> json_number = lex_number(str);
			if (std::holds_alternative<int>(json_number))
				tokens.push_back(std::get<int>(json_number));
			else
				tokens.push_back(std::get<float>(json_number));
		}
		catch (int e) {}

		try {
			bool json_bool = lex_bool(str);
			tokens.push_back(json_bool);
		}
		catch (int e) {}


		bool json_null = lex_null(str);
		if (json_null) {
			tokens.push_back("null");
		}

		if (str[0] == JSON_WHITESPACE || str[0] == JSON_TAB) {
			str = str.substr(1);
		}
		else if (std::find(std::begin(JSON_SYNTAX), std::end(JSON_SYNTAX), str[0]) != std::end(JSON_SYNTAX)) {
			tokens.push_back(str[0]);
			str = str.substr(1);
		}
		else {
			std::string message = "Unexpected character: ";
			message += str[0];
			throw std::runtime_error(message);
		}

	}

	return tokens;

}

JSON_VALUE
JSONParser::parse_array(TOKEN_VECTOR& tokens) {
	JSON_VALUE ret;

	JSON_ARRAY json_array;

	auto t = tokens.at(0);

	if (std::holds_alternative<char>(t) && std::get<char>(t) == JSON_RIGHTBRACKET) {
		tokens.erase(tokens.begin());
		ret.json_value = json_array;
		return ret;
	}

	while (true) {
		JSON_VALUE json = parse(tokens);
		json_array.push_back(json);

		t = tokens.at(0);
		if (std::holds_alternative<char>(t) && std::get<char>(t) == JSON_RIGHTBRACKET) {
			tokens.erase(tokens.begin());
			ret.json_value = json_array;
			return ret;
		}
		else if (std::holds_alternative<char>(t) && std::get<char>(t) != JSON_COMMA) {
			std::string message = "Expected comma, got: ";
			message += std::get<char>(t);
			throw std::runtime_error(message);
		}
		else {
			tokens.erase(tokens.begin());
		}

	}


}

JSON_VALUE
JSONParser::parse_object(TOKEN_VECTOR& tokens) {

	JSON json;
	JSON_VALUE ret;

	JSON_VALUE json_value;

	auto t = tokens.at(0);

	if (std::holds_alternative<char>(t) && JSON_RIGHTBRACE == std::get<char>(t)) {
		tokens.erase(tokens.begin());
		return ret;
	}

	while (true) {

		auto json_key = tokens.at(0);

		if (std::holds_alternative<std::string>(json_key)) {
			tokens.erase(tokens.begin());
		}
		else {
			throw std::runtime_error("Expected a string");
		}

		auto t = tokens.at(0);
		if (std::holds_alternative<char>(t) && std::get<char>(t) != JSON_COLON) {
			std::string message = "Expected colon, got: ";
			message += std::get<char>(t);
			throw std::runtime_error(message);
		}
		else if (!std::holds_alternative<char>(t)) {
			throw std::runtime_error("Expected colon");
		}

		tokens.erase(tokens.begin());
		JSON_VALUE json_value = parse(tokens);

		std::string key = std::get<std::string>(json_key);
		json[key] = json_value;

		t = tokens.at(0);

		if (std::holds_alternative<char>(t) && std::get<char>(t) == JSON_RIGHTBRACE) {
			tokens.erase(tokens.begin());
			ret.json_value = json;
			return ret;
		}
		else if (std::holds_alternative<char>(t) && std::get<char>(t) != JSON_COMMA) {
			std::string message = "Expected comma, got: ";
			message += std::get<char>(t);
			throw std::runtime_error(message);
		}
		else if (!std::holds_alternative<char>(t)) {
			throw std::runtime_error("Expected comma");
		}

		tokens.erase(tokens.begin());


	}


}

JSON_VALUE
JSONParser::parse(TOKEN_VECTOR& tokens) {
	JSON_VALUE ret;
	JSON_TOKEN t = tokens.at(0);
	if (std::holds_alternative<char>(t)) {
		if (JSON_LEFTBRACKET == std::get<char>(t)) {
			tokens.erase(tokens.begin());
			return parse_array(tokens);
		}
		else if (JSON_LEFTBRACE == std::get<char>(t)) {
			tokens.erase(tokens.begin());
			return parse_object(tokens);
		}
	}
	else {
		tokens.erase(tokens.begin());
		if (std::holds_alternative<std::string>(t)) {
			ret.json_value = std::get<std::string>(t);
		}
		else if (std::holds_alternative<int>(t)) {
			ret.json_value = std::get<int>(t);
		}
		else if (std::holds_alternative<float>(t)) {
			ret.json_value = std::get<float>(t);
		}
		else if (std::holds_alternative<bool>(t)) {
			ret.json_value = std::get<bool>(t);
		}
		return ret;
	}

}


JSON JSONParser::string_to_json(std::string json) {

	TOKEN_VECTOR a = lex(json);
	JSON_VALUE b = parse(a);
	return std::get<JSON>(b.json_value);

}