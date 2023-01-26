#pragma once
#ifndef _MODULETRACEFUNCTION_H_
#define _MODULETRACEFUNCTION_H_
#include<set>
#include<string>
#include<vector>

#include"modulefindinstruction.h"
#include"context.h"
class ModuleTraceFunction {
	friend class BuilderModuleTraceFunction;
private:
	std::set<std::string> commandFill = {
		"mov",
		"movsx",
		"movzx",
		"lea"
	};
	
	std::vector<std::string> registersName = {
		"al", "ah", "ax", "eax", "rax",
		"bl", "bh", "bx", "ebx", "rbx",
		"cl", "ch", "cx", "ecx", "rcx",
		"dl", "dh", "dx", "edx", "rdx",
		"si", "esi", "rsi",
		"di", "edi", "rdi",
		"bp", "ebp", "rbp",
		"sp", "esp", "rsp",
		"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
	};
	
	Logger* outLogger = LoggerFactory::MakeLogger();

	int Ret = 0;
	
	std::vector<std::vector<int>> registersFill;
	std::vector<std::vector<void*>> args;
	std::vector<int> countOfRegArg;
	std::vector<int> allCountNum;

	std::vector<std::pair<std::string, int>> safeArg;

	int allCount = 0;

	bool isWorking = true;
private:
	void* getAnsValue(const CONTEXT& context) {
		if (!isWorking) {
			return 0;
		}
		#ifndef _AMD64_
			return (void*)context.Eax;
		#else
			return (void*)context.Rax;
		#endif;
	}
	void* getCallAddress(const CONTEXT& context) {
		if (!isWorking) {
			return 0;
		}
		#ifndef _AMD64_
			return (void*)context.Esp;
		#else
			return (void*)context.Rsp;
		#endif;
	}
	void outCallAddress(const std::string& instruction, const CONTEXT& context) {
		outLogger->write("\nINSTRUCTION FOR CALL FUN" + std::to_string(allCountNum.back()) + ": " + instruction + "\n");
		outLogger->write(MakerString::OutInfo("FUN" + std::to_string(allCountNum.back()), "CALL ADDRESS:", getCallAddress(context)));
	}
	void outAnsValue(const CONTEXT& context) {
		if (isWorking) {
			outLogger->write("FUN" + std::to_string(allCountNum.back()) + "\nRETURN VALUE:\n" + MakerString::MakeHexNum((DWORD64)getAnsValue(context)) + "\n");
		}
	}
	void outWriteArgs() {
		outLogger->write("FUN" + std::to_string(allCountNum.back()) + "\nCOUNT ARGS:\n" + std::to_string(args[Ret - 1].size()) + "\n");
		if (args[Ret - 1].size() > 0) {
			outLogger->write("ARGS:\n");
			for (auto part : args[Ret - 1]) {
				outLogger->write(MakerString::MakeHexNum((DWORD64)part) + " ");
			}
			outLogger->write("\n");
		}
	}
private:

	std::pair<int, std::string> findReg(const std::string& instruction) {
		for (auto part : registersName) {
			auto pos = instruction.find(part);
			if ( pos != std::string::npos) {
				return { pos,part };
			}
		}
		return { std::string::npos,"nope" };
	}

