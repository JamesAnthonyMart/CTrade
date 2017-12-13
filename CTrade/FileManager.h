#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <memory>

#include "pugixml.hpp"

class Transaction;
class Client;

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#elif __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

class FileManager {
public:
	FileManager();
	bool RecordTransactionsInFile(std::string p_fileName, const std::vector<std::shared_ptr<Transaction>>& p_transactions);
	void CreateClient(std::shared_ptr<Client> p_client, std::string p_fileName);

private:
	static std::string _GetCurrentWorkingDir();

	void _GetDocument(std::string p_fileName, pugi::xml_document& doc);

	void _CreateFile(pugi::xml_document& p_document, std::string p_fileName);

	std::string m_workingFilePath;
};