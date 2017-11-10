#include "Manager.h"

#include <algorithm>
#include <Windows.h>

#include "ExchangeManager.h"

//For console printing
#include <iostream>
using std::cout;
using std::endl;

using std::string;


bool Manager::userQuit = false;

Manager::Manager()
{
	managementThread = std::thread(&Manager::Manage, this);
}

Manager::~Manager()
{
	Manager::userQuit = true;
	managementThread.join();
}

void Manager::Manage()
{
	while (!Manager::userQuit)
	{
		cout << "Get order history, alert user if new order appeared." << endl;
		cout << "Get currently open orders, update all open orders display." << endl;
		cout << "Get orders configured for floating lures, update if necessary." << endl;
		Sleep(5000);
	}
	//Updates all those asset prices
	//_UpdateAllAssetPrices();

	//For each portfolio, the manager determines what it should do based on buy/sell conditions, and updates the orders on Bittrex appropriately
	//_UpdateOrders();
}

void Manager::AddPortfolio(const Portfolio p_portfolio)
{
	if (std::find(m_portfolios.begin(), m_portfolios.end(), p_portfolio) == m_portfolios.end())
	{
		m_portfolios.push_back(p_portfolio);
	}
}

Portfolio* Manager::GetPortfolio(std::string p_name)
{
	for (size_t i = 0; i < m_portfolios.size(); ++i)
	{
		if (m_portfolios[i].GetOwner() == p_name)
		{
			return &m_portfolios[i];
		}
	}
	
	return nullptr;
}

void Manager::PollOrdersComplete() 
{
	
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