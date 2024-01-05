#pragma once

#include <string>
#include <unordered_map>

class HttpQueryParameter final {
public:
	void add(std::string const& key, std::string const& value);

	std::string const& getResult() const noexcept {
		return parameter_;
	}
private:
	std::string parameter_;
};

class HttpRequest final {
public:
	enum class HTTP_METHOD {
		GET,
		POST,
		PUT,
		DELETE,
		HEAD,
		OPTIONS,
		TRACE,
		CONNECT,
		PATCH,

		MAX,
	};

	void method(HTTP_METHOD protocol);
	void host(std::string const& host);
	void url(std::string const& url);
	void cookie(std::string const& cookie);
	void setContentType(std::string const& contentType);
	void query(HttpQueryParameter const& queryParameter);
	void body(std::string const& body);
	void body(std::string && body);
	void addHeadValue(std::string const& key, std::string const& value);

	std::string getResult() const noexcept;

private:
	HTTP_METHOD method_ { HTTP_METHOD::GET };
	std::string url_;
	std::string body_;
	HttpQueryParameter queryParameter_;
	std::unordered_map<std::string, std::string> headField_;
};

class HttpResponse final {
public:
	enum class HTTP_RESPONSE_STATUS {
		NONE,
		OK = 200,
	};

	void status(HTTP_RESPONSE_STATUS status) {
		status_ = status;
	}

	void setContentType(std::string const& contentType) {
		headField_["Content-Type"] = contentType;
	}

	void addHeadValue(std::string const& field, std::string const& value) {
		headField_[field] = value;
	}

	void body(std::string const& body);
	void body(std::string&& body);

	std::string getResult() const noexcept;

private:
	HTTP_RESPONSE_STATUS status_ { HTTP_RESPONSE_STATUS::OK };
	std::unordered_map<std::string, std::string> headField_;
	std::string body_;
};
