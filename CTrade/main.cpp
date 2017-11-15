/******************************************************************************************
Had an idea - buy batmanheads.com, give this shit a web interface, allow anyone to sign up and create
their portfolio and use dan's techniques to trade the crypto market.

CoinMarketCap integration if needed
string url = "http://api.coinmarketcap.com/v1/ticker/?limit=10";


TODO:
* Read in configuration file instead of just harcoding shit here.
* Integrate Qt to give an actual UI to this program.
******************************************************************************************/

#include <iostream>
#include <map>		
#include <functional>
#include <boost/algorithm/string.hpp>

#include "Portfolio.h"
#include "ClientManager.h"

/*Temporary includes for file read*/
#include <sstream>
#include <fstream>


using std::string;

void HandleCommands();
void _TemporaryGetClientDataFromFile(std::vector<std::string> &clientdata);

int main()
{	
	//Temporary: Read client data from file
	std::vector<std::string> clientdata;
	_TemporaryGetClientDataFromFile(clientdata);

	//Create/configure client
	std::shared_ptr<Client> c1 = std::make_shared<Client>(clientdata[0]);
	c1->RegisterExchangeKeys("Bittrex", clientdata[1], clientdata[2]);
	c1->RegisterAlertPhone(clientdata[3]);
	c1->m_ManagementStrategy.NotifyOnTradeCompletion = true;

	//Give the manager this client
	ClientManager::Get().AddClient(c1);

	//Handle console commands for client input
	HandleCommands();
}

void _TemporaryGetClientDataFromFile(std::vector<std::string> &clientdata)
{
	std::string line;
	std::ifstream infile("C:\\Users\\James\\Google Drive\\Desktop\\clientdata.txt");
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string name, pubstr, privstr, phonenumber;
		iss >> name >> pubstr >> privstr >> phonenumber;
		clientdata.push_back(name);
		clientdata.push_back(pubstr);
		clientdata.push_back(privstr);
		clientdata.push_back(phonenumber);
	}
}

void HandleCommands() 
{
	std::map<std::string, std::function<void()>> commands;
	bool bContinuePrompting = true;

	std::function<void()> fcExit = std::bind([&bContinuePrompting]() {std::cout << "Stopping..." << std::endl; bContinuePrompting = false; });

	commands["STOP"] = fcExit;
	commands["EXIT"] = fcExit;

	while (bContinuePrompting)
	{
		std::string inputStr;
		std::cin >> inputStr;

		boost::to_upper(inputStr);

		if (commands.find(inputStr) != commands.end())
		{
			commands[inputStr]();
		}
	}
}