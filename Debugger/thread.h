#pragma once
#ifndef _THREAD_H_
#define _THREAD_H_
#include<windows.h>
#include<string>
#include<list>

#include"makerstring.h"
class Thread {
	friend class BuilderThread;
private:
	DWORD64 pid = 0;
	DWORD64 tid = 0;
	LPTHREAD_START_ROUTINE startAddress = 0;
public:

	inline static Thread findInListByPidAndTid(std::list<Thread>& list, DWORD64 pid, DWORD64 tid) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).pid == pid && (*l).tid == tid) {
				return *l;
			}
		}
		return Thread();
	}

	inline static void addInList(std::list<Thread>& list, const Thread& point) {
		list.push_back(point);
	}
	inline static bool deleteInListByPidAndTid(std::list<Thread>& list, DWORD64 pid, DWORD64 tid) {
		for (auto l = list.begin(); l != list.end(); l++) {
			if ((*l).pid == pid && (*l).tid==tid) {
				list.erase(l);
				return true;
			}
		}
		return false;
	}

	std::string getInfo() {
		return "PID: " + std::to_string(pid) + " TID: " + std::to_string(tid)
			+ " ADDR: " + MakerString::MakeHexNum((DWORD64)startAddress) + "\n";
	}
};


class BuilderThread {
private:
	Thread thread;
public:
	BuilderThread pid(DWORD64 pid) {
		thread.pid = pid;
		return *this;
	}
	BuilderThread tid(DWORD64 tid) {
		thread.tid = tid;
		return *this;
	}
	BuilderThread addr(LPTHREAD_START_ROUTINE addr) {
		thread.startAddress = addr;
		return *this;
	}
	Thread build() {
		return thread;
	}
};

#endif