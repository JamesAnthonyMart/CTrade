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

using std::cout;
using std::endl;

Exchange::~Exchange()
{
	//NOP Virtual Destructor
}


pplx::task<void> Exchange::GetOpenOrders(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_openTransactions)
{
	p_openTransactions->clear();
	
	string functionId = "OpenOrders";
	web::http::http_request request = _GetAuthenticatedRequest(p_publicKey, p_privateKey, functionId);

	//Handle web response
	return _GetHttpClient().request(request)
		.then(std::bind(&Exchange::ExtractJSON,		   this, std::placeholders::_1))
		.then(std::bind(&Exchange::_ParseTransactions, this, std::placeholders::_1, functionId, p_openTransactions));
}

pplx::task<void> Exchange::GetTransactionHistory(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_exchangeTransHistory)
{
	p_exchangeTransHistory->clear();

	string functionId = "TransactionHistory";
	web::http::http_request request = _GetAuthenticatedRequest(p_publicKey, p_privateKey, functionId);

	//Handle web response
	return _GetHttpClient().request(request)
		.then(std::bind(&Exchange::ExtractJSON, this, std::placeholders::_1))
		.then(std::bind(&Exchange::_ParseTransactions, this, std::placeholders::_1, functionId, p_exchangeTransHistory));
}

pplx::task<void> Exchange::GetPrice(std::string p_ticker, double* p_price)
{	
	//string functionId = "Price";
	//web::http::http_request request = _GetUnAuthenticatedRequest(functionId, p_ticker);

	//  If you hit this, you probably forgot to finish implementing a new Exchange.
	assert(m_uriBase != "");

	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetRequestWithParameters_GetPrice(p_ticker));

	web::http::client::http_client client(baseUri);
	return client.request(request)
		.then(std::bind(&Exchange::ExtractJSON, this, std::placeholders::_1))
		.then(std::bind(&Exchange::_ParsePrice, this, std::placeholders::_1, p_price));
}

pplx::task<web::json::value> Exchange::ExtractJSON(web::http::http_response response)
{
	if (response.status_code() != status_codes::OK)
	{
		throw EBadResponse;
	}

	return response.extract_json();
}

std::string Exchange::_GetNonce()
{
	std::stringstream ssNonce; 
	ssNonce.str(""); 
	ssNonce << std::time(nullptr);
	return ssNonce.str();
}

std::string Exchange::_GenerateRequestUri(std::string p_uriBase, std::vector<std::pair<std::string, std::string>> p_parameters)
{
	web::uri requestUri = web::uri(StrUtil::s2ws(p_uriBase));
	web::uri_builder fullRequestUri(requestUri);
	for (size_t i = 0; i < p_parameters.size(); ++i)
	{
		std::string key = p_parameters[i].first;
		std::string value = p_parameters[i].second;
		fullRequestUri.append_query(StrUtil::s2ws(key), StrUtil::s2ws(value));
	}

	return StrUtil::ws2s(fullRequestUri.to_string());
}


web::http::client::http_client Exchange::_GetHttpClient()
{
	//  If you hit this, you probably forgot to finish implementing a new Exchange.
	assert(m_uriBase != "");
	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));
	web::http::client::http_client client(baseUri);
	return client;
}

web::http::http_request Exchange::_GetAuthenticatedRequest(std::string p_publicKey, std::string p_privateKey, std::string p_functionId)
{
	//Create base request
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetAuthenticatedRequestWithParameters(p_functionId, p_publicKey));

	//Add necessary headers
	map<string, string> requestHeaders;
	_GetAuthenticatedHeaders(p_functionId, p_publicKey, p_privateKey, requestHeaders);
	for (auto header : requestHeaders)
		request.headers().add(StrUtil::s2ws(header.first), StrUtil::s2ws(header.second));

	return request;
}

web::http::http_request Exchange::_GetUnauthenticatedRequest(std::string p_functionId)
{
	//Create base request
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetUnauthenticatedRequestWithParameters(p_functionId));

	return request;
}



void Exchange::_InitURIs()
{
	assert(false); // Child class must override this function
}

void Exchange::_ParseTransactions(pplx::task<web::json::value> p_previousTask, std::string p_functionId, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	assert(false); // Child class must override this function
}

void Exchange::_ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price)
{
	assert(false); // Child class must override this function
}

utility::string_t Exchange::_GetRequestWithParameters_GetPrice(std::string p_ticker)
{
	assert(false); // Child class must override this function
	return utility::string_t();
}

utility::string_t Exchange::_GetAuthenticatedRequestWithParameters(std::string p_functionId, std::string p_publicKey)
{
	assert(false); // Child class must override this function
	return utility::string_t();
}

