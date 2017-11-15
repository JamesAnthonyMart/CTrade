#include "Exchange.h"

#include <cassert>

#include "StringUtil.h"
#include "HashUtil.h"
#include "Exceptions.h"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web::http;
using namespace web::http::client;
using namespace web::json;

using std::map;
using std::string;
using std::make_unique;

Exchange::~Exchange()
{
	//NOP Virtual Destructor
}


pplx::task<void> Exchange::GetOpenOrders(std::string p_publicKey, std::string p_privateKey)
{
	//  Todo - assert that m_uriBase isn't an empty string. 
	//  If it is, I forgot to finish implementing a new Exchange.

	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetRequestWithParameters_OpenOrders(p_publicKey));

	//Add necessary headers
	map<string, string> requestHeaders;
	_GetAdditionalHeaders_OpenOrders(p_publicKey, p_privateKey, requestHeaders);
	for (auto header : requestHeaders)
	{
		request.headers().add(StrUtil::s2ws(header.first), StrUtil::s2ws(header.second));
	}

	web::http::client::http_client client(baseUri);
	try {
		return client.request(request)
			.then(std::bind(&Exchange::ExtractJSON,					this, std::placeholders::_1))
			.then(std::bind(&Exchange::_CreateTransactionFromJSON,	this, std::placeholders::_1));
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

	return pplx::task<void>();
}

pplx::task<web::json::value> Exchange::ExtractJSON(web::http::http_response response)
{
	if (response.status_code() != status_codes::OK)
	{
		throw EBadResponse;
	}

	return response.extract_json();
}

void Exchange::_InitURIs()
{
	assert(false); // Child class must override this function
}

void Exchange::_CreateTransactionFromJSON(pplx::task<web::json::value> p_jsonValue)
{
	assert(false); // Child class must override this function
}

utility::string_t Exchange::_GetRequestWithParameters_OpenOrders(std::string p_publicKey)
{
	assert(false); // Child class must override this function
	return utility::string_t(); // Silence the compiler
}

void Exchange::_GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	assert(false); // Child class must override this function
}




/*****************           BITTREX            *************************/

Bittrex::Bittrex() : Exchange("Bittrex") {}
Bittrex::~Bittrex() {} //NOP Virtual Destructor

void Bittrex::_InitURIs()
{
	m_uriBase = "https://bittrex.com/api/v1.1/";
	m_uriOpenTransactions = "market/getopenorders";
	m_uriTransactionHistory = "account/getorderhistory";
}

void Bittrex::_CreateTransactionFromJSON(pplx::task<web::json::value> p_jsonValue)
{
	const web::json::value& jsonResponse = p_jsonValue.get();
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

			std::wcout << "Market found: " << exchange << std::endl;

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