#pragma once

#include <string.h>
#include <vector>
#include <map>

#include "Portfolio.h"
#include "DatabaseManager.h"

class ClientManager;

struct ManagementStrategy
{
	ManagementStrategy() : NotifyOnTradeCompletion(false), EnableArbitrage(false), EnableOrderTracking(false), EnableFloatingLures(false) {	}

	//Configurations
	bool NotifyOnTradeCompletion;
	bool EnableArbitrage;
	bool EnableOrderTracking;
	bool EnableFloatingLures;
};

struct ArbitrageConfiguration
{
	ArbitrageConfiguration() : OpenTrigger(-1.0), CloseTrigger(-1.0), AvailablePercentage(-1.0) {}

	double OpenTrigger;
	double CloseTrigger;
	double AvailablePercentage;
};

class AuthObject {
public:
	AuthObject() : publicKey(""), privateKey(""), passphrase("") {}
	AuthObject(std::string p_publicKey, std::string p_privateKey) : publicKey(p_publicKey), privateKey(p_privateKey), passphrase("") {}
	std::string publicKey;
	std::string privateKey;
	std::string passphrase;
};

class Client {
public:
	Client() : m_clientID(""), m_clientName(""), m_phoneNumber("") {}
	Client(std::string p_clientID) : m_clientID(p_clientID), m_clientName(""), m_phoneNumber("") {}
	
	void SetID(std::string p_ID) { m_clientID = p_ID; }
	std::string GetID() { return m_clientID; }

	void SetName(std::string p_name) { m_clientName = p_name; }
	std::string GetName() { return m_clientName; }

	void AddPortfolio(const Portfolio p_portfolio);
	Portfolio* GetPortfolio(std::string p_name);

	void RegisterAlertPhone(std::string p_phoneNumber) { m_phoneNumber = p_phoneNumber; }
	void RegisterExchangeKeys(std::string p_exchangeName, std::string p_exchangeKey, std::string p_exchangePrivateKey);
	void RegisterExchangePassphrase(std::string p_exchangeName, std::string p_exchangePassphrase);

	void GetUsedExchanges(std::vector<std::string>& p_RegisteredExchanges);

	std::string GetPublicKey(std::string p_exchangeName);
	std::string GetPrivateKey(std::string p_exchangeName);
	std::string GetPassphrase(std::string p_exchangeName);
	bool HasExchange(std::string p_exchangeName);

	ManagementStrategy m_ManagementStrategy;
	ArbitrageConfiguration m_ArbitrageConfiguration;

	bool Client::operator==(const Client &other) const 
	{
		return m_clientID == other.m_clientID;
	}

private:
	std::string m_clientID;
	std::string m_clientName;
	std::string m_phoneNumber;

	std::vector<Portfolio> m_portfolios;
	
	//			name	 AuthenticationObject
	std::map<std::string, AuthObject> m_exchangeKeys;
};