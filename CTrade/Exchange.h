#pragma once

#include <string.h>
#include <map>
#include <cpprest/http_client.h>

#include "Transaction.h"


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
	
	// No API Keys (should be) required
	pplx::task<void> GetPrice(std::string p_ticker, double* p_price);

	// API Keys Required
	pplx::task<void> GetOpenOrders(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_openTransactions);
	pplx::task<void> GetTransactionHistory(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_exchangeTransHistory);

	pplx::task<web::json::value> ExtractJSON(web::http::http_response response);

	friend class ExchangeManager;

protected:
	// Shared among all children
	std::string _GetNonce();
	std::string _GenerateRequestUri(std::string p_uriBase, std::vector<std::pair<std::string, std::string>> p_parameters);

	std::string m_exchangeName;
	std::string m_uriBase;
	std::string m_uriOpenTransactions;
	std::string m_uriTransactionHistory;
	std::string m_uriGetCoinInfo;


	// Define in each child class
	virtual void _InitURIs();
	virtual void _ParseTransactions(pplx::task<web::json::value> p_previousTask, std::string p_functionId, std::shared_ptr<std::vector<Transaction>> p_transactions);
	virtual utility::string_t _GetAuthenticatedRequestWithParameters(std::string p_functionId, std::string p_publicKey);
	virtual void _GetAuthenticatedHeaders(std::string p_functionId, std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);
	virtual utility::string_t _GetUnauthenticatedRequestWithParameters(std::string p_functionId);
	
	
	/*   DEPRECATED   */
	virtual void _ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price);
	virtual utility::string_t _GetRequestWithParameters_GetPrice(std::string p_ticker);
	/******************/

private:
	//Used only by the this base class
	web::http::client::http_client _GetHttpClient();
	web::http::http_request _GetAuthenticatedRequest  (std::string p_publicKey, std::string p_privateKey, std::string p_functionId);
	web::http::http_request _GetUnauthenticatedRequest(std::string p_functionId);
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

	virtual void _ParseTransactions(pplx::task<web::json::value> p_previousTask, std::string p_functionId, std::shared_ptr<std::vector<Transaction>> p_transactions) override;
	virtual utility::string_t _GetAuthenticatedRequestWithParameters(std::string p_functionId, std::string p_publicKey) override;
	virtual void _GetAuthenticatedHeaders(std::string p_functionId, std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders) override;
	virtual utility::string_t _GetUnauthenticatedRequestWithParameters(std::string p_functionId) override;

	/*  DEPRECATED  */
	virtual void _ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price) override;
	virtual utility::string_t _GetRequestWithParameters_GetPrice(std::string p_ticker) override;
	/****************/

private:
	bool _ResponseIndicatesFailure(const web::json::value& p_jsonValue);

	//JSON Handling
	std::string _GetTransactionID(web::json::value& p_jvalue);
	std::string _GetFromAsset(web::json::value& p_jvalue);
	std::string _GetToAsset(web::json::value& p_jvalue);
	std::string _GetTransactionType(web::json::value& p_jvalue);
	double _GetQuantity(web::json::value& p_jvalue);
	double _GetQuantityRemaining(web::json::value& p_jvalue);
	double _GetLimitValue(web::json::value& p_jvalue);
	std::string _GetTimeOpened(web::json::value& p_jvalue);
	bool _GetIsConditional(web::json::value& p_jvalue);
	std::string _GetCondition(bool p_isConditional, web::json::value& p_jvalue);
	double _GetConditionTarget(bool p_isConditional, web::json::value& p_jvalue);

	std::string _GetCloseDate(web::json::value& p_jvalue);
	double _GetCommission(web::json::value& p_jvalue);
	double _GetTotalPrice(web::json::value& p_jvalue);
	std::string _GetTimestamp(web::json::value& p_jvalue);
};