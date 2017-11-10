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
#include <functional>

#include "Transaction.h"


/*
This class is an abstraction for an individual exchange like Bittrex or Poloniex. They all have their own API endpoints,
  but can all execute similar queries.
*/
class Exchange {
public:
	Exchange(std::string p_name, std::string p_apiKey) : m_exchangeName(p_name), m_apiKey(p_apiKey) {}

	void GetOpenTransactions(std::vector<Transaction>& p_transactions) { _GetOpenTransactions(p_transactions); }
	void GetClosedTransactions(std::vector<Transaction>& p_transactions) { _GetClosedTransactions(p_transactions); }

protected:
	virtual std::string _DefEndpointHome() = 0;
	virtual std::string _DefEndpointGetOpenTransactions() = 0;
	virtual std::string _DefEndpointGetClosedTransactions() = 0;

	std::string m_apiKey;
	std::string m_exchangeName;

private:
	std::function<void(std::vector<Transaction>&)> _GetOpenTransactions;
	std::function<void(std::vector<Transaction>&)> _GetClosedTransactions;

};
class BittrexClient : public Exchange {
public:
	BittrexClient(std::string p_name, std::string p_apiKey) : Exchange(p_name, p_apiKey) {  }
	virtual ~BittrexClient() { }

	virtual std::string _DefEndpointHome() { return "https://www.bittrex.com/api"; }
	virtual std::string _DefEndpointGetOpenTransactions() { return "/openTransactions/?apiKey="; }
	virtual std::string _DefEndpointGetClosedTransactions() { return "/closedTransactions/?apiKey="; }

private:

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

	std::vector<Transaction> GetOpenTransactions(std::string p_exchangeId, std::string apiKey);

	void OpenShort();
	void OpenLong();
	void CloseShort();
	void CloseLong();

private: 
	ExchangeManager();
	~ExchangeManager();

	void _OnInit_CreateExchanges();

	std::vector<Exchange*> m_exchanges;
};