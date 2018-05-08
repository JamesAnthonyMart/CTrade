#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include "ClientManager.h"
#include "ExchangeManager.h"
#include "DatabaseManager.h"
#include "ArbitratorManager.h"
#include "Output.h"


using std::cout;
using std::endl;

using std::string;
using std::vector;
using std::shared_ptr;


bool ClientManager::userQuit = false;
bool ClientManager::userPause = false;

ClientManager::ClientManager()
{
	managementThread = std::thread(&ClientManager::_Manage, this);
}

ClientManager::~ClientManager()
{
	ClientManager::userQuit = true;
	managementThread.join();
}

bool ClientManager::RegisterClient(shared_ptr<Client> p_client)
{
	bool clientAlreadyExists = false;
	for (std::shared_ptr<Client> client : m_clients)
	{
		if (*client == *p_client)
		{
			clientAlreadyExists = true;
			break;
		}
	}
	
	if (!clientAlreadyExists)
	{
		m_clients.push_back(p_client);
	}
	
	return !clientAlreadyExists;
}

void ClientManager::_Manage()
{
	while (!ClientManager::userQuit)
	{
		std::for_each(m_clients.begin(), m_clients.end(), [this](shared_ptr<Client> c) 
		{
			int enabledFeatures = 0;
			//todo: try { } catch (Exception NoApiKeysConfigured) { ... }
			if (c->m_ManagementStrategy.EnableOrderTracking)
			{
				enabledFeatures++;
				_PollCompleteOrders(c);
				_PollOpenOrders(c);
			}
			if (c->m_ManagementStrategy.EnableFloatingLures)
			{
				enabledFeatures++;
				_ReconfigureFloatingLures(c);
			}
			if (c->m_ManagementStrategy.EnableArbitrage)
			{
				enabledFeatures++;
				_CheckArbitrageOpportunities(c);
			}

			if (enabledFeatures == 0)
			{
				Output::Info("Client with ID: " + c->GetID() + ": No services enabled!");
			}
		});
		Sleep(5000);

		while (ClientManager::userPause)
		{
			Sleep(500);
		}

	}
}

void ClientManager::_PollCompleteOrders(const shared_ptr<Client> p_client)
{
	cout << endl << "Getting order history..." << endl; //Alert user if anything new appears.
	
	// For each exchange the client uses
	vector<string> usedExchanges;
	p_client->GetUsedExchanges(usedExchanges);
	for (auto exchange : usedExchanges)
	{
		vector<Transaction> transactionHistory;
		ExchangeManager::Get().GetTransactionHistory(exchange, p_client->GetPublicKey(exchange), p_client->GetPrivateKey(exchange), transactionHistory);

		_PruneForNewTransactions(p_client->GetID(), transactionHistory);

		// If there are any new transactions to record
		if (transactionHistory.size() > 0)
		{
			// Temporary workaround to adhere to the filemanager API
			vector<std::shared_ptr<Transaction>> transactionHistoryPtrs;
			for (size_t i = 0; i < transactionHistory.size(); ++i)
				transactionHistoryPtrs.push_back(std::make_shared<Transaction>(transactionHistory[i]));

			// Write it to the client's file
			DatabaseManager::Get().RecordTransactions(p_client->GetID(), transactionHistoryPtrs);

			std::cout << "   " << exchange << ": " << transactionHistory.size() << " closed transactions." << std::endl;
		}
	}
}
void ClientManager::_PollOpenOrders(const std::shared_ptr<Client> p_client)
{
	cout << "Getting currently open orders..." << endl;
	vector<string> usedExchanges;
	p_client->GetUsedExchanges(usedExchanges);
	for (auto exchange : usedExchanges)
	{
		vector<Transaction> openTransactions;
		ExchangeManager::Get().GetOpenTransactions(exchange, p_client->GetPublicKey(exchange), p_client->GetPrivateKey(exchange), openTransactions);
		std::cout << "   " << exchange << ": " << openTransactions.size() << " open transactions." << std::endl;
	}
}

void ClientManager::_ReconfigureFloatingLures(const std::shared_ptr<Client> p_client)
{
	std::cout << "Reconfigure floating lures on Bittrex. (Incomplete)" << endl;
}

void ClientManager::_CheckArbitrageOpportunities(const std::shared_ptr<Client> p_client)
{
	std::cout << "Find and execute arbitrages on ETH and LTC through Bittrex and GDAX. (Incomplete)" << endl;

	// Check that an arbitrator doesn't already exist for this client
	if (ArbitratorManager::Get().HasClient(p_client->GetID()))
		return;

	// Check that the client has both Bittrex and GDAX
	if (!(p_client->HasExchange("Bittrex") && p_client->HasExchange("GDAX")))
		return;

	// Update the price of each arbitrageable coin on each supported exchange
	std::vector<std::string> exchangeManagerSupportedExchanges;
	ExchangeManager::Get().GetSupportedExchangeNames(exchangeManagerSupportedExchanges);
	std::vector<std::string> arbitratorManagerSupportedCoins;
	ArbitratorManager::Get().GetSupportedArbitratorCoinNames(arbitratorManagerSupportedCoins);
	for (size_t i = 0; i < arbitratorManagerSupportedCoins.size(); ++i)
	{
		std::string coinTicker = arbitratorManagerSupportedCoins[i];

		for (size_t j = 0; j < exchangeManagerSupportedExchanges.size(); ++j)
		{
			std::string exchange = exchangeManagerSupportedExchanges[j];
			double coinPrice = ExchangeManager::Get().GetPriceOnExchange(coinTicker, exchange);
			ArbitratorManager::Get().SetArbitratorCoinPrice(coinTicker, exchange, coinPrice);
			std::cout << "   " << exchange << ": " << coinTicker << " is trading for " << coinPrice << std::endl;
		}
	}

	// If there exists an arbitrage opportinity over the threshold set by the client
	std::shared_ptr<Arbitrator> arbitrator = nullptr;
	ArbitratorManager::Get().GetArbitrationObject(p_client->m_ArbitrageConfiguration.OpenTrigger, arbitrator);
	if (arbitrator)
	{
		std::cout << "Arbitrage opportunity detected." << std::endl;
		// ARBITRATION TODO!!!!
		// Arbitrator object is responsible for:
		// * Continuing to check whether the arbitrage opportunity exists
		// * Keeps track of its own state (Running, Closing)
		// * Keeps track of its substate (Buying, SendA, Selling, SendB)	
	}
}

void ClientManager::_PruneForNewTransactions(std::string p_clientName, std::vector<Transaction>& p_transactions)
{
	std::vector<Transaction> recordedTransactions;
	DatabaseManager::Get().GetRecordedTransactions(p_clientName, recordedTransactions);
}
