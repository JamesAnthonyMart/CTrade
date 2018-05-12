#include "DatabaseManager.h"

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

DatabaseManager::DatabaseManager() :   m_dataDirectoryPath("")
							 , m_dataFileName("")
{
	m_workingFilePath = DatabaseManager::_GetCurrentWorkingDir();
}

bool DatabaseManager::RecordCompleteTransactions(std::string p_clientName, const std::vector<std::shared_ptr<Transaction>>& p_transactions)
{	
	std::shared_ptr<pugi::xml_document> doc = m_clientFiles[p_clientName];
	
	//breakpoint because I need to see if doc is nullptr if the clientname isn't in the clientfiles map
	std::string fullFileName = m_dataDirectoryPath + _ClientFileName(p_clientName);

	if (doc == nullptr)
	{
		_GetClientTransactionDoc(fullFileName, doc); //Creates the document if not found
		m_clientFiles[p_clientName] = doc;
	}

	pugi::xml_node TransactionHistory = doc->child("Client").child("TransactionHistory");

	for (size_t i = 0; i < p_transactions.size(); ++i)
	{
		auto t = p_transactions[i];

		//  Todo: I think I should ask the Transaction object how it wants to be serialized, and then just follow its directions.
		//        Doesn't make sense the database manager itself knows how to serialize a transaction.
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

bool DatabaseManager::LoadClientData(std::vector<std::shared_ptr<Client>>& p_clients)
{	

	////////////////////////////////////////////////////////////////////////////////
	////						LOAD CLIENT FILE								////
	////////////////////////////////////////////////////////////////////////////////

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
	std::shared_ptr<pugi::xml_document> doc = nullptr;
	bool bNewDocumentCreated = _GetClientInfoDoc(doc);
	if (bNewDocumentCreated)
	{
		Output::Info("New client info document created.");
	}
	if (doc == nullptr)
	{
		Output::Error("Error retrieving client document.");
		return false;
	}

	p_clients.clear();
	// Get client data from file
	for (pugi::xml_node ClientNode = doc->child("Client"); ClientNode; ClientNode = ClientNode.next_sibling())
	{
		auto Configuration = ClientNode.child("Configuration");
		std::string currentClientID = ClientNode.attribute("id").value();
		std::shared_ptr<Client> client = std::make_shared<Client>(currentClientID);

		
		////////////////////////////////////////////////////////////////////////////////
		////						COPY CLIENT DATA								////
		////////////////////////////////////////////////////////////////////////////////
		//		Phone number
		std::string phoneNumber = ClientNode.attribute("phoneNumber").value();
		if (phoneNumber != "")
		{
			client->RegisterAlertPhone(phoneNumber);
		}
		//		Exchanges
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

		////////////////////////////////////////////////////////////////////////////////
		////						ENABLE CONFIGURATIONS							////
		////////////////////////////////////////////////////////////////////////////////
		bool bValidClient = true;
		
		std::function<bool(const pugi::char_t*)> IsAttributeTrue = [&](const pugi::char_t* p_attributeName) -> bool
		{
			return (static_cast<std::string>(Configuration.attribute(p_attributeName).as_string()) == "true");
		};

		if (IsAttributeTrue("enableOrderTracking"))
		{
			//  Requirements: Must have at least one exchange configured
			//    At the moment, the one exchange must be Bittrex

			if (client->HasExchange("Bittrex"))
			{
				client->m_ManagementStrategy.EnableOrderTracking = true;
			}
			else
			{
				Output::Error("Order tracking is enabled, but requirements are not met.");
			}
		}

		if (IsAttributeTrue("enableAlertsOnTradeCompletion"))
		{
			//Todo - Add requirements: Must have at least one exchange configured
			Output::Warning("Alerts on trade completion not yet supported... try again later.");
			client->m_ManagementStrategy.NotifyOnTradeCompletion = /*true;*/ false;
		}

		if (IsAttributeTrue("enableFloatingLures"))
		{
			//Todo - Add requirements: Must have at least one exchange configured
			Output::Warning("Floating lure management not yet supported... try again later.");
			client->m_ManagementStrategy.EnableFloatingLures = /*true;*/ false;
		}

		if (IsAttributeTrue("enableArbitrage"))
		{
			//  Requirements: Must have two exchanges configured.
			//    At the moment, the two exchanges MUST be Bittrex and GDAX
			
			if (client->HasExchange("Bittrex") && client->HasExchange("GDAX"))
			{	// Arbitrage is currently only supported between these two exchanges
				auto ArbitrageConfigNode = ClientNode.child("ArbitrageConfig");

				double openTrigger = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("openTrigger").value()) / 100.0;
				double closeTrigger = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("closeTrigger").value()) / 100.0;
				double availablePercentage = boost::lexical_cast<double>(ArbitrageConfigNode.attribute("availablePercentage").value()) / 100.0;
				client->m_ArbitrageConfiguration.OpenTrigger = openTrigger;
				client->m_ArbitrageConfiguration.CloseTrigger = closeTrigger;
				client->m_ArbitrageConfiguration.AvailablePercentage = availablePercentage;
				
				Output::Warning("Arbitrage not yet supported... try again later.");
				client->m_ManagementStrategy.EnableArbitrage = /*true;*/ false;
			}
			else
			{
				Output::Error("Arbitrage is enabled, but requirements are not met.");
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

bool DatabaseManager::GetCompletedTransactions(std::string p_client, std::vector<Transaction>& p_Transactions)
{

	std::shared_ptr<pugi::xml_document> doc = m_clientFiles[p_client];

	std::string fullFileName = m_dataDirectoryPath + _ClientFileName(p_client);

	if (doc == nullptr)
	{
		_GetClientTransactionDoc(fullFileName, doc); //Creates the document if not found
		m_clientFiles[p_client] = doc;
	}
	
	pugi::xml_node TransactionHistory = doc->child("Client").child("TransactionHistory");

	
	for (pugi::xml_node transaction : TransactionHistory.children("Transaction"))
	{
		//Todo - I might want to move create-transaction to a static function in the Transaction class, since a filemanager
		//  shouldn't really know how to create a transaction.
		p_Transactions.push_back(_CreateTransaction(transaction));
	}
	
	return true;
}

std::string DatabaseManager::_GetCurrentWorkingDir()
{
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	current_working_dir.append("\\");
	return current_working_dir;
}

bool DatabaseManager::_GetClientInfoDoc(std::shared_ptr<pugi::xml_document>& p_doc)
{
	std::string fileName = m_dataDirectoryPath + m_dataFileName;
	if (p_doc == nullptr)
	{
		p_doc = std::make_shared<pugi::xml_document>();
	}
	pugi::xml_parse_result result = p_doc->load_file(fileName.c_str());
	bool bNewFileNeeded = (result.status == pugi::xml_parse_status::status_file_not_found);
	bool bNewFileCreated = false;
	if (bNewFileNeeded)
	{
		Output::Info("File " + fileName + " not found, creating file...");

		p_doc = std::make_shared<pugi::xml_document>();
		
		pugi::xml_node Client = p_doc->append_child("Client");
		Client.append_attribute("id") = "001";
		Client.append_attribute("name") = "";
		Client.append_attribute("phoneNumber") = "";
		pugi::xml_node Exchanges = Client.append_child("Exchanges");
		
		//Todo - get needed attributes from ExchangeManager
		pugi::xml_node BittrexExchange = Exchanges.append_child("Exchange");
		BittrexExchange.append_attribute("name") = "Bittrex";
		BittrexExchange.append_attribute("publicKey") = "";
		BittrexExchange.append_attribute("privateKey") = "";
		pugi::xml_node GDAXExchange = Exchanges.append_child("Exchange");
		GDAXExchange.append_attribute("name") = "GDAX";
		GDAXExchange.append_attribute("publicKey") = "";
		GDAXExchange.append_attribute("privateKey") = "";
		GDAXExchange.append_attribute("passphrase") = "";

		pugi::xml_node ArbitrageConfig = Client.append_child("ArbitrageConfig");
		ArbitrageConfig.append_attribute("openTrigger") = "";
		ArbitrageConfig.append_attribute("closeTrigger") = "";
		ArbitrageConfig.append_attribute("availablePercentage") = "";
		
		pugi::xml_node Configuration = Client.append_child("Configuration");
		Configuration.append_attribute("enableOrderTracking") = "false";
		Configuration.append_attribute("enableAlertsOnTradeCompletion") = "false";
		Configuration.append_attribute("enableFloatingLures") = "false";
		Configuration.append_attribute("enableArbitrage") = "false";

		p_doc->save_file(fileName.c_str());
		bNewFileCreated = true;
	}

	if (bNewFileNeeded)
	{
		p_doc->reset();
		pugi::xml_parse_result result = p_doc->load_file(fileName.c_str());
		if (result.status != pugi::xml_parse_status::status_ok)
		{
			Output::Error("Unable to create file " + fileName);
			
			// Todo - Should probably throw an exception here
			
			return false; // No new file created
		}
	}

	return bNewFileCreated;
}

bool DatabaseManager::_GetClientTransactionDoc(std::string p_fullFilePath, std::shared_ptr<pugi::xml_document>& p_doc)
{
	if (p_doc == nullptr)
	{
		p_doc = std::make_shared<pugi::xml_document>();
	}

	pugi::xml_parse_result result = p_doc->load_file(p_fullFilePath.c_str());
	bool bCreateFileNeeded = (result.status == pugi::xml_parse_status::status_file_not_found);
	bool bNewFileCreated = false;
	if (bCreateFileNeeded)
	{
		// add node with some name
		pugi::xml_node Client = p_doc->append_child("Client");

		// add description node with text child
		pugi::xml_node OpenOrders = Client.append_child("OpenOrders");
		pugi::xml_node TransactionHistory = Client.append_child("TransactionHistory");

		p_doc->save_file(p_fullFilePath.c_str());
		bNewFileCreated = true;
	}

	if (bCreateFileNeeded)
	{
		p_doc->reset();
		pugi::xml_parse_result result = p_doc->load_file(p_fullFilePath.c_str());
		if (result.status != pugi::xml_parse_status::status_ok)
		{
			Output::Error("Unable to create file " + p_fullFilePath);
			
			// Todo - Should throw an exception here

			return false; //No new file created...
		}
	}

	return bNewFileCreated;
}

Transaction DatabaseManager::_CreateTransaction(const pugi::xml_node & p_transactionNode)
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
