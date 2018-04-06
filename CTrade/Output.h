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
	static void Info(std::string p_message)
	{
		_Print(p_message + "\n");
	}

	static void Warning(std::string p_message)
	{
		Info("Warning: " + p_message);
	}

	static void Error(std::string p_message)
	{
		Info("Error: " + p_message);
	}

private:
	static void _Print(std::string p_message)
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

	const static OutputType m_mode = Console;
};