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
	Transaction(std::string p_transactionId, std::string p_fromAsset, std::string p_toAsset, ETransactionType p_transactionType,
		int p_quantity, double p_limitPrice, std::string p_dateTimeOpened);

	bool   IsNetGain();

	double EarningsInUSD();
	double EarningsPercentage();

private:

	//Variables on initialization
	std::string m_transactionId;
	std::string m_fromAsset;
	std::string m_toAsset;
	ETransactionType m_transactionType;
	int m_quantity;
	double m_openPriceUSD;
	std::string m_openTime;
	ETransactionState m_transactionState;

	//When closing, update these variables:
	double m_closePriceUSD;
	std::string m_closeTime;
};