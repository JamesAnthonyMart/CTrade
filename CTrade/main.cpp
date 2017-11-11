#include <iostream>

#include "Portfolio.h"
#include "ClientManager.h"
#include "StringUtil.h"

/*
	Had an idea - buy batmanheads.com, give this shit a web interface, allow anyone to sign up and create 
		their portfolio and use dan's techniques to trade the crypto market. 
*/

using std::string;

// CoinMarketCap integration if needed
// string url = "http://api.coinmarketcap.com/v1/ticker/?limit=10";

/****************     Todo     ******************

* Read in configuration file instead of just harcoding shit here.
* Integrate Qt to give an actual UI to this program.

*************************************************/




#include "HashUtil.h"
#include <cpprest/http_client.h>
//#include <cpprest/json.h>
//#include <ctime>
//#include <sstream>

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;


pplx::task<void> HTTPGetAsync()
{
	std::string apiKey = "3a24d67d68db419eb29d6ba2d50f956b";
	std::string secKey = "0c04ad606e4f4ce98d7f687ac82fdec6";

	//Build client with URI
	web::uri base_uri(U("https://bittrex.com/api/v1.1/"));
	web::uri request_uri_base(U("market/getopenorders"));
	web::uri_builder request_uri_builder(request_uri_base);
	request_uri_builder.append_query(U("apikey"), StrUtil::s2ws(apiKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	request_uri_builder.append_query(U("nonce"), ssNonce.str().c_str());

	std::string completeUri = StrUtil::ws2s(base_uri.to_string().append(request_uri_builder.to_string()));
	std::string onlineHashResult = HashUtil::sha512(secKey, completeUri);

	std::cout << std::endl;
	std::cout << "Request: " << completeUri << "\n";
	std::cout << "Hash:    " << onlineHashResult << "\n";

	web::http::http_request request(web::http::methods::GET);
	request.headers().add(U("apisign"), StrUtil::s2ws(onlineHashResult));
	request.headers().add(U("cache-control"), U("no-cache"));
	request.headers().add(U("Host"), U("bittrex.com"));
	request.set_request_uri(request_uri_builder.to_string());
	
	// Make an HTTP GET request and asynchronously process the response

	web::http::client::http_client client(base_uri);
	return client.request(request).then([](http_response response) -> pplx::task<web::json::value>
	{
		if (response.status_code() == status_codes::OK)
		{
			return response.extract_json();
		}

		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(web::json::value());
	})
		.then([](pplx::task<web::json::value> previousTask)
	{
		try
		{
			const web::json::value& jsonResponse = previousTask.get();


			auto jarray = jsonResponse.at(U("result")).as_array();
			for (int i = 0; i < jarray.size(); ++i)
			{
				try {
					auto orderDetails = jarray[i];
					auto orderUuid = orderDetails.at(U("OrderUuid")).as_string();
					auto exchange = orderDetails.at(U("Exchange")).as_string();
					
					std::wcout << "Echange found: " << exchange << std::endl;
					
					auto orderType = orderDetails.at(U("OrderType")).as_string();
					auto quantity = orderDetails.at(U("Quantity")).as_integer();
					auto quantityRemaining = orderDetails.at(U("QuantityRemaining")).as_integer();
					auto limit = orderDetails.at(U("Limit")).as_double();
					auto opened = orderDetails.at(U("Opened")).as_string();
					auto isConditional = orderDetails.at(U("IsConditional")).as_bool();
					std::wstring condition = U("NONE");
					double conditionTarget = -1.0;
					if (isConditional)
					{
						condition = orderDetails.at(U("Condition")).as_string();
						conditionTarget = orderDetails.at(U("ConditionTarget")).as_double();
					}

					//Save this transaction somewhere?
					
				}
				catch (const web::json::json_exception e) 
				{
					//Alert the user of a JSON failure.
					jarray[0];
				}
				
			}
		}
		catch (const http_exception& e)
		{
			// Print error.
			std::wostringstream ss;
			ss << e.what() << std::endl;
			std::wcout << ss.str();
		}
	});
}


void HandleCommands();

int main()
{	
	/*    Create Portfolios    */
	std::shared_ptr<Client> c1 = std::make_shared<Client>("James");
	c1->RegisterExchangeKeys("Bittrex", "3a24d67d68db419eb29d6ba2d50f956b", "c04ad606e4f4ce98d7f687ac82fdec6");
	c1->RegisterAlertPhone("2676143317");
	c1->m_ManagementStrategy.NotifyOnTradeCompletion = true;

	//Portfolio portfolio("LowRiskLowReward");
	//Portfolio portfolio("HighRiskHighReward");	

	/*    Give portfolios to manager    */
	ClientManager::Get().AddClient(c1);

	HandleCommands();
}

#include <map>
#include <functional>
#include <boost/algorithm/string.hpp>

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