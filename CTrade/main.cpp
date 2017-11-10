#include <iostream>
#include <string>

#include "Portfolio.h"
#include "ClientManager.h"
/*
	Had an idea - buy batmanheads.com, give this shit a web interface, allow anyone to sign up and create 
		their portfolio and use dan's techniques to trade the crypto market. 
*/



#include <cpprest/containerstream.h>
#include <cpprest/filestream.h>
#include <cpprest/producerconsumerstream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <ctime>
#include <sstream>
#include <windows.h>
//converting from wstring to string
#include <locale>
#include <codecvt>

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;








using std::string;

// CoinMarketCap integration if needed
// string url = "http://api.coinmarketcap.com/v1/ticker/?limit=10";

/****************     Todo     ******************
  
  * Read in configuration file instead of just harcoding shit here.
  * Integrate Qt to give an actual UI to this program.

*************************************************/



string sha512(string p_SecretKey, string p_Message)
{
	//Todo determine way to be more memory efficient
	char key[10000]; 
	char data[1024];
	strcpy(key, p_SecretKey.c_str());
	strncpy_s(data, p_Message.c_str(), sizeof(data));
	data[sizeof(data) - 1] = 0;


	unsigned char* digest;

	//============================================
	//Example extract from http://www.askyb.com/cpp/openssl-hmac-hasing-example-in-cpp/
	//askyb on February, 26th 2013 in C++ OpenSSL
	//=============================================
	// Using sha1 hash engine here.
	// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
	digest = HMAC(EVP_sha512(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);

	//Print digest for debugging purposes
	//printf("HMAC digest: ");
	//for (int i = 0; i != SHA512_DIGEST_LENGTH; i++)
		//printf("%02hhx", digest[i]);
	//printf("\n");

	//SHA512 produces a 64-byte hash value which rendered as 128 characters.
	// Change the length accordingly with your choosen hash engine
	char mdString[SHA512_DIGEST_LENGTH*2+1];
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		sprintf(&mdString[i * 2], "%02hhx", digest[i]);
	mdString[128] = '\0';

	//printf("\nOnline HMAC digest: %s\n", mdString);
	//printf("Size is %u characters long.\n", (unsigned)strlen(mdString));
	//=============================================

	string output = mdString;
	return output;
}


std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}


std::string ws2s(const std::wstring& str) 
{
	
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(str);
}

pplx::task<void> HTTPGetAsync()
{
	std::string apiKey = "3a24d67d68db419eb29d6ba2d50f956b";
	std::string secKey = "0c04ad606e4f4ce98d7f687ac82fdec6";

	//Build client with URI
	web::uri base_uri(U("https://bittrex.com/api/v1.1/"));
	web::uri request_uri_base(U("market/getopenorders"));
	web::uri_builder request_uri_builder(request_uri_base);
	request_uri_builder.append_query(U("apikey"), s2ws(apiKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	request_uri_builder.append_query(U("nonce"), ssNonce.str().c_str());

	std::string completeUri = ws2s(base_uri.to_string().append(request_uri_builder.to_string()));
	std::string onlineHashResult = sha512(secKey, completeUri);

	std::cout << std::endl;
	std::cout << "Request: " << completeUri << "\n";
	std::cout << "Hash:    " << onlineHashResult << "\n";

	web::http::http_request request(web::http::methods::GET);
	request.headers().add(U("apisign"), s2ws(onlineHashResult));
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




int main()
{	
	/*    Create Portfolios    */
	std::shared_ptr<Client> c1("James");
	c1->RegisterExchangeKeys("Bittrex", "3a24d67d68db419eb29d6ba2d50f956b", "c04ad606e4f4ce98d7f687ac82fdec6");
	c1->RegisterAlertPhone("2676143317");
	c1->m_ManagementStrategy.NotifyOnTradeCompletion = true;

	//Portfolio portfolio("LowRiskLowReward");
	//Portfolio portfolio("HighRiskHighReward");	

	/*    Give portfolios to manager    */
	ClientManager::Get().AddClient(c1);
	
	//Quick test - calling wait - usually don't wanna do this
	HTTPGetAsync().wait();

	std::string temp;
	std::cin >> temp;
	std::cout << temp << std::endl;
}
