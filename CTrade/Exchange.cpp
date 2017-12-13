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
	//  If you hit this, you probably forgot to finish implementing a new Exchange.
	assert(m_uriBase != "");

	p_openTransactions->clear();
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
	return client.request(request)
		.then(std::bind(&Exchange::ExtractJSON,					this, std::placeholders::_1))
		.then(std::bind(&Exchange::_ParseOpenTransactions,	this, std::placeholders::_1, p_openTransactions));

}

pplx::task<void> Exchange::GetTransactionHistory(std::string p_publicKey, std::string p_privateKey, std::shared_ptr<std::vector<Transaction>> p_exchangeTransHistory)
{
	//https://bittrex.com/api/v1.1/account/getorderhistory
	
	assert(m_uriBase != "");

	p_exchangeTransHistory->clear();
	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetRequestWithParameters_TransactionHistory(p_publicKey));

	//Add necessary headers
	map<string, string> requestHeaders;
	_GetAdditionalHeaders_TransactionHistory(p_publicKey, p_privateKey, requestHeaders);
	for (auto header : requestHeaders)
	{
		request.headers().add(StrUtil::s2ws(header.first), StrUtil::s2ws(header.second));
	}

	web::http::client::http_client client(baseUri);
	
	return client.request(request)
		.then(std::bind(&Exchange::ExtractJSON, this, std::placeholders::_1))
		.then(std::bind(&Exchange::_ParseTransactionHistory, this, std::placeholders::_1, p_exchangeTransHistory));
}

pplx::task<void> Exchange::GetPrice(std::string p_ticker, double* p_price)
{	
	//  If you hit this, you probably forgot to finish implementing a new Exchange.
	assert(m_uriBase != "");

	web::uri baseUri = web::uri(StrUtil::s2ws(m_uriBase));
	web::http::http_request request(web::http::methods::GET);

	//Create web request with specific exchange parameters
	request.set_request_uri(_GetRequestWithParameters_GetPrice(p_ticker));

	//Add necessary headers
	map<string, string> requestHeaders;
	_GetAdditionalHeaders_GetPrice(requestHeaders);
	for (auto header : requestHeaders)
	{
		request.headers().add(StrUtil::s2ws(header.first), StrUtil::s2ws(header.second));
	}

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

void Exchange::_InitURIs()
{
	assert(false); // Child class must override this function
}

void Exchange::_ParseOpenTransactions(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	assert(false); // Child class must override this function
}

void Exchange::_ParseTransactionHistory(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	assert(false); // Child class must override this function
}

void Exchange::_ParsePrice(pplx::task<web::json::value> p_previousTask, double* p_price)
{
	assert(false); // Child class must override this function
}

utility::string_t Exchange::_GetRequestWithParameters_OpenOrders(std::string p_publicKey)
{
	assert(false); // Child class must override this function
	return utility::string_t(); // Silence the compiler
}

utility::string_t Exchange::_GetRequestWithParameters_TransactionHistory(std::string p_publicKey)
{
	assert(false); // Child class must override this function
	return utility::string_t(); // Silence the compiler
}

utility::string_t Exchange::_GetRequestWithParameters_GetPrice(std::string p_ticker)
{
	assert(false); // Child class must override this function
	return utility::string_t(); // Silence the compiler
}

void Exchange::_GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	assert(false); // Child class must override this function
}

void Exchange::_GetAdditionalHeaders_TransactionHistory(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	assert(false); // Child class must override this function
}

void Exchange::_GetAdditionalHeaders_GetPrice(std::map<std::string, std::string>& p_additionalHeaders)
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
	m_uriGetCoinInfo = "public/getmarketsummary";
}

void Bittrex::_ParseOpenTransactions(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	const web::json::value& jsonResponse = p_previousTask.get();
	if (_ResponseIndicatesFailure(jsonResponse)) return;

	auto jarray = jsonResponse.at(U("result")).as_array();
	for (int i = 0; i < jarray.size(); ++i)
	{
		try {
			auto orderDetails = jarray[i];
			string orderUuid =			_GetTransactionID(orderDetails);					//eg asodnsad1029e
			string fromAsset =			_GetFromAsset(orderDetails);						//eg BTC
			string toAsset =			_GetToAsset(orderDetails);							//eg ETH
			string orderType =			_GetTransactionType(orderDetails);					//eg LIMIT_BUY / LIMIT_SELL
			double quantity =			_GetQuantity(orderDetails);							//eg 16.08
			double quantityRemaining =	_GetQuantityRemaining(orderDetails);				//eg 13.00
			double limitValue =			_GetLimitValue(orderDetails);						//eg 124.46
			string dateTimeOpened =		_GetTimeOpened(orderDetails);						//eg 2017-11-18T13:09:10.533
			bool isConditional =		_GetIsConditional(orderDetails);					//eg false
			string condition =			_GetCondition(isConditional, orderDetails);			//eg NONE
			double conditionTarget =	_GetConditionTarget(isConditional, orderDetails);	//334.00
			
			ETransactionType e_orderType =	(orderType == "LIMIT_BUY") ? ELimitBuy :
											(orderType == "LIMIT_SELL") ? ELimitSell
											: EUnsupportedType;

			//std::cout << dateTimeOpened << ":   " << orderType << " " << quantity << " " << toAsset 
					  //<< " in market " << fromAsset << "-" << toAsset << " for " << limitValue << " each." << "\n\n";

			p_transactions->push_back(Transaction(orderUuid, fromAsset, toAsset, e_orderType, quantity, limitValue, dateTimeOpened));

		}
		catch (const web::json::json_exception e)
		{
			cout << "JSON Parsing failure." << endl;
		}
	}
}

