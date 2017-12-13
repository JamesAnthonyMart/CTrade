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
	
	//Uses the default read-only keys
	pplx::task<void> GetPrice(std::string p_ticker, double* p_price);
	pplx::task<void> GetOpenOrders(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_openTransactions);
	pplx::task<void> GetTransactionHistory(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_exchangeTransHistory);

	pplx::task<web::json::value> ExtractJSON(web::http::http_response response);

	friend class ExchangeManager;
protected:

	//Must define all of these protected functions in each child class to fully support an exchange
	virtual void _InitURIs();
	virtual void _ParseOpenTransactions(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions);
	virtual void _ParseTransactionHistory(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions);
	virtual void _ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price);

		//Supported exchange functions:
	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey);
	virtual utility::string_t _GetRequestWithParameters_TransactionHistory(std::string p_publicKey);
	virtual utility::string_t _GetRequestWithParameters_GetPrice(std::string p_ticker);

	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);
	virtual void _GetAdditionalHeaders_TransactionHistory(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);
	virtual void _GetAdditionalHeaders_GetPrice(std::map<std::string, std::string>& p_additionalHeaders);\

	std::string m_exchangeName;
	std::string m_uriBase;
	std::string m_uriOpenTransactions;
	std::string m_uriTransactionHistory;
	std::string m_uriGetCoinInfo;
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

	virtual void _ParseOpenTransactions(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions) override;
	virtual void _ParseTransactionHistory(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions) override;
	virtual void _ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price) override;

	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey) override;
	virtual utility::string_t _GetRequestWithParameters_TransactionHistory(std::string p_publicKey) override;
	virtual utility::string_t _GetRequestWithParameters_GetPrice(std::string p_ticker) override;

	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders) override;
	virtual void _GetAdditionalHeaders_TransactionHistory(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders) override;
	virtual void _GetAdditionalHeaders_GetPrice(std::map<std::string, std::string>& p_additionalHeaders) override;

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