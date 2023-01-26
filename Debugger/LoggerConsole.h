#pragma once
#include"Logger.h"
#include<string>
#include<iostream>
class LoggerConsole:public Logger {
	
public:
	LoggerConsole() = default;
	void write(const std::string& message) {
		std::cout << message;
	}
};