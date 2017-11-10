#include "TransactionManager.h"

#include "ExchangeManager.h"

void TransactionManager::OpenShort()
{
	ExchangeManager::Get().OpenShort();
	//m_shortOrders.push_back(orderDetails, time.now());
}
