#pragma once

#include <vector>
#include <string>
#include <thread>
#include <memory>

#include "Portfolio.h"
#include "Client.h"

//The buy/sell signals are functions of current asset holdings, price of the asset, and available cash
//m_buySignals.push_back([this](const std::string p_asset) -> bool {
//  // Compute 7-day moving average
//  // if fifteen-minute price falls by 5% below weighted 7-day moving average in one hour
// 
//});

class ClientManager {
public:

	static bool userQuit;
	static bool userPause;

	bool RegisterClient(std::shared_ptr<Client> p_client);

private:
	ClientManager();
	~ClientManager();

	void _Manage();
	void _PollCompleteOrders(const std::shared_ptr<Client> p_client);
	void _PollOpenOrders(const std::shared_ptr<Client> p_client);
	void _ReconfigureFloatingLures(const std::shared_ptr<Client> p_client);
	void _CheckArbitrageOpportunities(const std::shared_ptr<Client> p_client);

	void _PruneForNewlyCompletedTransactions(std::string p_clientName, std::vector<Transaction>& p_transactions);

	std::thread managementThread;

	std::vector<std::shared_ptr<Client>> m_clients;

public:
	//Meyers' Singleton Implementation
	static ClientManager& Get() {
		static ClientManager M;
		return M;
	}
};