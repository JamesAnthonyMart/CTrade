#pragma once

#include <string>

class Transaction {
public:
	Transaction(std::string p_fromAsset, std::string p_toAsset)
	{
		//Todo
		//m_openTime = Time::now();
	}

	bool   IsNetGain();

	double EarningsInUSD();
	double EarningsPercentage();

private:
	double m_openTime;
	double m_closeTime;

	double m_openPrice;
	double m_closePrice;

	std::string m_fromAsset;
	std::string m_toAsset;
};