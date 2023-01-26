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
		//�������� ����� (������� ��� ����������)
		this->filename = filename;
	}
	void write(const std::string& message) {
		//������� ������ � ����
		std::ofstream fout(filename, std::ios::app);
		fout << message;
		fout.close();
	}
};
