#ifndef LOOP_PRIVATE_H
#define LOOP_PRIVATE_H

#include <uv.h>
#include <thread>
#include <atomic>
#include "common.h"
#include "ref.h"

class Async;
class LoopPrivate
{
	PARENT_DECLARE
public:
	//LoopPrivate();
	LoopPrivate(bool isDefault);
	~LoopPrivate();
	int run();
	int runNoWait();
	int stop();
	bool isRunning();
	bool isSameThread();
	void runInLoop(const ActionCallback func);
	uv_loop_t* handle();
	bool isDefaultLoop();
	void init();
public:
	static std::string getErrorMessage(int status);
private:
	
	//static LoopPrivate* defaultLoop();
// private:
// 	LoopPrivate(bool isDefault);
private:
	std::thread::id threadId_;
	uv_loop_t* loop_ = nullptr;
	Async* async_ = nullptr;
	std::atomic<bool> isRunning_;
};

#endif