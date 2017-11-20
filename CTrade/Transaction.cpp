#include <cmath>

#include "Transaction.h"

Transaction::Transaction(std::string p_transactionId, std::string p_fromAsset, std::string p_toAsset, 
		ETransactionType p_transactionType, double p_quantity, double p_limitPrice, std::string p_dateTimeOpened) : 
	m_transactionId(p_transactionId),
	m_fromAsset(p_fromAsset),
	m_toAsset(p_toAsset),
	m_transactionType(p_transactionType),
	m_quantity(p_quantity),
	m_openPriceUSD(p_limitPrice),
	m_openTime(p_dateTimeOpened),
	m_closeTime("N/A"),
	m_transactionState(EOpenTransaction)
{ /* NOP */ }

bool Transaction::IsNetGain()
{
	return (m_closePriceUSD > m_openPriceUSD);
}

void Transaction::Close(std::string p_closeTime)
{
	m_closeTime = p_closeTime;
}

double Transaction::EarningsInUSD()
{
	return m_closePriceUSD - m_openPriceUSD;
}

double Transaction::EarningsPercentage()
{
	/*This function does not yet have accurate math*/

	double change = (m_closePriceUSD / m_openPriceUSD);
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