	int getNumInRegisterFill(const std::string& str) {
		if (str == "al" || str == "ah" || str == "ax" || str == "eax" || str == "rax")
			return 0;
		if (str == "bl" || str == "bh" || str == "bx" || str == "ebx" || str == "rbx")
			return 1;
		if (str == "cl" || str == "ch" || str == "cx" || str == "ecx" || str == "rcx")
			return 2;
		if (str == "dl" || str == "dh" || str == "dx" || str == "edx" || str == "rdx")
			return 3;
		if (str == "si" || str == "esi" || str == "rsi")
			return 4;
		if (str == "di" || str == "edi" || str == "rdi")
			return 5;
		if (str == "bp" || str == "ebp" || str == "rbp")
			return 6;
		if (str == "sp" || str == "esp" || str == "rsp")
			return 7;
		if (str >= "r8" && str <= "r15")
			return std::stoi(str.substr(1, 3));
	}
	void* getRegValue(HANDLE debugProcess, const CONTEXT& context, const std::string& rightInstruction) {
		auto finding = findReg(rightInstruction);
		std::string str = finding.second;
		DWORD64 numReg = 0;
		#ifndef _AMD64_
		if (str == "al" || str == "ah" || str == "ax" || str == "eax")
			numReg = context.Eax;
		if (str == "bl" || str == "bh" || str == "bx" || str == "ebx")
			numReg = context.Ebx;
		if (str == "cl" || str == "ch" || str == "cx" || str == "ecx")
			numReg = context.Ecx;
		if (str == "dl" || str == "dh" || str == "dx" || str == "edx")
			numReg = context.Edx;
		if (str == "si" || str == "esi")
			numReg = context.Esi;
		if (str == "di" || str == "edi")
			numReg = context.Edi;
		if (str == "bp" || str == "ebp")
			numReg = context.Ebp;
		if (str == "sp" || str == "esp")
			numReg = context.Esp;
		#else
		if (str == "al" || str == "ah" || str == "ax" || str == "eax" || str == "rax")
			numReg = context.Rax;
		if (str == "bl" || str == "bh" || str == "bx" || str == "ebx" || str == "rbx")
			numReg = context.Rbx;
		if (str == "cl" || str == "ch" || str == "cx" || str == "ecx" || str == "rcx")
			numReg = context.Rcx;
		if (str == "dl" || str == "dh" || str == "dx" || str == "edx" || str == "rdx")
			numReg = context.Rdx;
		if (str == "si" || str == "esi" || str == "rsi")
			numReg = context.Rsi;
		if (str == "di" || str == "edi" || str == "rdi")
			numReg = context.Rdi;
		if (str == "bp" || str == "ebp" || str == "rbp")
			numReg = context.Rbp;
		if (str == "sp" || str == "esp" || str == "rsp")
			numReg = context.Rsp;
		#endif;

		int n = 0;
		auto it = std::find_if(str.begin(), str.end(), isdigit);
		if (it != str.end()) n = std::atoi(str.c_str() + (it - str.begin()));
		numReg += n;
		if (str.find('[') != std::string::npos) {
			void* ans;
			ReadProcessMemory(debugProcess, &numReg, &ans, 1, NULL);

			return ans;
		}
		return (void*)numReg;
	}

public:
	void workingModule(HANDLE debugProcess, const std::string& instruction, const CONTEXT& context) {
		if (!isWorking)
			return;
		isCallInstruction(instruction, context);
		isArgInInstruction(debugProcess,instruction, context);
		isRetInstruction(instruction, context);
	}


	void isCallInstruction(const std::string& instruction,const CONTEXT& context) {
		if (!isWorking)
			return;
		
		ModuleFindInstruction helperFindCall = BuilderFindModuleInstraction().setOfInstruction({ "call" }).build();
		if (helperFindCall.findInStringInstruction(instruction)) {
			
			allCountNum.push_back(allCount);

			outCallAddress(instruction, context);
			

			allCount++;
			Ret++;
			
			registersFill.push_back(std::vector<int>(16, false));
			args.push_back(std::vector<void*>());
			countOfRegArg.push_back(0);
		}
	}

