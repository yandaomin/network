#include "timer.h"
#include "timerPrivate.h"

//timeout 这个参数指的是，在 uv_run() 之后多久启动定时器，单位是毫秒
//repeat 这个参数是指定，定时器循环定时的时间，也就是重复调用的时间，单位是毫秒，为0时只执行一次
Timer::Timer(Loop * loop, uint64_t timeout, uint64_t repeat, OnTimerCallback callback) {
	private_ = new TimerPrivate(loop, timeout, repeat, callback);
	private_->setParent(this);
}

Timer::~Timer() {
	if (private_) {
		delete private_;
		private_ = nullptr;
	}
}

void Timer::start() {
	private_->start();
}

void Timer::close(TimerOnCloseCallback callback) {
	private_->close(callback);
}


void Timer::setTimerRepeat(uint64_t ms) {
	private_->setTimerRepeat(ms);
}