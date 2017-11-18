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
#include <memory>

#include "Transaction.h"
#include "Exchange.h"

/*
The actual exchange being used should be irrelevant to the Manager, unless the manager is trying to engage in 
  arbitrage opportunities. Otherwise, he should just be able to open positions on assets without caring what exchange 
  is used.
*/
class ExchangeManager
{
public:

	friend class Exchange;

	static ExchangeManager& Get() {
		static ExchangeManager E;
		return E;
	}

	void GetOpenTransactions(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey, std::vector<Transaction>& p_openTransactions);

private: 
	ExchangeManager();
	~ExchangeManager();

	std::vector<std::unique_ptr<Exchange>> m_exchanges;
};
