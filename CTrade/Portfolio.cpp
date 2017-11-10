#include "Portfolio.h"

using std::string;

Portfolio::Portfolio(string p_owner) : m_owner(p_owner) {}

void Portfolio::RegisterExchangeKey(string p_exchangeName, string p_exchangePublicKey, string p_exchangePrivateKey)
{
	m_exchangeKeys[p_exchangeName] = std::pair<string, string> (p_exchangePublicKey, p_exchangePrivateKey);
}

/*
\brief   Only add assets for which you are bullish in the 1-3 months timespan
*/
void Portfolio::AddAsset(string p_assetTicker)
{
	m_assetWatch.push_back(p_assetTicker);
}

void Portfolio::AddContactPhone(string p_phoneNumber)
{
	m_phoneNumber = p_phoneNumber;
}

const std::vector<string>& Portfolio::GetAssets()
{
	return m_assetWatch;
}
