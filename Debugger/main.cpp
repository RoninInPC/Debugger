#include <iostream>

#include"debugger.h"
int main(unsigned int argc, char* argv[], char* evnp[])
{
	bool isWorking1 = true;
	bool isWorking2 = true;
	bool isWorking3 = true;
	bool isWorking4 = true;
	std::cout << "Will module number one work?\n";
	std::cin >> isWorking1;
	std::cout << "Will module number two work?\n";
	std::cin >> isWorking2;
	std::cout << "Will module number three work?\n";
	std::cin >> isWorking3;
	std::cout << "Will module number four work?\n";
	std::cin >> isWorking4;

	ConsoleDebugger debugger = BuilderConsoleDebugger()
		.workingModuleOne(isWorking1)
		.workingModuleTwo(isWorking2)
		.workingModuleThree(isWorking3)
		.workingModuleFour(isWorking4)
		.build();
	std::string argvs(argv[1]);
	if (debugger.CreateDebugProces(argvs)) {
		debugger.DebugRun();
	}
	/*if(debugger.DebugActiveProces(13384)) {
		debugger.DebugRun();
	}*/
	
}

