#pragma once
#ifndef _MODULEDUMPLISTFUNCTION_H_
#define _MODULEDUMPLISTFUNCTION_H_

#include"LoggerFactory.h"
#include"parsingargument.h"
#include"breakpoint.h"
#include"context.h"
class ModuleDumpListFunction {
	friend class BuilderModuleDumpListFunction;
	friend class ConsoleDebugger;
private:


	std::map<void*, std::string> mapOfFunctions;
	
	Logger* outLogger = LoggerFactory::MakeLogger();

	ParsingArgument* parsing = new ParsingArgument(); //��������� ��� ������� ���������� � ������������� ��������


	void* lastBreakpointAddress = 0;
	void* lastBreakpointAddressSafe = 0;


	int countOfCallRet = 0; //�� ������ ����, ��� ������� ����� ����� ������ ���� ������ ������ �������

	bool findRet = false;

	bool isFirstTimeSet = true;

	bool isWorking = true;
public:

	bool findAddr(void* addr) {
		return mapOfFunctions.find(addr) != mapOfFunctions.end();
	}

	int getCountOfCallRet() {
		return this->countOfCallRet;
	}

	void setCountOfCallRet(int count) {
		this->countOfCallRet = count;
	}

	bool getFindRet() {
		return this->findRet && isWorking;//���� �� ��������, �� ����� false
	}

	void setFindRet(bool findRet) {
		this->findRet = findRet;
	}

	void* getLastBreakPointAddress() {
		return this->lastBreakpointAddress;
	}

	void setLastBreakPointAddress(void* address) {
		this->lastBreakpointAddress = address;
	}

	void* getLastBreakPointAddressSafe() {
		return this->lastBreakpointAddressSafe;
	}

	void setLastBreakPointAddressSafe(void* address) {
		this->lastBreakpointAddressSafe = address;
	}

	std::vector<void*> getArguments(HANDLE debugProcess, const CONTEXT& context, void* addr) {
		if (!isWorking) {
			return {};
		}
		#ifndef _AMD64_
		const std::vector<DWORD64> registerArgs;
		#else
			const std::vector<DWORD64> registerArgs = { context.Rcx, context.Rdx, context.R8, context.R9 };
		#endif;		
		
		auto countArg = parsing->getBaseOfCountArg()[mapOfFunctions[addr]];

		std::vector<void*> args;

		void* arg;

		for (int i = 0; i < countArg; i++) {
			#ifndef _AMD64_
				DWORD copy = context.Esp + 4 + i * 4;
				void* address = (void*)copy;;
				ReadProcessMemory(debugProcess, address, &arg, 4, NULL);
			#else
			if (i < 4)
			{
				arg = (void*)registerArgs[i];
			}
			else
			{
				DWORD64 copy = context.Rsp + 8 + (i - 4) * 8;
				void* address = (void*)copy;
				ReadProcessMemory(debugProcess, address, &arg, 8, NULL);
			}	
			#endif;
			args.push_back(arg);
		}

		return args;
	}

	void outCallAddress(void* addrFun, const CONTEXT& context) {
		if (isWorking)
			outLogger->write(MakerString::OutInfo(mapOfFunctions[addrFun], "CALL ADDRESS:", getCallAddress(context)));
	}

	void outParsingAnsValue(void* addrFun, const CONTEXT& context) {
		if (isWorking) {
			auto nameFun = mapOfFunctions[addrFun];
			outLogger->write(nameFun + "\nRETURN PARSING VALUE:\n" + this->parsing->getInfoAboutAns(nameFun, getAnsValue(context)) + "\n");
		}
	}

	void outParsingArguments(HANDLE debugProcess, const CONTEXT& context, void* addrFun) {
		if (isWorking) {
			auto nameFun = mapOfFunctions[addrFun];
			outLogger->write(this->parsing->getInfoAboutAllArgument(nameFun, getArguments(debugProcess, context, addrFun)));
		}
	}

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

