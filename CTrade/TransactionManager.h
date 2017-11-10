#pragma once

#include <vector>
#include <string>

#include "Transaction.h"

class TransactionManager 
{
public:
	static TransactionManager& Get() {
		static TransactionManager T;
		return T;
	}

	void OpenShort();
	//...etc for OpenLong and 
private:
	TransactionManager() {}
	std::vector<Transaction> m_transactions;
};