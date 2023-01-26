#pragma once
#include<string>
class Logger {

public:
	Logger()=default;
	virtual void write(const std::string& message) = 0;

};