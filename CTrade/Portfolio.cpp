#include "Portfolio.h"

using std::string;

Portfolio::Portfolio(string p_owner) : m_owner(p_owner) {}

/*
\brief   Only add assets for which you are bullish in the 1-3 months timespan
*/
//void Portfolio::WatchAsset(string p_assetTicker)
//{
//	m_assetWatch.push_back(p_assetTicker);
//}
//
//const std::vector<string>& Portfolio::GetAssets()
//{
//	return m_assetWatch;
//}
