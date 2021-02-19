#ifndef LOOP_H
#define LOOP_H

#include <thread>
#include <atomic>
#include "common.h"
#include "async.h"

class LoopPrivate;
class NETWORK_EXPORT Loop
{
public:
	Loop();
	~Loop();
	int run();
	int runNoWait();
	int stop();
	bool isRunning();
	bool isSameThread();
	void runInLoop(const ActionCallback func);
	void* handle();
	bool isDefaultLoop();
private:
	Loop(bool isDefault);
public:
	static std::string getErrorMessage(int status);
	static Loop* defaultLoop();
private:
	LoopPrivate* private_;
};

#endif