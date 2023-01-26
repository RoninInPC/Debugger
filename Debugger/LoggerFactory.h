#pragma once
#include"Logger.h"
#include"LoggerConsole.h"
#include"LoggerFile.h"

class LoggerFactory {
public:
	//Логгер выводит все сообщения в консоль
	static Logger * MakeLogger() {
		return new LoggerConsole();
	}
	//Логгер выводит все сообщения в созданный файл по имени namefile
	static Logger* MakeLogger(const std::string& namefile) {
		return new LoggerFile(namefile);
	}

};