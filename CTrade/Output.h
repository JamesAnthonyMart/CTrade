#pragma once

#include <iostream>
#include <cassert>
#include <string>

enum OutputType
{
	Console = 0,
	GUI = 1,
};

class Output 
{
public:
	static void Print(std::string p_message)
	{
		if (m_mode == Console)
		{
			std::cout << p_message;
		}
		else
		{
			assert(false); //not supported yet
		}
	}
	static void PrintLn(std::string p_message)
	{
		Print(p_message + "\n");
	}
	static void PrintLnErr(std::string p_message)
	{
		PrintLn("Error: " + p_message);
	}

	const static OutputType m_mode = Console;
};