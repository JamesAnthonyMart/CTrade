#pragma once

#include <vector>
#include <string>

class Portfolio {
public:
	Portfolio(std::string p_owner);
	std::string GetOwner() const { return m_owner; }
	
	//void WatchAsset(std::string p_assetTicker);
	//const std::vector<std::string>& GetAssets();

	bool operator==(const Portfolio& p_right) { return m_owner == p_right.GetOwner(); }

private:
	std::string m_owner;

	std::vector<std::string> m_assetWatch;
};