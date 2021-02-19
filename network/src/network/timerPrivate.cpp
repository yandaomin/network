#include "timerPrivate.h"
#include "timer.h"

//timeout 这个参数指的是，在 uv_run() 之后多久启动定时器，单位是毫秒
//repeat 这个参数是指定，定时器循环定时的时间，也就是重复调用的时间，单位是毫秒，为0时只执行一次
TimerPrivate::TimerPrivate(Loop * loop, uint64_t timeout, uint64_t repeat, OnTimerCallback callback)
    :isStarted_(false),
    handle_(new uv_timer_t),
    timeout_(timeout),
    repeat_(repeat),
    onTimeCallback_(callback),
    onCloseCallback_(nullptr)
{
    handle_->data = static_cast<void*>(this);
    ::uv_timer_init((uv_loop_t*)(loop->handle()), handle_);
}

TimerPrivate::~TimerPrivate() {
}

void TimerPrivate::start() {
    if (!isStarted_) {
        isStarted_ = true;
        ::uv_timer_start(handle_, TimerPrivate::doOnTime, timeout_, repeat_);
    }
}

void TimerPrivate::close(TimerOnCloseCallback callback) {
    onCloseCallback_ = callback;
    if (uv_is_active((uv_handle_t*)handle_)) {
        uv_timer_stop(handle_);
    }
    if (uv_is_closing((uv_handle_t*)handle_) == 0) {
        ::uv_close((uv_handle_t*)handle_,
            [](uv_handle_t* handle)
		{
			auto ptr = static_cast<TimerPrivate*>(handle->data);
			ptr->doClose();
			delete handle;
		});
    } else {
        doClose();
    }
}

void TimerPrivate::setTimerRepeat(uint64_t ms) {
    repeat_ = ms;
    ::uv_timer_set_repeat(handle_, ms);
}

void TimerPrivate::onTime() {
    if (onTimeCallback_) {
        onTimeCallback_((Timer*)getParent());
    }
}

void TimerPrivate::doClose() {
    if (onCloseCallback_)
        onCloseCallback_((Timer*)getParent());
}

void TimerPrivate::doOnTime(uv_timer_t * handle) {
    auto ptr = static_cast<TimerPrivate*>(handle->data);
    ptr->onTime();
}
