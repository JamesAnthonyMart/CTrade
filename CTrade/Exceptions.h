#pragma once

#include <exception>
#include <string>

class InvalidSignatureException : public std::exception
{
public: 
	virtual const char* what() const throw()
	{
		return "Exception: HttpRequest had invalid signature. Check hash";
	}
} EInvalidSignature;

class BadHttpResponse : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Exception: Server didn't respond with a 200 OK response code.";
	}
} EBadResponse;