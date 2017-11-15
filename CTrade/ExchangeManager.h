#pragma once
/*
Description: This class is used to make all queries to asset exchanges. In the first 
  use case (Crypto), that could be Bittrex, Poloniex, etc. (Anything for which the 
  portfolio has registered keys)

Author: James Mart
License: TBD
*/

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include <cpprest/http_client.h>

#include "Transaction.h"


/*
This class is an abstraction for an individual exchange like Bittrex or Poloniex. They all have their own API endpoints,
  but can all execute similar queries.
*/
class Exchange {
public:
	Exchange(std::string p_name) : m_exchangeName(p_name) { }
	~Exchange();
	
	std::string GetName() { return m_exchangeName; }
	pplx::task<void> GetOpenOrders(std::string p_publicKey, std::string p_privateKey);

protected:
	virtual void _DefineURIs();
	
	//Currently supported exchange functions
	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey);
	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);
	
	std::string m_exchangeName;

	std::string m_uriBase;
	std::string m_uriOpenTransactions;
	std::string m_uriTransactionHistory;
};

class Bittrex : public Exchange 
{
public:
	Bittrex();
	~Bittrex();

protected:
	virtual void _DefineURIs();

	//Currently supported exchange functions
	//virtual pplx::task<void> _GetOpenOrders(std::string p_publicKey, std::string p_privateKey);
	virtual utility::string_t _GetRequestWithParameters_OpenOrders(std::string p_publicKey);
	virtual void _GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders);
};




/*
The actual exchange being used should be irrelevant to the Manager, unless the manager is trying to engage in 
  arbitrage opportunities. Otherwise, he should just be able to open positions on assets without caring what exchange 
  is used.
*/
class ExchangeManager
{
public:
	static ExchangeManager& Get() {
		static ExchangeManager E;
		return E;
	}

	std::vector<Transaction> GetOpenTransactions(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey);

private: 
	ExchangeManager();
	~ExchangeManager();

	std::vector<std::unique_ptr<Exchange>> m_exchanges;
};
