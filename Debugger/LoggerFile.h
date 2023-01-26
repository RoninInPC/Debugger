#pragma once
#include"Logger.h"

#include<string>
#include<fstream>

class LoggerFile : public Logger
{
private:
	std::string filename;
public:
	LoggerFile(const std::string& filename) {
		//создание файла (очистка уже созданного)
		this->filename = filename;
	}
	void write(const std::string& message) {
		//заносит строку в файл
		std::ofstream fout(filename, std::ios::app);
		fout << message;
		fout.close();
	}
};
