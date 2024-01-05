#include "HttpFormat.h"
#include <cassert>

void HttpQueryParameter::add(std::string const& key, std::string const& value) {
	if (not parameter_.empty())
	{
		parameter_.append("&");
	}
	parameter_.append(key).append("=").append(value);	// key=value
}

void HttpRequest::method(HTTP_METHOD protocol)
{
	assert(protocol >= HTTP_METHOD::GET and protocol <= HTTP_METHOD::PATCH);
	method_ = protocol;
}

void HttpRequest::host(std::string const& host)
{
	headField_["Host"] = host;
}

void HttpRequest::url(std::string const& url)
{
	url_ = url;
}

void HttpRequest::cookie(std::string const& cookie)
{
	headField_["Cookie"] = cookie;
}

void HttpRequest::setContentType(std::string const& contentType)
{
	headField_["Content-Type"] = contentType;
}

void HttpRequest::query(HttpQueryParameter const& queryParameter)
{
	queryParameter_ = queryParameter;
}

void HttpRequest::body(std::string const& body)
{
	body_ = body;
	headField_["Content-Length"] = std::to_string(body.size());
}

void HttpRequest::body(std::string&& body)
{
	body_ = std::move(body);
	headField_["Content-Length"] = std::to_string(body_.size());
}

void HttpRequest::addHeadValue(std::string const& key, std::string const& value)
{
	headField_[key] = value;
}

std::string HttpRequest::getResult() const noexcept
{
	constexpr auto MethodMax = static_cast<int>(HTTP_METHOD::MAX);
	const static char* MethodStr[MethodMax] = {
		"GET",
		"POST",
		"PUT",
		"DELETE",
		"HEAD",
		"OPTIONS",
		"TRACE",
		"CONNECT",
		"PATCH",
	};
	
	std::string result = MethodStr[static_cast<int>(method_)];

	result.append(" ").append(url_).append("?").append(queryParameter_.getResult()).append(" HTTP/1.1\r\n");

	for (auto const& [key, value] : headField_)
	{
		result.append(key).append(": ").append(value).append("\r\n");
	}

	result.append("\r\n").append(body_);

	return result;
}

void HttpResponse::body(std::string const& body)
{
	body_ = body;
	headField_["Content-Length"] = std::to_string(body.size());
}

void HttpResponse::body(std::string&& body)
{
	body_ = std::move(body);
	headField_["Content-Length"] = std::to_string(body_.size());
}

std::string HttpResponse::getResult() const noexcept
{
	std::string result = "HTTP/1.1 " + std::to_string(static_cast<int>(status_));
	switch (status_)
	{
	case HttpResponse::HTTP_RESPONSE_STATUS::OK:
		result.append(" OK");
		break;
	default:
		result.append(" UNKNOWN");
		break;
	}
	result.append(" \r\n");

	for (auto const& [key, value] : headField_)
	{
		result.append(key).append(": ").append(value).append("\r\n");
	}

	result.append("\r\n").append(body_);

	return result;
}