	void isArgInInstruction(HANDLE debugProcess, const std::string& instruction, const CONTEXT& context) {
		if (!isWorking || Ret == 0)
			return;

		ModuleFindInstruction helperFindPush = BuilderFindModuleInstraction().setOfInstruction({ "push" }).build(); 
		//сохранение, когда происходит push регистра
		if (helperFindPush.findInStringInstruction(instruction)) { 
			auto finding = findReg(instruction);
			if (finding.second != "nope")
				safeArg.push_back({ finding.second,registersFill[Ret - 1][getNumInRegisterFill(finding.second)] });
			else
				safeArg.push_back({ "nope",0 });
		}

		ModuleFindInstruction helperFindPop = BuilderFindModuleInstraction().setOfInstruction({ "pop" }).build(); 
		//возвращение запушенного регистра
		if (helperFindPop.findInStringInstruction(instruction)) {
			auto finding = findReg(instruction);
			if (finding.second != "nope") {
				auto back = safeArg.back();
				if (back.first != "nope")
					registersFill[Ret - 1][getNumInRegisterFill(finding.second)] = back.second;
				safeArg.pop_back();
			}
		}


		ModuleFindInstruction helperFindFillInstruction = BuilderFindModuleInstraction().setOfInstruction(commandFill).build();

		if (helperFindFillInstruction.findInStringInstruction(instruction)) {
			auto num = instruction.find(',');

			if (num != std::string::npos) {
				//разбиваем инструкцию по запятой
				std::string leftPart = instruction.substr(0, num),
					rightPart = instruction.substr(num, instruction.size());

				auto findingLeft = findReg(leftPart),
					findingRight = findReg(rightPart);

				if (findingLeft.first != std::string::npos) {
					if (findingRight.second == "sp" || findingRight.second == "esp" || findingRight.second == "rsp")
						registersFill[Ret - 1][getNumInRegisterFill(findingLeft.second)] = 2; //указываем какой регистр используется как копия ESP/RSP
						else
						registersFill[Ret - 1][getNumInRegisterFill(findingLeft.second)] = 1; //указываем какой регистр используется как ПРИЁМНИК (1)
				}
				if (findingRight.first != std::string::npos) {
					auto numInRegisterFill = getNumInRegisterFill(findingRight.second);
					if (registersFill[Ret - 1][numInRegisterFill]!=1) { //если регистр ИСТОЧНИК или копией rsp, но он раньше не был ПРИЁМНИКОМ, то он аргумент
						if (registersFill[Ret - 1][numInRegisterFill] != 2 && findingRight.second != "sp" && findingRight.second != "esp" && findingRight.second != "rsp")
							countOfRegArg[Ret - 1]++;
						if(registersFill[Ret - 1][numInRegisterFill] == 2 && rightPart.find('[')!=std::string::npos)
							args[Ret - 1].push_back(getRegValue(debugProcess,context, rightPart));
						
						if(registersFill[Ret - 1][numInRegisterFill] == 0)
							args[Ret - 1].push_back(getRegValue(debugProcess,context, rightPart));
					}
				}
			}
		}

	}

	void isRetInstruction(const std::string& instruction, const CONTEXT& context) {
		if (!isWorking || Ret==0)
			return;
		ModuleFindInstruction helperFindRet = BuilderFindModuleInstraction().setOfInstruction({ "ret","retn","retf"}).build();
		if (helperFindRet.findInStringInstruction(instruction)) {
			
			outWriteArgs();

			std::string convention = "don't understand convention";
			if (countOfRegArg.back() > 0) {
				#ifndef _AMD64_
				convention = "m_fastcall"; //в х86 для передачи аргументов в регистры (2 штуки) используется m_fastcall 
				#else
				convention = "fastcall";
				#endif
			}
			
			if (std::find_if(instruction.begin(), instruction.end(), isdigit) != instruction.end()) {
				convention = "stdcall"; //если заканчивается инструкцией к примеру ret 12, то очевидно, что stdcall в x86 и в x64
			}
			else {
				convention = "cdecl";
			}
			outLogger->write("FUN" + std::to_string(allCountNum.back()) + "\n" + "CONVENTION:\n" + convention + "\n");
			outAnsValue(context);
			Ret--;
			registersFill.pop_back();
			args.pop_back();
			countOfRegArg.pop_back();
			allCountNum.pop_back();
		}
	}
};

class BuilderModuleTraceFunction {
private:
	ModuleTraceFunction ans;
public:
	BuilderModuleTraceFunction outLogger(Logger* logger) {
		ans.outLogger = logger;
		return *this;
	}
	BuilderModuleTraceFunction isWorking(bool isWorking) {
		ans.isWorking = isWorking;
		return *this;
	}
	ModuleTraceFunction build() {
		return ans;
	}
};


#endif;