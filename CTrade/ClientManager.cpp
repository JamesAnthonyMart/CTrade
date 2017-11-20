#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include "ClientManager.h"
#include "ExchangeManager.h"

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

void ClientManager::AddClient(shared_ptr<Client> p_client)
{
	m_clients.push_back(p_client);
}

void ClientManager::_Manage()
{
	while (!ClientManager::userQuit)
	{
		std::for_each(m_clients.begin(), m_clients.end(), [this](shared_ptr<Client> c) {
			//todo: try { } catch (Exception NoApiKeysConfigured) { ... }
			_PollCompleteOrders(c);
			_PollOpenOrders(c);
			_ReconfigureFloatingLures(c);
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
	cout << "****************************************************" << std::endl;
	cout << "         Getting order history..." << endl; //Alert user if anything new appears.
	cout << "****************************************************" << std::endl;
	vector<string> usedExchanges;
	p_client->GetUsedExchanges(usedExchanges);
	for (auto exchange : usedExchanges)
	{
		vector<Transaction> transactionHistory;
		ExchangeManager::Get().GetTransactionHistory(exchange, p_client->GetPublicKey(exchange), p_client->GetPrivateKey(exchange), transactionHistory);
	}
}
void ClientManager::_PollOpenOrders(const std::shared_ptr<Client> p_client)
{
	cout << "****************************************************" << std::endl;
	cout << "         Getting currently open orders..." << endl;
	cout << "****************************************************" << std::endl;
	vector<string> usedExchanges;
	p_client->GetUsedExchanges(usedExchanges);
	for (auto exchange : usedExchanges)
	{
		vector<Transaction> openTransactions;
		ExchangeManager::Get().GetOpenTransactions(exchange, p_client->GetPublicKey(exchange), p_client->GetPrivateKey(exchange), openTransactions);
	}
}

void ClientManager::_ReconfigureFloatingLures(const std::shared_ptr<Client> p_client)
{
	//cout << "Get orders configured for floating lures, update if necessary." << endl;
}