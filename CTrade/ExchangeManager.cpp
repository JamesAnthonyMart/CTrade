#include "ExchangeManager.h"

//Maybe move some of this garbage to an httpUtil or something
#include "StringUtil.h"

#include <cassert>

using std::string;
using std::make_unique;

/*
"Uuid": null,
"OrderUuid": "39cd4591-5571-48ee-b0e5-366d9e78cb15",
"Exchange": "BTC-SC",
"OrderType": "LIMIT_SELL",
"Quantity": 11000,
"QuantityRemaining": 11000,
"Limit": 0.00000296,
"CommissionPaid": 0,
"Price": 0,
"PricePerUnit": null,
"Opened": "2017-10-25T18:36:22.82",
"Closed": null,
"CancelInitiated": false,
"ImmediateOrCancel": false,
"IsConditional": false,
"Condition": "NONE",
"ConditionTarget": null*/

ExchangeManager::~ExchangeManager() 
{
	//NOP Virtual destructor
}

bool ExchangeManager::_SupportsExchange(std::string p_exchange)
{
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchange)
		{
			return true;
		}
	}

	return false;
}

ExchangeManager::ExchangeManager() 
{
	m_exchanges.push_back(make_unique<Bittrex>());

	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		m_exchanges[i]->_InitURIs();
	}
}

void ExchangeManager::GetOpenTransactions(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey, std::vector<Transaction>& p_openTransactions){
	//p_exchangeId currently unused. All go to Bittrex.

	p_openTransactions.clear();
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchangeId)
		{
			std::shared_ptr<std::vector<Transaction>> exchangeOpenOrders = std::make_shared<std::vector<Transaction>>();
			m_exchanges[i]->GetOpenOrders(p_publicKey, p_privateKey, exchangeOpenOrders).wait();
			for (size_t j = 0; j < exchangeOpenOrders->size(); ++j)
			{
				p_openTransactions.push_back((*exchangeOpenOrders)[j]);
			}
		}
	}
}

void ExchangeManager::GetTransactionHistory(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey, std::vector<Transaction>& p_transactionHistory)
{
	//p_exchangeId currently unused. All go to Bittrex.

	p_transactionHistory.clear();
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchangeId)
		{
			std::shared_ptr<std::vector<Transaction>> exchangeTransHistory = std::make_shared<std::vector<Transaction>>();
			m_exchanges[i]->GetTransactionHistory(p_publicKey, p_privateKey, exchangeTransHistory).wait();
			for (size_t j = 0; j < exchangeTransHistory->size(); ++j)
			{
				p_transactionHistory.push_back((*exchangeTransHistory)[j]);
			}
		}
	}
}

void ExchangeManager::GetSupportedExchangeNames(std::vector<std::string>& p_exchangeNames)
{
	p_exchangeNames.clear();
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		p_exchangeNames.push_back(m_exchanges[i]->GetName());
	}
}

double ExchangeManager::GetPriceOnExchange(std::string p_coinTicker, std::string p_exchange)
{
	assert(_SupportsExchange(p_exchange));

	double price = -1.0;
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchange)
		{
			m_exchanges[i]->GetPrice(p_coinTicker, &price).wait();
			return price;
		}
	}

	//This doesn't make sense to hit. The first assert would catch the case where the exchange isn't supported,
	//  and if the exchange IS supported, this should never get hit, right?
	assert(false);
	return price;

}
