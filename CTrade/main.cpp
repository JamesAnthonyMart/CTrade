/******************************************************************************************
Had an idea - buy batmanheads.com, allow people to download for free over a web interface 
so they can use dan's techniques to trade the crypto market.

CoinMarketCap integration if needed
string url = "http://api.coinmarketcap.com/v1/ticker/?limit=10";


TODO:
* Integrate Qt to give an actual UI to this program.
* Finish updating the client's file with transactions from their transaction history
* Alert the client by their preferred alert method whenever a new transaction is logged 
    in the transaction history.
* Finish the ability to reconfigure floating lures in Bittrex for the client
* Finish the ability to do ETH and LTC arbitrage using Bittrex & GDAX
******************************************************************************************/

#include <iostream>
#include <map>		
#include <functional>
#include <boost/algorithm/string.hpp>

#include "Portfolio.h"
#include "ClientManager.h"
#include "Output.h"

/*Temporary includes for file read*/
#include <sstream>
#include <fstream>


using std::string;

void HandleCommands();
void _TemporaryGetClientDataFromFile(std::vector<std::string> &clientdata);

int main()
{	
	FileManager fm;
	fm.ConfigureClientDataFile("C:\\Users\\James\\Google Drive\\Desktop\\clientdata.xml");
	
	std::shared_ptr<Client> c1 = std::make_shared<Client>();
	bool clientExists = fm.LoadClientData(c1, "James");
	if (clientExists)
	{
		//Give the manager this client
		ClientManager::Get().AddClient(c1);
	}

	//Handle console commands for client input
	HandleCommands();
}

void HandleCommands() 
{
	std::map<std::string, std::function<void()>> commands;
	bool bContinuePrompting = true;

	std::function<void()> fcPause = std::bind([]() {Output::PrintLn("[PAUSED]"); ClientManager::Get().userPause = true; });
	std::function<void()> fcResume = std::bind([]() {Output::PrintLn("[RESUMING...]\n"); ClientManager::Get().userPause = false; });
	std::function<void()> fcExit = std::bind([&bContinuePrompting, fcResume]() {if (ClientManager::Get().userPause == true) fcResume(); Output::PrintLn("[STOPPING...]"); bContinuePrompting = false; });
	
	commands["STOP"] = fcExit;
	commands["EXIT"] = fcExit;
	commands["PAUSE"] = fcPause;
	commands["RESUME"] = fcResume;
	commands["CONTINUE"] = fcResume;

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