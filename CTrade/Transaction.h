#pragma once

#include <string>

enum ETransactionState {
	EOpenTransaction, 
	EClosedTransaction,
};

enum ETransactionType {
	ELimitBuy,
	ELimitSell,
	EUnsupportedType
};

class Transaction {
public:
	Transaction(std::string p_transactionId, std::string p_exchange, ETransactionType p_transactionType, std::string p_fromAsset,
		std::string p_toAsset, double p_rate, double p_rateUSD, double p_quantity, std::string p_dateTimeOpened);

	void Close(std::string p_closeTime);

	std::string GetTransactionID()			{ return m_transactionId; }
	std::string GetExchange()				{ return m_exchange; }
	ETransactionType GetTransactionType()	{ return m_transactionType; }
	std::string GetFromAsset()				{ return m_fromAsset; }
	std::string GetToAsset()				{ return m_toAsset; }
	double GetRate()						{ return m_rate; }
	double GetRateUSD()						{ return m_rateUSD; }
	double GetQuantity()					{ return m_quantity; }
	std::string GetOpenTime()				{ return m_openTime; }
	ETransactionState GetTransactionState() { return m_transactionState; }
	std::string GetCloseTime()				{ return m_closeTime; }

	bool Transaction::operator==(const Transaction &other) const
	{
		return m_transactionId == other.m_transactionId;
	}

private:
	std::string m_transactionId;
	std::string m_exchange;
	ETransactionType m_transactionType;
	std::string m_fromAsset;
	std::string m_toAsset;
	double m_rate;
	double m_rateUSD;
	double m_quantity;
	std::string m_openTime;

	//When closing, update these variables:
	ETransactionState m_transactionState;
	std::string m_closeTime;
};