#pragma once

#include <string.h>
#include <map>
#include <cpprest/http_client.h>

class ExchangeManager;

/*
* This class is an abstraction for an individual exchange.
* This class keeps all shared code to make individual exchange implementations as simple as possible.
* This is to help with supporting new exchanges later.
*/
class Exchange {
public:
	Exchange(std::string p_name) : m_exchangeName(p_name) { }
	~Exchange();

	std::string GetName() { return m_exchangeName; }
	pplx::task<void> GetOpenOrders(std::string p_publicKey, std::string p_privateKey);
	
	pplx::task<web::json::value> ExtractJSON(web::http::http_response response);

	friend class ExchangeManager;
protected:

	//Must define all of these protected functions in each child class to fully support an exchange
	virtual void _InitURIs();
	virtual void _CreateTransactionFromJSON(pplx::task<web::json::value> p_jsonValue);
		//Supported exchange functions:
	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey);
	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);

	std::string m_exchangeName;
	std::string m_uriBase;
	std::string m_uriOpenTransactions;
	std::string m_uriTransactionHistory;
};




/*
* 
* Supported exchanges
* 
*/

class Bittrex : public Exchange
{
public:
	Bittrex();
	~Bittrex();

protected:
	virtual void _InitURIs() override;
	virtual void _CreateTransactionFromJSON(pplx::task<web::json::value> p_jsonValue) override;

	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey) override;
	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders) override;
};