#include "ClientManager.h"

#include <algorithm>
#include <Windows.h>

#include "ExchangeManager.h"

//For console printing
#include <iostream>
using std::cout;
using std::endl;

using std::string;
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
			_PollCompleteOrders(c);
		});
		

		cout << "Get currently open orders, update all open orders display." << endl;
		cout << "Get orders configured for floating lures, update if necessary." << endl;
		Sleep(5000);
	}
	//Updates all those asset prices
	//_UpdateAllAssetPrices();

	//For each client, the manager determines what it should do based on buy/sell conditions, and updates the orders on Bittrex appropriately
	//_UpdateOrders();
}

void ClientManager::_PollCompleteOrders(const shared_ptr<Client> p_client)
{
	cout << "Get order history, alert user if new order appeared." << endl;
	//ExchangeManager::Get().GetOpenTransactions(client->ecxhangekeys)

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