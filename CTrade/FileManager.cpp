#include "FileManager.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <string>

#include<memory>
#include<boost/lexical_cast.hpp>

#include "Client.h"
#include "Transaction.h"
#include "Output.h"
#include "StringUtil.h"


using std::cout;
using std::endl;
using std::vector;

FileManager::FileManager() :   m_dataDirectoryPath("")
							 , m_dataFileName("")
{
	m_workingFilePath = FileManager::_GetCurrentWorkingDir();
}

bool FileManager::RecordTransactionsInFile(std::string p_clientName, const std::vector<std::shared_ptr<Transaction>>& p_transactions)
{	
	std::shared_ptr<pugi::xml_document> doc = m_clientFiles[p_clientName];
	
	//breakpoint because I need to see if doc is nullptr if the clientname isn't in the clientfiles map
	std::string fullFileName = m_dataDirectoryPath + _ClientFileName(p_clientName);

	if (doc == nullptr)
	{
		if (!_GetClientTransactionDoc(fullFileName, doc)) //Creates the document if not found
			return false;	//Return false if creation failed
	}

	pugi::xml_node TransactionHistory = doc->child("Client").child("TransactionHistory");

	for (size_t i = 0; i < p_transactions.size(); ++i)
	{
		auto t = p_transactions[i];

		//  Todo: I think I should pass the pugi node into some public transaction function, which then attaches
		//    its information into the pugi node. This function shouldn't really know how to create a transaction node, 
		//    it should just facilitate the creation.
		pugi::xml_node newTransaction = TransactionHistory.append_child("Transaction");
		newTransaction.append_attribute("transId").set_value(t->GetTransactionID().c_str());
		newTransaction.append_attribute("exchange").set_value(t->GetExchange().c_str());
		newTransaction.append_attribute("type").set_value((t->GetTransactionType() == ELimitBuy)? "Buy" : "Sell");
		newTransaction.append_attribute("fromasset").set_value(t->GetFromAsset().c_str());
		newTransaction.append_attribute("toasset").set_value(t->GetToAsset().c_str());
		newTransaction.append_attribute("rate").set_value(t->GetRate());       // limit price (number of from assets for every 1 toasset)
		newTransaction.append_attribute("rateUSD").set_value(t->GetRateUSD()); // limit price (number of USD	      for every 1 toasset)
		newTransaction.append_attribute("amount").set_value(t->GetQuantity());
		newTransaction.append_attribute("opentime").set_value(t->GetOpenTime().c_str());
		newTransaction.append_attribute("closetime").set_value(t->GetCloseTime().c_str());
	}
	
	doc->save_file(fullFileName.c_str());

	return true;
}