void Bittrex::_ParseTransactionHistory(pplx::task<web::json::value> p_previousTask, std::shared_ptr<std::vector<Transaction>> p_transactions)
{
	const web::json::value& jsonResponse = p_previousTask.get();
	if (_ResponseIndicatesFailure(jsonResponse)) return;

	auto jarray = jsonResponse.at(U("result")).as_array();
	for (int i = 0; i < jarray.size(); ++i)
	{
		try {
			auto orderDetails = jarray[i];
			string dateTimeClosed = _GetCloseDate(orderDetails);
			double commission = _GetCommission(orderDetails);
			bool isConditional = _GetIsConditional(orderDetails);
			string condition = _GetCondition(isConditional, orderDetails);
			double conditionTarget = _GetConditionTarget(isConditional, orderDetails);
			string fromAsset = _GetFromAsset(orderDetails);
			string toAsset = _GetToAsset(orderDetails);
			double limitValue = _GetLimitValue(orderDetails);
			string orderType = _GetTransactionType(orderDetails);
			string orderUuid = _GetTransactionID(orderDetails);
			double totalPrice = _GetTotalPrice(orderDetails);
			double quantity = _GetQuantity(orderDetails);
			double quantityRemaining = _GetQuantityRemaining(orderDetails);
			string dateTimeOpened = _GetTimestamp(orderDetails);

			ETransactionType e_orderType =	(orderType == "LIMIT_BUY") ? ELimitBuy :
											(orderType == "LIMIT_SELL") ? ELimitSell
											: EUnsupportedType;

			//std::cout << dateTimeClosed << ":   " << orderType << " closed for " << quantity << " " << toAsset 
					  //<< " in market " << fromAsset << "-" << toAsset << " for " << limitValue << " each." << "\n\n";

			Transaction t(orderUuid, fromAsset, toAsset, e_orderType, quantity, limitValue, dateTimeOpened);
			t.Close(dateTimeClosed);
			p_transactions->push_back(t);
		}
		catch (const web::json::json_exception e)
		{
			cout << "JSON Parsing failure." << endl;
		}
	}
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

utility::string_t Bittrex::_GetRequestWithParameters_OpenOrders(std::string p_publicKey)
{
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriOpenTransactions));
	web::uri_builder fullRequestUri(requestUri);
	fullRequestUri.append_query(U("apikey"), StrUtil::s2ws(p_publicKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	fullRequestUri.append_query(U("nonce"), ssNonce.str().c_str());

	return fullRequestUri.to_string();
}

utility::string_t Bittrex::_GetRequestWithParameters_TransactionHistory(std::string p_publicKey)
{
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriTransactionHistory));
	web::uri_builder fullRequestUri(requestUri);
	fullRequestUri.append_query(U("apikey"), StrUtil::s2ws(p_publicKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	fullRequestUri.append_query(U("nonce"), ssNonce.str().c_str());

	auto temp = fullRequestUri.to_string();
	return temp;
}

utility::string_t Bittrex::_GetRequestWithParameters_GetPrice(std::string p_ticker)
{
	//https://bittrex.com/api/v1.1/public/getmarketsummary?market=btc-ltc    
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriGetCoinInfo));
	web::uri_builder fullRequestUri(requestUri);
	fullRequestUri.append_query(U("market"), U("btc-") + StrUtil::s2ws(p_ticker));

	auto temp = fullRequestUri.to_string();
	return temp;
}

void Bittrex::_GetAdditionalHeaders_OpenOrders(std::string p_publicKey, std::string p_privateKey, map<string, string>& p_additionalHeaders)
{
	p_additionalHeaders.clear();
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriOpenTransactions));
	web::uri_builder fullRequestUri(requestUri);
	std::string completeUri = StrUtil::ws2s(StrUtil::s2ws(m_uriBase).append(_GetRequestWithParameters_OpenOrders(p_publicKey)));

	p_additionalHeaders["apisign"] = HashUtil::sha512(p_privateKey, completeUri);
}

void Bittrex::_GetAdditionalHeaders_TransactionHistory(std::string p_publicKey, std::string p_privateKey, std::map<std::string, std::string>& p_additionalHeaders)
{
	p_additionalHeaders.clear();
	web::uri requestUri = web::uri(StrUtil::s2ws(m_uriTransactionHistory));
	web::uri_builder fullRequestUri(requestUri);
	
	fullRequestUri.append_query(U("apikey"), StrUtil::s2ws(p_publicKey));
	std::stringstream ssNonce; ssNonce.str(""); ssNonce << std::time(nullptr);
	fullRequestUri.append_query(U("nonce"), ssNonce.str().c_str());
	std::string completeUri = StrUtil::ws2s(StrUtil::s2ws(m_uriBase).append(fullRequestUri.to_string()));

	p_additionalHeaders["apisign"] = HashUtil::sha512(p_privateKey, completeUri);
}

void Bittrex::_GetAdditionalHeaders_GetPrice(std::map<std::string, std::string>& p_additionalHeaders)
{
	p_additionalHeaders.clear();
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

std::string Bittrex::_GetTimestamp(web::json::value & p_jvalue)
{
	return StrUtil::ws2s(p_jvalue.at(U("TimeStamp")).as_string());
}