void Exchange::_GetAuthenticatedHeaders(std::string p_functionId, std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	assert(false); // Child class must override this function
}

utility::string_t Exchange::_GetUnauthenticatedRequestWithParameters(std::string p_functionId)
{
	assert(false); // Child class must override this function
	return utility::string_t(); // Silence the compiler
}




/*****************           BITTREX            *************************/

Bittrex::Bittrex() : Exchange("Bittrex") {}
Bittrex::~Bittrex() {} //NOP Virtual Destructor

void Bittrex::_InitURIs()
{
	m_uriBase = "https://bittrex.com/api/v1.1/";
	m_uriOpenTransactions = "market/getopenorders";
	m_uriTransactionHistory = "account/getorderhistory";
	m_uriGetCoinInfo = "public/getmarketsummary";
}

void Bittrex::_ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price)
{
	const web::json::value& jsonResponse = p_previousTask.get();
	if (_ResponseIndicatesFailure(jsonResponse)) return;

	auto jarray = jsonResponse.at(U("result")).as_array();
	for (int i = 0; i < jarray.size(); ++i)
	{
		auto tickerSummary = jarray[i];
		*p_price = tickerSummary.at(U("Last")).as_double();

		string marketName = StrUtil::ws2s(tickerSummary.at(U("MarketName")).as_string());
		//std::cout << marketName << " price: " << *p_price << std::endl;
	}
}

void Bittrex::_ParseTransactions(pplx::task<web::json::value> p_previousTask, std::string p_functionId, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	const web::json::value& jsonResponse = p_previousTask.get();
	if (_ResponseIndicatesFailure(jsonResponse)) return;
	auto jarray = jsonResponse.at(U("result")).as_array();

	for (int i = 0; i < jarray.size(); ++i)
	{
		try {
			auto orderDetails		= jarray[i];
			bool isConditional		= _GetIsConditional(orderDetails);					//eg false
			string condition		= _GetCondition(isConditional, orderDetails);
			double conditionTarget	= _GetConditionTarget(isConditional, orderDetails);	//334.00
			string fromAsset		= _GetFromAsset(orderDetails);
			string toAsset			= _GetToAsset(orderDetails);
			double limitValue		= _GetLimitValue(orderDetails);
			
			double quantity			= _GetQuantity(orderDetails);						//eg 16.08
			double quantityRemaining= _GetQuantityRemaining(orderDetails);
			string orderUuid		= _GetTransactionID(orderDetails);					//eg asodnsad1029e

			string orderType		= _GetTransactionType(orderDetails);				//eg LIMIT_BUY / LIMIT_SELL
			ETransactionType e_orderType = (orderType == "LIMIT_BUY") ? ELimitBuy
										 : (orderType == "LIMIT_SELL") ? ELimitSell
										 : EUnsupportedType;

			if (p_functionId == "OpenOrders")
			{	
				 string dateTimeOpened = _GetTimeOpened(orderDetails);						//eg 2017-11-18T13:09:10.533
				 Transaction t(orderUuid, fromAsset, toAsset, e_orderType, quantity, limitValue, dateTimeOpened);
				 p_transactions->push_back(t);
			}
			else if (p_functionId == "TransactionHistory")
			{
				string dateTimeOpened = _GetTimestamp(orderDetails);
				string dateTimeClosed = _GetCloseDate(orderDetails);
				
				double commission	  = _GetCommission(orderDetails); //Currently not used
				double totalPrice	  = _GetTotalPrice(orderDetails); //Currently not used
				
				Transaction t(orderUuid, fromAsset, toAsset, e_orderType, quantity, limitValue, dateTimeOpened);
				t.Close(dateTimeClosed);

				p_transactions->push_back(t);
			}
		}
		catch (const web::json::json_exception e)
		{
			cout << "JSON Parsing failure." << endl;
		}
	}
}

utility::string_t Bittrex::_GetAuthenticatedRequestWithParameters(std::string p_functionId, std::string p_publicKey)
{
	std::string uriBase = "";
	std::vector<std::pair<std::string, std::string>> parameters;

	if (p_functionId == "OpenOrders")
	{
		uriBase = m_uriOpenTransactions;
		parameters.push_back(std::make_pair("apikey", p_publicKey));
		parameters.push_back(std::make_pair("nonce", _GetNonce()));
	}
	else if (p_functionId == "TransactionHistory")
	{
		uriBase = m_uriTransactionHistory;
		parameters.push_back(std::make_pair("apikey", p_publicKey));
		parameters.push_back(std::make_pair("nonce", _GetNonce()));
	}
	
	return StrUtil::s2ws(_GenerateRequestUri(uriBase, parameters));
}


