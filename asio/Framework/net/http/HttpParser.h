#pragma once

#include "http_parser.h"
#include "WebSocketFormat.h"
#include <iostream>
#include <unordered_map>

class HttpParser {
public:
	HttpParser(http_parser_type parserType);

	bool isUpgrade() const noexcept { return isUpgrade_; }
	bool isWebSocket() const noexcept { return isWebSocket_; }
	bool isKeepAlive() const noexcept { return isKeepAlive_; }

	// -1 为无效值，defined in http_method
	int method() const noexcept { return method_; }

	std::string const& path() const noexcept { return path_; }
	std::string const& query() const noexcept { return query_; }
	std::string const& status() const noexcept { return status_; }
	int statusCode() const noexcept { return statusCode_; }

	bool hasEntry(std::string const& field, std::string const& value) const noexcept;
	bool hasKey(std::string const& field) const noexcept;
	std::string const& get(std::string const& field) const noexcept;
	std::string const& body() const noexcept { return body_; }

	bool isCompleted() const noexcept { return isCompleted_; }
private:
	void clear();
	size_t tryParse(char const* buffer, size_t length);
private:

private:
	http_parser_type const parserType_;
	http_parser parser_ { };

	http_parser_settings settings_{
		.on_message_begin		= [](http_parser* parser) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			self->clear();
			return 0;
		},
		.on_url					= [](http_parser* parser, char const* at, size_t length) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			self->url_.append(at, length);
			return 0;
		},
		.on_status				= [](http_parser* parser, char const* at, size_t length) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			self->status_.append(at, length);
			self->statusCode_ = parser->status_code;
			return 0;
		},
		.on_header_field		= [](http_parser* parser, char const* at, size_t length) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			if (self->lastWasValue_) {
				self->currentField_.clear();
			}
			self->currentField_.append(at, length);
			self->lastWasValue_ = false;
			return 0;
		},
		.on_header_value		= [](http_parser* parser, char const* at, size_t length) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			auto& value = self->headers_[self->currentField_];
			value.append(at, length);
			self->lastWasValue_ = true;
			return 0;
		},
		.on_headers_complete	= [](http_parser* parser) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			if (self->url_.empty())
			{
				return 0;
			}
			http_parser_url u;
			int const result = http_parser_parse_url(self->url_.data(), self->url_.size(), 0, &u);
			if (result != 0)
			{
				return result;
			}
			if (not (u.field_set & (1 << UF_PATH)))
			{
				std::cerr << "No path in URL" << std::endl;
				return -1;
			}
			self->path_ = self->url_.substr(u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
			if (u.field_set & (1 << UF_QUERY))
			{
				self->query_ = self->url_.substr(u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);
			}
			
			return 0;
		},
		
		.on_body				= [](http_parser* parser, char const* at, size_t length) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			self->body_.append(at, length);
			return 0;
		},
		.on_message_complete	= [](http_parser* parser) -> int {
			auto self = reinterpret_cast<HttpParser*>(parser->data);
			self->isCompleted_ = true;
			self->isKeepAlive_ = http_should_keep_alive(parser);
			return 0;
		},
		.on_chunk_header		= [](http_parser* parser) -> int {
			return 0;
		},
		.on_chunk_complete		= [](http_parser* parser) -> int {
			return 0;
		},
	};

	bool lastWasValue_ { };

	int method_ = -1;
	bool isUpgrade_ { };
	bool isWebSocket_ { };
	bool isKeepAlive_ { };
	bool isCompleted_ { };

	int statusCode_{};
	WebSocketFormat::WebSocketFrameType webSocketFrameType_{ WebSocketFormat::WebSocketFrameType::ERROR_FRAME };

	std::string status_;
	std::string body_;
	std::string url_;
	std::string path_;
	std::string query_;
	std::string currentField_;
	std::unordered_map<std::string, std::string> headers_;
};
