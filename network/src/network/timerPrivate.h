#ifndef TIMER_PRIVATE_HPP
#define TIMER_PRIVATE_HPP

#include <functional>
#include <mutex>
#include "loop.h"
#include "common.h"
#include "ref.h"

class TimerPrivate
{
	PARENT_DECLARE
public:
	TimerPrivate(Loop* loop, uint64_t timeout, uint64_t repeat, OnTimerCallback callback);
    virtual ~TimerPrivate();

    void start();
    void close(TimerOnCloseCallback callback);
    void setTimerRepeat(uint64_t ms);
private:
    void onTime();
    void doClose();
    static void doOnTime(uv_timer_t* handle);

private:
    std::atomic<bool> isStarted_;
    uv_timer_t* handle_;
    uint64_t timeout_;
    uint64_t repeat_;

    OnTimerCallback onTimeCallback_;
    TimerOnCloseCallback onCloseCallback_;
	Loop* loop_ = nullptr;
};

#endif
