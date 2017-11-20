#pragma once

#include <exception>
#include <string>

class MalformedRequest : public std::exception
{
public: 
	MalformedRequest() : std::exception(), m_message("No additional information provided.") {}
	MalformedRequest(std::string p_message) : std::exception(), m_message(p_message) {}
	void SetExceptionMessage(std::string p_message) { m_message = p_message; }
	std::string GetExceptionMessage() { return m_message; }

	virtual const char* what() const throw()
	{
		return "Exception: HTTPRequest failed because of a malformed request.";
	}

private: 
	std::string m_message;
} EMalformedRequest;

class BadHttpResponse : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Exception: Server didn't respond with a 200 OK response code.";
	}
} EBadResponse;