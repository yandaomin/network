#include "loopPrivate.h"
#include "async.h"
#include "loop.h"

// LoopPrivate::LoopPrivate()
// 	: LoopPrivate(false)
// {
// 	isRunning_ = false;
// }

LoopPrivate::LoopPrivate(bool isDefault) {
	isRunning_ = false;
	if (isDefault) {
		loop_ = uv_default_loop();
	}
	else {
		loop_ = new uv_loop_t();
		::uv_loop_init(loop_);
	}
}

LoopPrivate::~LoopPrivate() {
	if (!isDefaultLoop()) {
		uv_loop_close(loop_);
		delete async_;
		delete loop_;
	}
}

// LoopPrivate* LoopPrivate::defaultLoop() {
// 	static LoopPrivate defaultLoop(true);
// 	return &defaultLoop;
// }

uv_loop_t* LoopPrivate::handle() {
	return loop_;
}

bool LoopPrivate::isDefaultLoop() {
	return (loop_ == uv_default_loop());
}

void LoopPrivate::init()
{
	async_ = new Async((Loop*)parent_);
}

int LoopPrivate::run() {
	if (!isRunning_) {
		async_->init();
		threadId_ = std::this_thread::get_id();
		isRunning_ = true;
		auto rlt = ::uv_run(loop_, UV_RUN_DEFAULT);
		isRunning_ = false;
		return rlt;
	}
	return -1;
}

int LoopPrivate::runNoWait(){
	if (!isRunning_) {
		async_->init();
		threadId_ = std::this_thread::get_id();
		isRunning_ = true;
		auto rst = ::uv_run(loop_, UV_RUN_NOWAIT);
		isRunning_ = false;
		return rst;
	}
	return -1;
}

int LoopPrivate::stop() {
	if (isRunning_){
		async_->close([](Async* ptr)
		{
			::uv_stop((uv_loop_t*)(ptr->loop()->handle()));
		});
		return 0;
	}
	return -1;
}

bool LoopPrivate::isRunning() {
	return isRunning_;
}

bool LoopPrivate::isSameThread() {
	return std::this_thread::get_id() == threadId_;
}

void LoopPrivate::runInLoop(const ActionCallback func) {
	if (nullptr == func)
		return;

	if (isSameThread() || !isRunning()) {
		func();
		return;
	}
	async_->run(func);
}

std::string LoopPrivate::getErrorMessage(int status) {
	if (WriteResult::result_disconnected == status)
		return "the connection is closed";

	const char* msg = uv_strerror(status);
	std::string errMsg(msg);
	//delete[] msg;
	return errMsg;
}