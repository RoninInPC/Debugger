#pragma once
#include"Logger.h"
#include"LoggerConsole.h"
#include"LoggerFile.h"

class LoggerFactory {
public:
	//������ ������� ��� ��������� � �������
	static Logger * MakeLogger() {
		return new LoggerConsole();
	}
	//������ ������� ��� ��������� � ��������� ���� �� ����� namefile
	static Logger* MakeLogger(const std::string& namefile) {
		return new LoggerFile(namefile);
	}

};