	void setLibraryBreakpoints(HANDLE debugProcess, std::list<BreakPoint>& points,DWORD tid){
		if (!isWorking) {
			return;
		}
		if (isFirstTimeSet) {
			for (auto i = mapOfFunctions.begin(); i != mapOfFunctions.end(); i++) {
				BreakPoint::SetBreakpoint(debugProcess, points, (*i).first, BreakPoint::TYPE::SOFT, tid);
			}
			isFirstTimeSet = false;
		}
	}

	void seeCountCallAndRetAndOut(const std::string& instruction, const CONTEXT& context) {
		if (getFindRet()) {
			//�������������� ������ ��� ������ ���������� call
			ModuleFindInstruction helperFindCall = BuilderFindModuleInstraction().setOfInstruction({ "call" }).
				isWorking(true).build();
			if (helperFindCall.findInStringInstruction(instruction)) {
				//���� ����� ��������� call, �� ����������� �������
				setCountOfCallRet(getCountOfCallRet() + 1);
			}

			//�������������� ������ ��� ������ ���������� ���� ret
			ModuleFindInstruction helperFindRet = BuilderFindModuleInstraction().setOfInstruction({ "ret","retn","retf" }).
				isWorking(true).build();
			if (helperFindRet.findInStringInstruction(instruction)) {
				//���� ����� ret, �� ��������� �������
				setCountOfCallRet(getCountOfCallRet() - 1);
			}

			//���� ���������� ����� 0, ������ ��� ����� ������� ������������ ������� 
			if (getCountOfCallRet() == 0) {
				//���� �����, �� ������� � ���� ����������� ������������ ��������
				outParsingAnsValue(getLastBreakPointAddressSafe(), context);
				//������� ���������� ����� � ������� � ����� false
				setLastBreakPointAddressSafe(0);
				setFindRet(false);
			}
		}
	}

	void saveDeleteBreakpointFunction(HANDLE debugProcess, DWORD64 tid, void* addr) {
		if (findAddr(addr) && isWorking) {
			//��������� ����� ��������� �������� ����� �������� �������
			setLastBreakPointAddress(addr);
			//��������� ����� ��������� �������� ����� �������� ������� (��� ������ ret)
			setLastBreakPointAddressSafe(addr);
			//���������, ��� ���� ret
			setFindRet(true);
			//��������� ����� callRet call ++ ret--
			setCountOfCallRet(1);
			//�������� �������� � ������ ���� ����������� � ���� 
			auto context = ContextHelper::SetFlagTraceInContext(tid);
			//������� � ���� ����� ��������
			outCallAddress(addr, context);
			//������� � ���� ����������� ��������� 
			outParsingArguments(debugProcess, context, addr);
		}
	}

	void resetDeleteBreakpoint(HANDLE debugProcess,std::list<BreakPoint>& list,DWORD64 tid) {
		if (getLastBreakPointAddress() != 0 && isWorking) {
			//�������������� ����� �������� ������� 
			BreakPoint::SetBreakpoint(debugProcess, list, getLastBreakPointAddress(), BreakPoint::TYPE::SOFT, tid);
			setLastBreakPointAddress(0);
		}
	}

};

class BuilderModuleDumpListFunction {
private:
	ModuleDumpListFunction ans;
public:
	BuilderModuleDumpListFunction isWorking(bool isWorking) {
		ans.isWorking = isWorking;
		return *this;
	}

	BuilderModuleDumpListFunction parsing(int maxLengthStr, int maxLengthArray,int maxNesting) {
		ans.parsing = new ParsingArgument(BuilderParsingArgument().maxLengthStr(maxLengthStr).
			maxLengthArray(maxLengthArray).maxNesting(maxNesting).build());

		auto vec = ans.parsing->getNamesOfFunctions();

		//ans.mapOfFunctions.insert({ (void*)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "HeapCreate"),"HeapCreate"});
		for (auto i = vec.begin(); i != vec.end(); i++) {
			ans.mapOfFunctions.insert({ (void*)GetProcAddress(GetModuleHandle(L"kernel32.dll"), (*i).c_str()),(*i) });
		}

		return *this;
	}
	BuilderModuleDumpListFunction outLogger(Logger* logger) {
		ans.outLogger = logger;
		return *this;
	}

	ModuleDumpListFunction build() {
		return ans;
	}

};

#endif;