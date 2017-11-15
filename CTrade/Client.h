#pragma once

#include <string.h>
#include <vector>
#include <map>

#include "Portfolio.h"

class ClientManager;

struct ManagementStrategy
{
	ManagementStrategy() : NotifyOnTradeCompletion(false) {	}

	//Configurations
	bool NotifyOnTradeCompletion;
};


class Client {
public:
	Client(std::string p_ClientName) {}
	std::string GetName() { return m_clientName; }

	void AddPortfolio(const Portfolio p_portfolio);
	Portfolio* GetPortfolio(std::string p_name);

	void RegisterAlertPhone(std::string p_phoneNumber) { m_phoneNumber = p_phoneNumber; }
	void RegisterExchangeKeys(std::string p_exchangeName, std::string p_exchangeKey, std::string p_exchangePrivateKey);

	void GetUsedExchanges(std::vector<std::string>& p_RegisteredExchanges);

	std::string GetPublicKey(std::string p_exchangeName);
	std::string GetPrivateKey(std::string p_exchangeName);

	ManagementStrategy m_ManagementStrategy;

private:
	std::string m_clientName;
	std::string m_phoneNumber;

	std::vector<Portfolio> m_portfolios;
	
	//			name			pub key				priv key
	std::map<std::string, std::pair<std::string, std::string>> m_exchangeKeys;
};