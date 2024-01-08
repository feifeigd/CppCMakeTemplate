#include "HttpParser.h"

HttpParser::HttpParser(http_parser_type parserType)
	: parserType_(parserType)
{
	parser_.data = this;
	http_parser_init(&parser_, parserType_);
}

bool HttpParser::hasEntry(std::string const& field, std::string const& value) const noexcept
{
	auto it = headers_.find(field);
	return it != headers_.end() && value == it->second;
}

bool HttpParser::hasKey(std::string const& field) const noexcept
{
	auto it = headers_.find(field);
	return it != headers_.end();
}

std::string const& HttpParser::get(std::string const& field) const noexcept
{
	auto it = headers_.find(field);
	if (headers_.end() == it)
	{
		return it->second;
	}

	const static std::string empty_str{};

	return empty_str;
}

void HttpParser::clear()
{
	method_ = -1;
	isUpgrade_ = false;
	isWebSocket_ = false;
	isCompleted_ = false;
	lastWasValue_ = false;

	url_.clear();
	query_.clear();
	body_.clear();
	status_.clear();
	currentField_.clear();
	
	headers_.clear();

	path_.clear();
	statusCode_ = 0;
	isKeepAlive_ = false;
}

size_t HttpParser::tryParse(char const* buffer, size_t length)
{
	auto const parsed = http_parser_execute(&parser_, &settings_, buffer, length);
	if (isCompleted_)
	{
		isUpgrade_ = parser_.upgrade;
		isWebSocket_ = isUpgrade_ && hasEntry("Upgrade", "websocket");
		method_ = parser_.method;
		http_parser_init(&parser_, parserType_);
	}

	return parsed;
}
