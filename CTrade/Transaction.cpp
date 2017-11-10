#include <cmath>

#include "Transaction.h"

bool Transaction::IsNetGain()
{
	return (m_closePrice > m_openPrice);
}

double Transaction::EarningsInUSD()
{
	return m_closePrice - m_openPrice;
}

double Transaction::EarningsPercentage()
{
	/*This function does not yet have accurate math*/

	double change = (m_closePrice / m_openPrice);
	if (change > 1.0)
	{
		change -= 1.0;
	}
	else if (change == 1.0)
	{
		change = 0.0;
	}
	else
	{
		change = -1*(1.0 - change);
	}

	return (std::round(change * 100.0) / 100.0) - 1.0;

	//value = round(value * 1000.0) / 1000.0;
}
