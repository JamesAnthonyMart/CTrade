#include "ArbitratorManager.h"

#include <cassert>
#include <cmath>

bool ArbitratorManager::HasClient(std::string p_clientName)
{
	for (size_t i = 0; i < m_arbitrations.size(); ++i)
	{
		if (m_arbitrations[i].GetIdentifier() == p_clientName)
		{
			return true;
		}
	}
	return false;
}

void ArbitratorManager::SetArbitratorCoinPrice(std::string p_coinTicker, std::string p_exchange, double p_price)
{
	bool bFoundTheCoin = false;
	for (size_t i = 0; i < m_supportedCoins.size(); ++i)
	{
		if (p_coinTicker == m_supportedCoins[i].m_name)
		{
			bFoundTheCoin = true;
			m_supportedCoins[i].SetPrice(p_exchange, p_price);
		}
	}

	//Until I find a need which proves the contrary, only let this function run on tickers which I already know that I support
	assert(bFoundTheCoin);
}

void ArbitratorManager::GetSupportedArbitratorCoinNames(std::vector<std::string>& p_coinNames)
{
	p_coinNames.clear();
	for (size_t i = 0; i < m_supportedCoins.size(); ++i)
	{
		p_coinNames.push_back(m_supportedCoins[i].m_name);
	}
}

void ArbitratorManager::GetArbitrationObject(double p_openTriggerPercent, std::shared_ptr<Arbitrator> p_arbitrator)
{
	p_arbitrator = nullptr;
	
	// For each coin
	for (size_t i = 0; i < m_supportedCoins.size(); ++i)
	{
		ArbitratorCoin* coin = &m_supportedCoins[i];
		
		// Shortcircuit case
		if (coin->m_exchangePrices.size() == 0) return;

		// Find the exchange which charges the least, and the exchange which charges the most
		auto it = coin->m_exchangePrices.begin();
		auto minIterator = it;
		auto maxIterator = it;
		for (it; it != coin->m_exchangePrices.end(); ++it)
		{
			double exchangePrice = it->second;
			if (exchangePrice > maxIterator->second) maxIterator = it;
			if (exchangePrice < minIterator->second) minIterator = it;
		}

		// Shortcircuit case
		double maxPrice = maxIterator->second;
		double minPrice = minIterator->second;
		double bigConstant = 100000000.0;
		if (std::floor(maxPrice * bigConstant) <= std::floor(minPrice * bigConstant)) return;

		// If the ratio is greater than the openTriggerPercent
		double percentageGreater = (maxPrice / minPrice) - 1.0;
		if (percentageGreater >= p_openTriggerPercent)
		{
			// Create an arbitrator object
			p_arbitrator = std::make_shared<Arbitrator>("Baby's first arbitrator");
		}
	}
}

ArbitratorManager::ArbitratorManager()
{
	m_supportedCoins.push_back(ArbitratorCoin("LTC"));
	m_supportedCoins.push_back(ArbitratorCoin("ETH"));
}

void ArbitratorCoin::SetPrice(std::string p_exchange, double p_price)
{
	m_exchangePrices[p_exchange] = p_price;
}
