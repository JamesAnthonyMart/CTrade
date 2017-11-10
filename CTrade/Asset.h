#pragma once


/*
Not convinced this class is actually useful.
*/

#include <string>
#include <vector>

class Asset 
{
public:
	Asset(std::string p_ticker) : m_ticker(p_ticker) {}

	//void GetPrice(std::vector<double>& p_price);

private:
	std::string m_ticker;
	std::vector<double> m_priceHistory;
};