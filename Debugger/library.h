#pragma once
#ifndef _LIBRARY_H_
#define _LIBRARY_H_
#include<windows.h>
#include<string>
#include<list>

#include"makerstring.h"
class Library {
	friend class BuilderLibrary;
private:
	DWORD64 pid;
	DWORD64 tid;
	LPVOID addrDll = 0;
	std::string nameOfDll = "nope";
public:
	inline static Library findInListByAddr(std::list<Library>& list, void* addr) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).addrDll == addr) {
				return *l;
			}
		}
		return Library();
	}

	inline static void addInList(std::list<Library>& list,const Library& point) {
		list.push_back(point);
	}
	inline static bool deleteInListByAddr(std::list<Library>& list, void* addr) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).addrDll == addr) {
				list.erase(l);
				return true;
			}
		}
		return false;
	}


	bool operator==(const Library& l) {
		return this->addrDll == l.addrDll;
	}
	std::string getInfo() {
		return "PID: " + std::to_string(pid) + " TID: " + std::to_string(tid) 
			+ " ADDR: " + MakerString::MakeHexNum((DWORD64)addrDll) + " NAME: " + nameOfDll + "\n";
	}
};

class BuilderLibrary {
private:
	Library ans;
public:
	BuilderLibrary pid(DWORD64 pid) {
		ans.pid = pid;
		return *this;
	}
	BuilderLibrary tid(DWORD64 tid) {
		ans.tid = tid;
		return *this;
	}
	BuilderLibrary addr(void* addr) {
		ans.addrDll = addr;
		return *this;
	}
	BuilderLibrary name(const std::string& name) {
		ans.nameOfDll = name;
		return *this;
	}
	Library build() {
		return ans;
	}
};

#endif