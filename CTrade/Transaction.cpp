#include <cmath>

#include "Transaction.h"

Transaction::Transaction(std::string p_transactionId, std::string p_exchange, ETransactionType p_transactionType, std::string p_fromAsset, 
						 std::string p_toAsset, double p_rate, double p_rateUSD, double p_quantity, std::string p_dateTimeOpened) :
	m_transactionId(p_transactionId),
	m_exchange(p_exchange),
	m_transactionType(p_transactionType),
	m_fromAsset(p_fromAsset),
	m_toAsset(p_toAsset),
	m_rate(p_rate),			//price per to-asset in terms of from-asset at time of transaction
	m_quantity(p_quantity),
	m_rateUSD(p_rateUSD),	//price per to-asset in terms of USD        at time of transaction
	m_openTime(p_dateTimeOpened),
	m_closeTime("N/A"),
	m_transactionState(EOpenTransaction)
{ /* NOP */ }

void Transaction::Close(std::string p_closeTime)
{
	m_closeTime = p_closeTime;
	m_transactionState = EClosedTransaction;
}
