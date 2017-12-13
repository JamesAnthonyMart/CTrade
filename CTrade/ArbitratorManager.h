#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

enum EArbitratorState { EArbitratorRunning, EArbitratorClosing };
enum EArbitratorSubState { EBuying, ESendA, ESelling, ESendB };

class Arbitrator 
{
public:
	Arbitrator(std::string p_identifier) : m_identifier(p_identifier) {}

	std::string GetIdentifier() { return m_identifier; }

private:
	std::string m_identifier;

	EArbitratorState m_state;
	EArbitratorSubState m_subState;
};

class ArbitratorCoin 
{
public:
	ArbitratorCoin(std::string p_name) : m_name(p_name) {}
	
	void SetPrice(std::string p_exchange, double p_price);

	std::string m_name;
	std::map<std::string, double> m_exchangePrices;
};

class ArbitratorManager 
{
public:
	static ArbitratorManager& Get() {
		static ArbitratorManager A;
		return A;
	}

	bool HasClient(std::string p_clientName);
	void SetArbitratorCoinPrice(std::string p_coinTicker, std::string p_exchange, double p_price);
	void GetSupportedArbitratorCoinNames(std::vector<std::string>& p_coinNames);
	void GetArbitrationObject(double p_openTriggerPercent, std::shared_ptr<Arbitrator> p_arbitrator);

private:
	ArbitratorManager();
	~ArbitratorManager() {}

	std::vector<Arbitrator> m_arbitrations;
	std::vector<ArbitratorCoin> m_supportedCoins;
};
