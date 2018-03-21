#include "FileManager.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include<memory>
#include<boost/lexical_cast.hpp>

#include "Client.h"
#include "Transaction.h"
#include "Output.h"


using std::cout;
using std::endl;
using std::vector;

FileManager::FileManager() : m_clientDataFilePath("")
{
	m_workingFilePath = FileManager::_GetCurrentWorkingDir();
}

bool FileManager::RecordTransactionsInFile(std::string p_fileName, const std::vector<std::shared_ptr<Transaction>>& p_transactions)
{	
	pugi::xml_document doc;
	bool bFileLoaded = _GetDocument(p_fileName, doc);

	/*pugi::xml_node TransactionHistory = doc.child("TransactionHistory");

	for (pugi::xml_node xmlTransactionNode : TransactionHistory.children("Transaction"))
	{
		Transaction xmlTransaction = _CreateTransaction(xmlTransactionNode);
		for (auto iter = p_transactions.begin(); iter != p_transactions.end(); ++iter)
		{
			if (xmlTransaction == *iter)
			{
				//Transaction already recorded
			}
		}
		//Transaction not yet recorded

			
		for (pugi::xml_attribute attr : tool.attributes())
		{
			std::cout << " " << attr.name() << "=" << attr.value();
		}

		for (pugi::xml_node child : tool.children())
		{
			std::cout << ", child " << child.name();
		}

		std::cout << std::endl;
	}
	
	*/

	return true;
}

bool FileManager::LoadClientData(std::shared_ptr<Client> p_client, std::string p_clientName)
{	
	if (m_clientDataFilePath == "")
	{
		Output::PrintLn("No client data file specified. Cannot load client.");
		return false;
	}

	pugi::xml_document doc;
	bool bDocumentLoaded = _GetDocument(m_clientDataFilePath, doc);
	if (!bDocumentLoaded)
	{
		Output::PrintLnErr("Problem loading file " + m_clientDataFilePath + ". Are you sure it exists?");
		return false;
	}

	
	//Find the client in the file
	pugi::xml_node ClientNode = doc.child("Client");
	bool bFound = false;
	while(ClientNode)
	{
		std::string sClientName = ClientNode.attribute("name").value();
		if (sClientName == p_clientName)
		{
			bFound = true;
			p_client->SetName(sClientName);
			break; //Client has been found;
		}
		
		ClientNode = ClientNode.next_sibling();
	}
	if (!bFound)
	{
		Output::PrintLnErr("No Client by the name of " + p_clientName + " can be found!");
		return false;
	}

	//Get phone number
	std::string phoneNumber = ClientNode.attribute("phoneNumber").value();
	if (phoneNumber != "") 
	{
		p_client->RegisterAlertPhone(phoneNumber);
		p_client->m_ManagementStrategy.NotifyOnTradeCompletion = true;
	}

	//Get exchanges
	pugi::xml_node ExchangesNode = ClientNode.child("Exchanges");
	for (pugi::xml_node xmlExchangeNode : ExchangesNode.children("Exchange"))
	{
		std::string exchangeName = xmlExchangeNode.attribute("name").value();
		std::string exchangePublicKey = xmlExchangeNode.attribute("publicKey").value();
		std::string exchangePrivateKey = xmlExchangeNode.attribute("privateKey").value();
		std::string exchangePassphrase = "";
		p_client->RegisterExchangeKeys(exchangeName, exchangePublicKey, exchangePrivateKey);
		if (exchangeName == "GDAX")
		{
			exchangePassphrase = xmlExchangeNode.attribute("passphrase").value();
			p_client->RegisterExchangePassphrase(exchangeName, exchangePassphrase);
		}
	}

	//Get Arbitrage Configuration
	if (p_client->HasExchange("Bittrex") && p_client->HasExchange("GDAX"))
	{	//Arbitrage is currently only supported between these two exchanges
		pugi::xml_node ArbitrageConfigNode = ClientNode.child("ArbitrageConfig");
		std::string enabled = ArbitrageConfigNode.attribute("enabled").value();
		if (enabled == "true")
		{
			p_client->m_ManagementStrategy.EnableArbitrage = true;
			double openTrigger			= boost::lexical_cast<double>(ArbitrageConfigNode.attribute("openTrigger").value()) / 100.0;
			double closeTrigger			= boost::lexical_cast<double>(ArbitrageConfigNode.attribute("closeTrigger").value()) / 100.0;
			double availablePercentage	= boost::lexical_cast<double>(ArbitrageConfigNode.attribute("availablePercentage").value()) / 100.0;
			p_client->m_ArbitrageConfiguration.OpenTrigger = openTrigger;
			p_client->m_ArbitrageConfiguration.CloseTrigger = closeTrigger;
			p_client->m_ArbitrageConfiguration.AvailablePercentage = availablePercentage;
		}
	}

	return true;
}

void FileManager::ConfigureClientDataFile(std::string p_clientDataFilePath)
{
	m_clientDataFilePath = p_clientDataFilePath;
}

std::string FileManager::_GetCurrentWorkingDir()
{
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	current_working_dir.append("\\");
	return current_working_dir;
}

bool FileManager::_GetDocument(std::string p_fileName, pugi::xml_document& p_doc)
{
	pugi::xml_parse_result result = p_doc.load_file(p_fileName.c_str());

	if (result.status == pugi::xml_parse_status::status_file_not_found)
	{
		_CreateFile(p_doc, p_fileName);
		result = p_doc.load_file(p_fileName.c_str());

		if (result.status != pugi::xml_parse_status::status_ok)
		{
			Output::PrintLnErr("Unable to create file" + p_fileName);
			return false;
		}
	}

	return true;
}

void FileManager::_CreateFile(pugi::xml_document& p_document, std::string p_fileName)
{
	//Inspired by the example on github:
	//	https://github.com/zeux/pugixml/blob/master/docs/samples/modify_add.cpp

	// add node with some name
	pugi::xml_node Client = p_document.append_child("Client");

	// add description node with text child
	pugi::xml_node Exchanges			= Client.append_child("Exchanges");
	pugi::xml_node OpenOrders			= Client.append_child("OpenOrders");
	pugi::xml_node TransactionHistory	= Client.append_child("TransactionHistory");

	
	// add attributes to param node
	//param.append_attribute("name") = "version";
	//param.append_attribute("value") = 1.1;
	//param.insert_attribute_after("type", param.attribute("name")) = "float";

	p_fileName.insert(0, m_workingFilePath);
	p_document.save_file(p_fileName.c_str());


	/*
	<node>
		<param name="version" type="float" value="1.1000000000000001" />
		<description>Simple node</description>
	</node>

	<Client name="James">
		<Exchanges>
			<Bittrex>
				<PublicKey></PublicKey>
				<PrivateKey></PrivateKey>
			</Bittrex>
		</Exchanges>
		<OpenOrders>
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" ... />
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" ... />
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" ... />
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" ... />
		</OpenOrders>
		<TransactionHistory>
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" CloseDateTime="" .../>
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" CloseDateTime="" .../>
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" CloseDateTime="" .../>
			<Transaction exchange="Bittrex" market="BTC-SIA" OpenDateTime="" CloseDateTime="" .../>
		</TransactionHistory>
	</Client>
	
	
	*/



}
