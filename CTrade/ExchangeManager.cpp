#include "ExchangeManager.h"

#include <cassert>

//Maybe move some of this garbage to an httpUtil or something
#include "HashUtil.h"
#include "StringUtil.h"
#include "Exceptions.h"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

using std::string;
using std::map;
using std::make_unique;

/*
"Uuid": null,
"OrderUuid": "39cd4591-5571-48ee-b0e5-366d9e78cb15",
"Exchange": "BTC-SC",
"OrderType": "LIMIT_SELL",
"Quantity": 11000,
"QuantityRemaining": 11000,
"Limit": 0.00000296,
"CommissionPaid": 0,
"Price": 0,
"PricePerUnit": null,
"Opened": "2017-10-25T18:36:22.82",
"Closed": null,
"CancelInitiated": false,
"ImmediateOrCancel": false,
"IsConditional": false,
"Condition": "NONE",
"ConditionTarget": null*/

ExchangeManager::~ExchangeManager() 
{
}
ExchangeManager::ExchangeManager() 
{
	m_exchanges.push_back(make_unique<Bittrex>());
}

std::vector<Transaction> ExchangeManager::GetOpenTransactions(std::string p_exchangeId, std::string p_publicKey, std::string p_privateKey)
{
	//p_exchangeId currently unused. All go to Bittrex.
	for (size_t i = 0; i < m_exchanges.size(); ++i)
	{
		if (m_exchanges[i]->GetName() == p_exchangeId)
		{
			m_exchanges[i]->GetOpenOrders(p_publicKey, p_privateKey).wait();
		}
	}
	/*for (auto exchange : m_exchanges) 
	{
		if (exchange->GetName() == p_exchangeId)
		{
			exchange->GetOpenOrders(p_publicKey, p_privateKey);
		}
	}*/

	return std::vector<Transaction>();
}




Exchange::~Exchange()
{
	//NOP Virtual Destructor
}

pplx::task<void> Exchange::GetOpenOrders(std::string p_publicKey, std::string p_privateKey)
{
	//  Todo - assert that m_uriBase isn't an empty string. 
	//  If it is, I forgot to finish implementing a new Exchange.

	//Start with base URI
	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));

	//Create web request with specific exchange parameters
	web::http::http_request request(web::http::methods::GET);
	request.set_request_uri(_GetRequestWithParameters_OpenOrders(p_publicKey));
	
	//Add necessary headers
	request.headers().add(U("cache-control"), U("no-cache")); //Todo - try removing headers: cache-control and Host.
	request.headers().add(U("Host"), U("bittrex.com"));	
	map<string, string> additionalHeaders;
	_GetAdditionalHeaders_OpenOrders(p_publicKey, p_privateKey, additionalHeaders);
	for (auto header : additionalHeaders) 
		request.headers().add(StrUtil::s2ws(header.first), StrUtil::s2ws(header.second));
	
	web::http::client::http_client client(baseUri);
	try {
		return client.request(request).then([](http_response response) -> pplx::task<web::json::value>
		{
			if (response.status_code() != status_codes::OK)
			{
				throw EBadResponse;
			}

			return response.extract_json();

		}).then([](pplx::task<web::json::value> previousTask)
		{
			const web::json::value& jsonResponse = previousTask.get();
			if (jsonResponse.at(U("result")).is_null())
			{
				std::cout << "Probably your hash is calculating incorrectly or something." << std::endl;
				return;
			}

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
		});
	}
	catch (const http_exception& e)
	{
		// Print error.
		std::wostringstream ss;
		ss << "HttpException: " << e.what() << std::endl;
		std::wcout << ss.str();
	}
	catch (const InvalidSignatureException& e)
	{
		std::cout << e.what() << '\n';
	}

}

void Exchange::_DefineURIs()
{
	assert(false); // Child class must overwrite this function
}

utility::string_t Exchange::_GetRequestWithParameters_OpenOrders(std::string p_publicKey)
{
	assert(false); // Child class must overwrite this function
	return utility::string_t(); //silence the compiler
}

void Exchange::_GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	assert(false); // Child class must overwrite this function
}

Bittrex::Bittrex() : Exchange("Bittrex") 
{
	_DefineURIs();
}

Bittrex::~Bittrex()
{
	//NOP Virtual Destructor
}

void Bittrex::_DefineURIs()
{
	m_uriBase = "https://bittrex.com/api/v1.1/";
	m_uriOpenTransactions = "market/getopenorders";
	m_uriTransactionHistory = "account/getorderhistory";
}

utility::string_t Bittrex::_GetRequestWithParameters_OpenOrders(std::string p_publicKey)
{
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriOpenTransactions));
	web::uri_builder fullRequestUri(requestUri);
	fullRequestUri.append_query(U("apikey"), StrUtil::s2ws(p_publicKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	fullRequestUri.append_query(U("nonce"), ssNonce.str().c_str());

	return fullRequestUri.to_string();
}

void Bittrex::_GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, map<string, string>& p_additionalHeaders)
{
	p_additionalHeaders.clear();
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriOpenTransactions));
	web::uri_builder fullRequestUri(requestUri);
	std::string completeUri = StrUtil::ws2s(StrUtil::s2ws(m_uriBase).append(_GetRequestWithParameters_OpenOrders(p_publicKey)));

	p_additionalHeaders["apisign"] = HashUtil::sha512(p_privateKey, completeUri);
}

/* Todo - delete this, I already ported it into the exchange base class

pplx::task<void> Bittrex::_GetOpenOrders(std::string p_publicKey, std::string p_privateKey)
{
	//Build client with URI
	web::uri base_uri(StrUtil::s2ws(m_uriBase));
	web::uri request_uri_base(StrUtil::s2ws(m_uriOpenTransactions));

	web::uri_builder request_uri_builder(request_uri_base);
	request_uri_builder.append_query(U("apikey"), StrUtil::s2ws(p_publicKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	request_uri_builder.append_query(U("nonce"), ssNonce.str().c_str());
	
	web::http::http_request request(web::http::methods::GET);
	request.set_request_uri(request_uri_builder.to_string());



	std::string completeUri = StrUtil::ws2s(base_uri.to_string().append(request_uri_builder.to_string()));
	std::string onlineHashResult = HashUtil::sha512(p_privateKey, completeUri);
	request.headers().add(U("apisign"), StrUtil::s2ws(onlineHashResult));

	
	

	// Make an HTTP GET request and asynchronously process the response

	web::http::client::http_client client(base_uri);
	
}
*/