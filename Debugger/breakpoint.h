#pragma once
#ifndef _BREAKPOINT_H_
#define _BREAKPOINT_H_
#include<string>
#include<list>

extern class BuilderBreakPoint;

#include"makerstring.h"
class BreakPoint {
	friend class ConsoleDebugger;
	friend class ModuleDumpListFunction;
	friend class ModuleDumpLibraryFunction;
	friend class BuilderBreakPoint;
private:
	enum class TYPE {
		SOFT = 1,
		HARD = 2

	}type;
	enum class STATE {
		DISABLE = 0,
		ENABLE = 1
	}state = (STATE)1;
	void* addr = 0;
	void* saveByte = 0;

public:
	inline static bool SetBreakpoint(HANDLE debugProcess, std::list<BreakPoint>& list, void* addr, BreakPoint::TYPE type, DWORD64 tid) {
		void* saveByte;
		BreakPoint point;

		auto finding = BreakPoint::findInListByAddr(list, addr);
		if (finding.addr == 0 && finding.saveByte == 0) {
			switch (type) {
			case BreakPoint::TYPE::SOFT:

				saveByte = 0;
				ReadProcessMemory(debugProcess, addr, &saveByte, 1, NULL);
				WriteProcessMemory(debugProcess, addr, "\xCC", 1, NULL);

			
				point.addr = addr;
				point.saveByte = saveByte;
				point.type = type;
				BreakPoint::addInList(list, point);

				break;
			case BreakPoint::TYPE::HARD:



				break;
			default:

				break;
			}
			return true;
		}

		return false;
	}
	inline static bool DeleteBreakpoint(HANDLE debugProcess, std::list<BreakPoint>& list, void* addr, DWORD64 tid)
	{
		auto finding = BreakPoint::findInListByAddr(list, addr);
		if (finding.addr != 0 && finding.saveByte != 0) {
			switch (finding.type) {
			case BreakPoint::TYPE::SOFT:

				WriteProcessMemory(debugProcess, addr,
					&finding.saveByte, 1, NULL);

				break;
			case BreakPoint::TYPE::HARD:



				break;
			default:
				break;
			}
			BreakPoint::deleteInListByAddr(list, addr);
			return true;
		}

		return false;
	}

	inline static BreakPoint findInListByAddr(std::list<BreakPoint>& list, void* addr) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).addr == addr) {
				return *l;
			}
		}
		return BreakPoint();
	}

	inline static void addInList(std::list<BreakPoint>& list,const BreakPoint& point) {
		list.push_back(point);
	}
	inline static bool deleteInListByAddr(std::list<BreakPoint>& list, void* addr) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).addr == addr) {
				list.erase(l);
				return true;
			}
		}
		return false;
	}

	std::string getInfo() {
		std::string ans;

		switch (type){
		case BreakPoint::TYPE::SOFT:
			ans += "SOFT ";
			break;
		case BreakPoint::TYPE::HARD:
			ans += "HARD ";
			break;
		default:
			break;
		}

		switch (state){
		case BreakPoint::STATE::DISABLE:
			ans += "DISABLE ";
			break;
		case BreakPoint::STATE::ENABLE:
			ans += "ENABLE ";
			break;
		default:
			break;
		}

		ans += "ADDR: " + MakerString::MakeHexNum((DWORD64)addr) + " SAVE: " + std::to_string((size_t)saveByte) + "\n";

		return ans;
	}
	bool operator==(const BreakPoint& bp) {
		return this->addr == bp.addr;
	}
};


class BuilderBreakPoint {
private:
	BreakPoint ans;
public:
	BuilderBreakPoint() = default;

	BuilderBreakPoint type(BreakPoint::TYPE type) {
		ans.type = type;
		return *this;
	}
	BuilderBreakPoint state(BreakPoint::STATE state) {
		ans.state = state;
		return *this;
	}
	BuilderBreakPoint addr(void* addr) {
		ans.addr = addr;
		return *this;
	}
	BuilderBreakPoint saveByte(void* saveByte) {
		ans.saveByte = saveByte;
		return *this;
	}
	BreakPoint build() {
		return ans;
	}
};
	


#endif