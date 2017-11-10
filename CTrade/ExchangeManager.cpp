#include "ExchangeManager.h"

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


ExchangeManager::ExchangeManager() {}

ExchangeManager::~ExchangeManager()
{
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		delete m_exchanges[i];
	}
	m_exchanges.clear();
}

std::vector<Transaction> ExchangeManager::GetOpenTransactions(std::string p_exchangeId, std::string apiKey)
{
	//Todo
	std::vector<Transaction> temp;
	return temp;
	
	//return m_exchanges[p_exchangeId]->GetOpenTransactions(apiKey);
}

void ExchangeManager::OpenShort()
{
	//Todo
}

void ExchangeManager::OpenLong()
{
	//Todo
}

void ExchangeManager::CloseShort()
{
	//Todo
}

void ExchangeManager::CloseLong()
{
	//Todo
}

void ExchangeManager::_OnInit_CreateExchanges()
{
	m_exchanges.push_back(new BittrexClient("Bittrex", "asdflkmsdf"));
}
