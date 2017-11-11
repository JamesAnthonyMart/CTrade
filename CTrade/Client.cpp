#include "Client.h"
#include "ClientManager.h"

using std::string;

/* Manager Configuration (Give portfolios) */
void Client::AddPortfolio(const Portfolio p_portfolio)
{
	if (std::find(m_portfolios.begin(), m_portfolios.end(), p_portfolio) == m_portfolios.end())
	{
		m_portfolios.push_back(p_portfolio);
	}
}

Portfolio* Client::GetPortfolio(string p_name)
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

void Client::RegisterExchangeKeys(string p_exchangeName, string p_exchangePublicKey, string p_exchangePrivateKey)
{
	m_exchangeKeys[p_exchangeName] = std::pair<string, string>(p_exchangePublicKey, p_exchangePrivateKey);
}

std::string Client::GetPublicKey(string p_exchangeName)
{
	string sReturn("UNKOWN");
	auto it = m_exchangeKeys.find(p_exchangeName);
	if (it != m_exchangeKeys.end())
	{
		sReturn = it->second.first;
	}
	return sReturn;
}

std::string Client::GetPrivateKey(string p_exchangeName)
{
	string sReturn("UNKNOWN");
	auto it = m_exchangeKeys.find(p_exchangeName);
	if (it != m_exchangeKeys.end())
	{
		sReturn = it->second.second;
	}
	return sReturn;
}
