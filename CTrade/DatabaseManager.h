#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <map>

#include "Transaction.h"

#pragma warning( push )
#pragma warning( disable : 4275) //Disable warning 4275 from pugi
#include "pugixml.hpp"
#pragma warning( pop ) 

class Client;

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#elif __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

class DatabaseManager {
public:
	//Singleton
	static DatabaseManager& Get()
	{
		static DatabaseManager f;
		return f;
	}

	//Mutate information
	bool RecordTransactions(std::string p_clientName, const std::vector<std::shared_ptr<Transaction>>& p_transactions);
	
	//Access information
	bool LoadClientData(std::vector<std::shared_ptr<Client>>& p_clients); //Returns true if the clients were successfully loaded
	
	//Configure File Manager
	void ConfigureClientDataFile(std::string p_clientDataFileName) { m_dataFileName = p_clientDataFileName; }
	void ConfigureDataDirectoryPath(std::string p_dataDirectoryPath) { m_dataDirectoryPath = p_dataDirectoryPath; }
	bool GetRecordedTransactions(std::string p_client, std::vector<Transaction>& p_Transactions);

private:
	DatabaseManager();
	static std::string _GetCurrentWorkingDir();
	static std::string _ClientFileName(std::string p_clientName) { return "trans_" + p_clientName + ".xml"; }
	
	bool _GetClientInfoDoc(std::shared_ptr<pugi::xml_document>& p_doc);
	bool _GetClientTransactionDoc(std::string p_fullFilePath, std::shared_ptr<pugi::xml_document> p_doc);
	
	Transaction _CreateTransaction(const pugi::xml_node& p_transactionNode);

	std::string m_workingFilePath;	// Path the program is running out of
	std::string m_dataFileName;		// file which holds basic information on all clients
	std::string m_dataDirectoryPath;// Path the data will be stored at

	std::map<std::string, std::shared_ptr<pugi::xml_document>> m_clientFiles;
};