utility::string_t Bittrex::_GetRequestWithParameters_GetPrice(std::string p_ticker)
{
	string uriBase = m_uriGetCoinInfo;
	std::vector<std::pair<std::string, std::string>> parameters;
	parameters.push_back(std::make_pair("market", "btc-" + p_ticker));
	return StrUtil::s2ws(_GenerateRequestUri(uriBase, parameters));
}

void Bittrex::_GetAuthenticatedHeaders(std::string p_functionId, std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	p_additionalHeaders.clear();
	string uri = "";

	if (p_functionId == "OpenOrders")
	{	
		uri = m_uriOpenTransactions;
	}
	else if (p_functionId == "TransactionHistory")
	{
		uri = m_uriTransactionHistory;
	}
	

	//Todo refactoring - Move the below to base class
	web::uri requestUri = web::uri(StrUtil::s2ws(uri));
	web::uri_builder fullRequestUri(requestUri);
	auto requestWithParameters = _GetAuthenticatedRequestWithParameters(p_functionId, p_publicKey);
	std::string completeUri = StrUtil::ws2s(StrUtil::s2ws(m_uriBase).append(requestWithParameters));
	
	//But not this
	p_additionalHeaders["apisign"] = HashUtil::sha512(p_privateKey, completeUri);	
}

utility::string_t Bittrex::_GetUnauthenticatedRequestWithParameters(std::string p_functionId)
{
	std::string uriBase = "";
	std::vector<std::pair<std::string, std::string>> parameters;

	if (p_functionId == "Price")
	{
		uriBase = m_uriGetCoinInfo;
		//parameters.push_back(std::make_pair("market", "btc-" + p_ticker));
		// In fact, this function should probably take  vector<string> as arguments, 
		//and then based on the func ID I can interpret the parameters in the vector of strings.
	}
	

	return StrUtil::s2ws(_GenerateRequestUri(uriBase, parameters));
}

bool Bittrex::_ResponseIndicatesFailure(const web::json::value& p_jsonValue)
{
	bool earlyTerminate = false;
	auto success = p_jsonValue.at(U("success")).as_bool();
	auto message = StrUtil::ws2s(p_jsonValue.at(U("message")).as_string());
		 message = (message == "") ? "NO ADDITIONAL INFORMATION" : message;

	if (!success)
	{
		earlyTerminate = true;
		cout << "Malformed request error: " << message;
	}

	return earlyTerminate;
}

string Bittrex::_GetTransactionID(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("OrderUuid")).as_string());
}

string Bittrex::_GetFromAsset(web::json::value & p_jvalue)
{
	auto exchange = p_jvalue.at(U("Exchange")).as_string();
	std::wstring fromAsset = U("Ticker_unable_to_be_parsed");
	if (exchange != U(""))
	{
		fromAsset = exchange.substr(0, exchange.find_first_of('-'));
	}
	return StrUtil::ws2s(fromAsset);
}

string Bittrex::_GetToAsset(web::json::value & p_jvalue)
{
	auto exchange = p_jvalue.at(U("Exchange")).as_string();
	std::wstring toAsset = U("Ticker_unable_to_be_parsed");
	if (exchange != U(""))
	{
		toAsset = exchange.substr(exchange.find_first_of('-') + 1, exchange.end() - exchange.begin());
	}
	return StrUtil::ws2s(toAsset);
}

string Bittrex::_GetTransactionType(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("OrderType")).as_string());
}

double Bittrex::_GetQuantity(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("Quantity")).as_double();
}

double Bittrex::_GetQuantityRemaining(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("QuantityRemaining")).as_double();
}

double Bittrex::_GetLimitValue(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("Limit")).as_double();
}

string Bittrex::_GetTimeOpened(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("Opened")).as_string());
}

std::string Bittrex::_GetTimestamp(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("TimeStamp")).as_string());
}

bool Bittrex::_GetIsConditional(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("IsConditional")).as_bool();
}

string Bittrex::_GetCondition(bool p_isConditional, web::json::value & p_jvalue)
{

	std::wstring condition = U("NONE");
	if (p_isConditional)
	{
		condition = p_jvalue.at(U("Condition")).as_string();
	}
	return StrUtil::ws2s(condition);
}

double Bittrex::_GetConditionTarget(bool p_isConditional, web::json::value & p_jvalue)
{
	double conditionTarget = -1.0;
	if (p_isConditional)
	{
		conditionTarget = p_jvalue.at(U("ConditionTarget")).as_double();
	}
	return conditionTarget;
}

std::string Bittrex::_GetCloseDate(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("Closed")).as_string());
}

double Bittrex::_GetCommission(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("Commission")).as_double();
}

double Bittrex::_GetTotalPrice(web::json::value & p_jvalue)
{
	return p_jvalue.at(U("Price")).as_double();
}
