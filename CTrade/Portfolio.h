#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

struct ManagementStrategy
{
	ManagementStrategy() : NotifyOnTradeCompletion(false) {	}

	//Configurations
	bool NotifyOnTradeCompletion;
};

class Portfolio {
public:
	Portfolio(std::string p_owner);

	//void RegisterAlertPhone("### ### ####"); //Todo
	void RegisterExchangeKey(std::string p_exchangeName, std::string p_exchangeKey, std::string p_exchangePrivateKey);
	void AddAsset(std::string p_assetTicker);
	
	void AddContactPhone(std::string p_phoneNumber);

	const std::vector<std::string>& GetAssets();
	std::string GetOwner() const { return m_owner; } 	
	
	bool operator==(const Portfolio& p_right) { return m_owner == p_right.GetOwner(); }

	ManagementStrategy m_ManagementStrategy;

private:
	std::string m_owner;
	std::string m_phoneNumber;

	//			name			pub key				priv key
	std::map<std::string, std::pair<std::string, std::string>> m_exchangeKeys;
	std::vector<std::string> m_assetWatch;
};