bool FileManager::LoadClientData(std::vector<std::shared_ptr<Client>>& p_clients)
{	
	// Ensure the data directory path is specified
	if (m_dataDirectoryPath == "")
	{
		Output::Error("No data directory path specified. You must choose the path to store the program data.");
	}

	// Ensure client data file specified
	if (m_dataFileName == "")
	{
		Output::Error("No client data file specified. Cannot load client.");
		return false;
	}

	// Ensure specified client data file exists
	std::shared_ptr<pugi::xml_document> doc;
	bool bDocumentLoaded = _GetClientInfoDoc(doc);
	if (!bDocumentLoaded) return false;

	p_clients.clear();
	// Get client data from file
	for (pugi::xml_node ClientNode = doc->child("Client"); ClientNode; ClientNode = ClientNode.next_sibling())
	{
		std::string currentClientID = ClientNode.attribute("id").value();
		std::shared_ptr<Client> client = std::make_shared<Client>(currentClientID);

		// Get Phone number
		std::string phoneNumber = ClientNode.attribute("phoneNumber").value();
		if (phoneNumber != "")
		{
			client->RegisterAlertPhone(phoneNumber);
			//client->m_ManagementStrategy.NotifyOnTradeCompletion = true; //just because there's a phone number doesn't mean they want notifications...
		}

		// Get exchanges
		pugi::xml_node ExchangesNode = ClientNode.child("Exchanges");
		for (pugi::xml_node xmlExchangeNode : ExchangesNode.children("Exchange"))
		{
			std::string exchangeName = xmlExchangeNode.attribute("name").value();
			std::string exchangePublicKey = xmlExchangeNode.attribute("publicKey").value();
			std::string exchangePrivateKey = xmlExchangeNode.attribute("privateKey").value();
			std::string exchangePassphrase = "";
			client->RegisterExchangeKeys(exchangeName, exchangePublicKey, exchangePrivateKey);
			if (exchangeName == "GDAX")
			{
				exchangePassphrase = xmlExchangeNode.attribute("passphrase").value();
				client->RegisterExchangePassphrase(exchangeName, exchangePassphrase);
			}
		}

		// Get Arbitrage Configuration
		if (client->HasExchange("Bittrex") && client->HasExchange("GDAX"))
		{	// Arbitrage is currently only supported between these two exchanges
			pugi::xml_node ArbitrageConfigNode = ClientNode.child("ArbitrageConfig");
			std::string enabled = ArbitrageConfigNode.attribute("enabled").value();
			if (enabled == "true")
			{
				client->m_ManagementStrategy.EnableArbitrage = true;
				double openTrigger = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("openTrigger").value()) / 100.0;
				double closeTrigger = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("closeTrigger").value()) / 100.0;
				double availablePercentage = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("availablePercentage").value()) / 100.0;
				client->m_ArbitrageConfiguration.OpenTrigger = openTrigger;
				client->m_ArbitrageConfiguration.CloseTrigger = closeTrigger;
				client->m_ArbitrageConfiguration.AvailablePercentage = availablePercentage;
			}
		}

		p_clients.push_back(client);
	}

	if (!(p_clients.size()))
	{
		Output::Error("No Clients could be found!");
		return false;
	}

	return true;
}

bool FileManager::GetRecordedTransactions(std::string p_client, std::vector<Transaction>& p_Transactions)
{

	std::shared_ptr<pugi::xml_document> doc = m_clientFiles[p_client];

	std::string fullFileName = m_dataDirectoryPath + _ClientFileName(p_client);

	if (doc == nullptr)
	{
		if (!_GetClientTransactionDoc(fullFileName, doc)) //Creates the document if not found
			return false;	//Return false if creation failed
		
		m_clientFiles[p_client] = doc;
	}
	
	pugi::xml_node TransactionHistory = doc->child("Client").child("TransactionHistory");

	
	for (pugi::xml_node transaction : TransactionHistory.children("Transaction"))
	{
		//Todo - I might want to move create-transaction to a static function in the Transaction class, since a filemanager
		//  shouldn't really know how to create a transaction.
		p_Transactions.push_back(_CreateTransaction(transaction));
	}
}

std::string FileManager::_GetCurrentWorkingDir()
{
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	current_working_dir.append("\\");
	return current_working_dir;
}

