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
	}
}

void ClientManager::_PollCompleteOrders(const shared_ptr<Client> p_client)
{
	cout << "Get order history, alert user if new order appeared." << endl;
	vector<string> usedExchanges;
	p_client->GetUsedExchanges(usedExchanges);
	for (auto exchange : usedExchanges)
	{
		ExchangeManager::Get().GetOpenTransactions(exchange, p_client->GetPublicKey(exchange), p_client->GetPrivateKey(exchange));
	}
}
void ClientManager::_PollOpenOrders(const std::shared_ptr<Client> p_client)
{
	//cout << "Get currently open orders, update all open orders display." << endl;
}

void ClientManager::_ReconfigureFloatingLures(const std::shared_ptr<Client> p_client)
{
	//cout << "Get orders configured for floating lures, update if necessary." << endl;
}







//void Manager::_UpdateOrders()
//{
//	std::for_each(m_portfolios.begin(), m_portfolios.end(), [this](Portfolio& p_portfolio) 
//	{
//		const std::vector<std::string>& portfolioAssets = p_portfolio.GetAssets();
//		for (int i = 0; i < portfolioAssets.size(); ++i) 
//		{
//			portfolioAssets[i]
//
//			_ModifyExistingOrders();
//			_OpenNewBuyOrders();  //Long orders
//			_OpenNewSellOrders(); //Short orders
//
//
//		}
//	});
//}

//void Manager::_ModifyExistingOrders()
//{
//	//If orders closed since last checked, update StatisticsManager with new statistics and send update report to portfolio holder's contact details.
//	_CheckExistingOrderStatus();
//
//	/* modify conditions could indicate:
//	* Reprice a long order
//	* Reprice a short order
//	* Cancel a long
//	* Cancel a short
//
//	All should happen with TransactionManager::Get()->CancelX and OpenX
//	*/
//
//
//}

//void Manager::_OpenNewShortOrders()
//{
//	//If sell conditions met
//	TransactionManager::Get()->OpenShort(...);
//}
//
//void Manager::_OpenNewLongOrders()
//{
//	//If buyconditionsMet
//
//	//Additionally, when an order is initiated, we do something like
//	TransactionManager::Get()->OpenLong(portfolioName, fromAssetName, fromAssetPriceUSD, fromAssetAmount, toAssetName, toAssetPrice); //toAssetAmount is calculated using the fromAssetAmount and the toAssetPrice
//}