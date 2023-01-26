#pragma once
#ifndef _MODULEFINDINSTRUCTION_H_
#define _MODULEFINDINSTRUCTION_H_
#include<string>
#include <windows.h>
#include<set>



#include"LoggerFactory.h"
#include"makerstring.h"
class ModuleFindInstruction {
	friend class BuilderFindModuleInstraction;
private:
	std::set<std::string> setOfInstuction = {
	"jz",	"je",	"jne",	"jze",	"jc",	"jnae",	"jb",	"jnc",
	"jae",	"jb",	"jp",	"jnp",	"js",	"jns",	"jo",	"jno",
	"ja",	"jnbe",	"jna",	"jbe",	"jg",	"jnle",	"jge",	"jnl",
	"jl",	"jnge",	"jle",	"jng",	"jcxz"
	};
	Logger* outLogger;
	

	bool isWorking = true;
public:
	bool findInStringInstruction(const std::string& instruction) {
		if (isWorking) {//ищет в строке с адресом и инструкцией имя инструкции из set, есть ограничение, если модуль выключен
			for (auto i = setOfInstuction.begin(); i != setOfInstuction.end(); i++) {
				if (instruction.find(*i) != std::string::npos) {
					return true;
				}
			}
		}
		return false;
	}
	void outputAboutInstruction(const std::string& instruction, CONTEXT& context) {
		if (isWorking)
			if (findInStringInstruction(instruction)) {
				outLogger->write(instruction + "\n");
				outLogger->write(MakerString::MakeRegisterContext(context) + "\n\n");
			}
				
	}
};

class BuilderFindModuleInstraction {//билдер этого модуля (можно задать любой логгер, множество команд, и обозначить его включение/выключение)
private:
	ModuleFindInstruction ans;
public:
	BuilderFindModuleInstraction setOfInstruction(const std::set<std::string>& setOfStr) {
		ans.setOfInstuction = setOfStr;
		return *this;
	}
	BuilderFindModuleInstraction outLogger(Logger* now) {
		ans.outLogger = std::move(now);
		return *this;
	}
	BuilderFindModuleInstraction isWorking(bool isWorking) {
		ans.isWorking = isWorking;
		return *this;
	}
	ModuleFindInstruction build() {
		return ans;
	}
};



#endif