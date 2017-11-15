#include "ExchangeManager.h"

//Maybe move some of this garbage to an httpUtil or something
#include "StringUtil.h"

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

ExchangeManager::ExchangeManager() 
{
	m_exchanges.push_back(make_unique<Bittrex>());

	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		m_exchanges[i]->_InitURIs();
	}
}

std::vector<Transaction> ExchangeManager::GetOpenTransactions(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey)
{
	//p_exchangeId currently unused. All go to Bittrex.
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchangeId)
		{
			m_exchanges[i]->GetOpenOrders(p_publicKey, p_privateKey).wait();
		}
	}

	return std::vector<Transaction>();
}