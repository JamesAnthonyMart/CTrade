#include "stdafx.h"
#include "CppUnitTest.h"

#include "Transaction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CTradeUnitTesting
{		
	TEST_CLASS(Transaction)
	{
	public:
		
		TEST_METHOD(EarningsPercentage)
		{
			Assert::IsTrue(true);
		}

	};




	//TEST_CLASS(Transaction)
	//{
	//public:
	//	Transaction()
	//	{
	//		Logger::WriteMessage("In Transaction");
	//	}
	//	~Transaction()
	//	{
	//		Logger::WriteMessage("In ~Transaction");
	//	}



	//	TEST_CLASS_INITIALIZE(TransactionInitialize)
	//	{
	//		Logger::WriteMessage("In Class Initialize");
	//	}
	//	TEST_CLASS_CLEANUP(TransactionCleanup)
	//	{
	//		Logger::WriteMessage("In Class Cleanup");
	//	}


	//	BEGIN_TEST_METHOD_ATTRIBUTE(Method1)
	//		TEST_OWNER(L"OwnerName")
	//		TEST_PRIORITY(1)
	//		END_TEST_METHOD_ATTRIBUTE()

	//		TEST_METHOD(EarningsPercentage)
	//	{
	//		Logger::WriteMessage("In Method1");
	//		Assert::AreEqual(0, 0);
	//	}

	//	TEST_METHOD(Method2)
	//	{
	//		Assert::Fail(L"Fail");
	//	}


}