bool FileManager::_GetClientInfoDoc(std::shared_ptr<pugi::xml_document> p_doc)
{
	std::string fileName = m_dataDirectoryPath + m_dataFileName;
	pugi::xml_parse_result result = p_doc->load_file(fileName.c_str());
	
	bool bCreateFileNeeded = (result.status == pugi::xml_parse_status::status_file_not_found);
	if (bCreateFileNeeded)
	{
		Output::Info("File " + fileName + " not found, creating file...");

		p_doc = std::make_shared<pugi::xml_document>();
		
		pugi::xml_node Client = p_doc->append_child("Client");
		Client.append_attribute("id") = "01";
		Client.append_attribute("name") = "";
		pugi::xml_node Exchanges = Client.append_child("Exchanges");
		
		//Todo - get needed attributes from ExchangeManager
		pugi::xml_node BittrexExchange = Exchanges.append_child("Exchange");
		BittrexExchange.append_attribute("name") = "Bittrex";
		BittrexExchange.append_attribute("publicKey") = "";
		BittrexExchange.append_attribute("privateKey") = "";
		pugi::xml_node GDAXExchange = Exchanges.append_child("Exchange");
		BittrexExchange.append_attribute("name") = "Bittrex";
		BittrexExchange.append_attribute("publicKey") = "";
		BittrexExchange.append_attribute("privateKey") = "";
		BittrexExchange.append_attribute("passphrase") = "";

		pugi::xml_node ArbitrageConfig = Client.append_child("ArbitrageConfig");
		ArbitrageConfig.append_attribute("openTrigger") = "";
		ArbitrageConfig.append_attribute("closeTrigger") = "";
		ArbitrageConfig.append_attribute("availablePercentage") = "";
		
		pugi::xml_node Configuration = Client.append_child("Configuration");
		Configuration.append_attribute("enableArbitrage") = "false";

		p_doc->save_file(fileName.c_str());
	}

	if (bCreateFileNeeded)
	{
		p_doc->reset();
		pugi::xml_parse_result result = p_doc->load_file(fileName.c_str());
		if (result.status != pugi::xml_parse_status::status_ok)
		{
			Output::Error("Unable to create file " + fileName);
			return false;
		}
	}

	return true;
}

bool FileManager::_GetClientTransactionDoc(std::string p_fullFilePath, std::shared_ptr<pugi::xml_document> p_doc)
{

	pugi::xml_parse_result result = p_doc->load_file(p_fullFilePath.c_str());

	bool bCreateFileNeeded = (result.status == pugi::xml_parse_status::status_file_not_found);
	if (bCreateFileNeeded)
	{
		if (p_doc == nullptr)
		{
			p_doc = std::make_shared<pugi::xml_document>();
		}

		// add node with some name
		pugi::xml_node Client = p_doc->append_child("Client");

		// add description node with text child
		pugi::xml_node OpenOrders = Client.append_child("OpenOrders");
		pugi::xml_node TransactionHistory = Client.append_child("TransactionHistory");

		p_doc->save_file(p_fullFilePath.c_str());
	}

	if (bCreateFileNeeded)
	{
		p_doc->reset();
		pugi::xml_parse_result result = p_doc->load_file(p_fullFilePath.c_str());
		if (result.status != pugi::xml_parse_status::status_ok)
		{
			Output::Error("Unable to create file " + p_fullFilePath);
			return false;
		}
	}

	return true;
}

Transaction FileManager::_CreateTransaction(const pugi::xml_node & p_transactionNode)
{
	/*
	<Transaction 
	transId="a3f503e7-5172-4d7c-bbc2-16da37b79358" 
	exchange="Bittrex" 
	type="Sell" 
	fromasset="SYS" 
	toasset="BTC" 
	rate="4.6919999999999998e-05" 
	rateUSD="0" 
	amount="13" 
	opentime="2018-03-21T17:43:10.83" 
	closetime="2018-03-21T17:43:10.97" />
	*/
	std::string transId = p_transactionNode.attribute("transId").as_string();
	std::string exchange = p_transactionNode.attribute("exchange").as_string();
	std::string type = p_transactionNode.attribute("type").as_string();
	std::string fromasset = p_transactionNode.attribute("fromasset").as_string();
	std::string toasset = p_transactionNode.attribute("toasset").as_string();
	double rate = p_transactionNode.attribute("rate").as_double();
	double rateUSD = p_transactionNode.attribute("rateUSD").as_double();
	double amount = p_transactionNode.attribute("amount").as_double();
	std::string openTime = p_transactionNode.attribute("opentime").as_string();
	std::string closeTime = p_transactionNode.attribute("closetime").as_string();

	Transaction t(transId, exchange, (type == "Buy") ? ELimitBuy : ELimitSell, fromasset, toasset, rate, rateUSD, amount, openTime);
	if (closeTime != "")
		t.Close(closeTime);

	return t;
}
