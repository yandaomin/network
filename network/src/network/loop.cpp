#include "loop.h"
//#include "include/TcpConnection.hpp"
#include "async.h"
#include "loopPrivate.h"

Loop::Loop()
	: Loop(false)
{
	
}

Loop::Loop(bool isDefault)
	: private_(new LoopPrivate(isDefault))
{
	private_->setParent(this);
	private_->init();
}

Loop::~Loop() {
	if (private_) {
		delete private_;
		private_ = nullptr;
	}
}

Loop* Loop::defaultLoop() {
	static Loop defaultLoop(true);
	return &defaultLoop;
}

void* Loop::handle() {
	return private_->handle();
}

bool Loop::isDefaultLoop() {
	return private_->isDefaultLoop();
}

int Loop::run() {
	return private_->run();
}

int Loop::runNoWait(){
	return private_->runNoWait();
}

int Loop::stop() {
	return private_->stop();
}

bool Loop::isRunning() {
	return private_->isRunning();
}

bool Loop::isSameThread() {
	return private_->isSameThread();
}

void Loop::runInLoop(const ActionCallback func) {
	private_->runInLoop(func);
}

std::string Loop::getErrorMessage(int status) {
	return LoopPrivate::getErrorMessage(status);
}