#pragma once

#include "Portfolio.h"
#include <vector>
#include <string>
#include <thread>

//The buy/sell signals are functions of current asset holdings, price of the asset, and available cash
//m_buySignals.push_back([this](const std::string p_asset) -> bool {
//  // Compute 7-day moving average
//  // if fifteen-minute price falls by 5% below weighted 7-day moving average in one hour
// 
//});

class Manager {
public:

	//Initialization
	void AddPortfolio(const Portfolio p_portfolio);
	Portfolio* GetPortfolio(std::string p_name);

	void Manage();
	void PollOrdersComplete();

	static bool userQuit;

private:
	Manager();
	~Manager();

	std::thread managementThread;

	std::vector<Portfolio> m_portfolios;

public:
	//Meyers' Singelton Implementation
	static Manager& Get() {
		static Manager M;
		return M;
	}
};