/******************************************************************************************
Had an idea - buy batmanheads.com, allow people to download for free over a web interface 
so they can use dan's techniques to trade the crypto market.

CoinMarketCap integration if needed
string url = "http://api.coinmarketcap.com/v1/ticker/?limit=10";

UPDATE NOTES:
* I think I finished updating the client's file with transactions from their transaction history
* I think I can now gather the transaction history also from the file, so all the transactions should persist now.
* Need to test the above ^ Maybe write a unit test?

TODO:
* Integrate Qt to give an actual UI to this program.
* Alert the client by their preferred alert method whenever a new transaction is logged 
    in the transaction history.
* Finish the ability to reconfigure floating lures in Bittrex for the client
* Finish the ability to do ETH and LTC arbitrage using Bittrex & GDAX
******************************************************************************************/

#include <iostream>
#include <map>		
#include <functional>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "Portfolio.h"
#include "ClientManager.h"
#include "Output.h"



using std::string;

void HandleCommands();

int main()
{	
	DatabaseManager::Get().ConfigureDataDirectoryPath("C:\\Users\\James\\Google Drive\\Desktop\\CTrade\\");
	DatabaseManager::Get().ConfigureClientDataFile("clientdata.xml");
	
	std::vector<std::shared_ptr<Client>> clients;
	DatabaseManager::Get().LoadClientData(clients);
	
	std::for_each(clients.begin(), clients.end(), [=](std::shared_ptr<Client> p_client) 
	{
		ClientManager::Get().RegisterClient(p_client);
	});

	//Handle console commands for client input
	HandleCommands();
}

void HandleCommands() 
{
	std::map<std::string, std::function<void()>> commands;
	bool bContinuePrompting = true;

	std::function<void()> fcPause = std::bind([]() {
		Output::Info("[PAUSED]"); 
		ClientManager::Get().userPause = true; 
	});

	std::function<void()> fcResume = std::bind([]() {
		Output::Info("[RESUMING...]\n"); 
		ClientManager::Get().userPause = false; 
	});

	std::function<void()> fcExit = std::bind([&bContinuePrompting, fcResume]() {
		if (ClientManager::Get().userPause == true) fcResume(); 
		Output::Info("[STOPPING...]"); 
		bContinuePrompting = false; 
	});
	
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