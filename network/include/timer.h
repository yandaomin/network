#ifndef TIMER_HPP
#define TIMER_HPP
#include "common.h"
#include <functional>
#include "loop.h"

class TimerPrivate;
class NETWORK_EXPORT Timer
{
public:
    Timer(Loop* loop, uint64_t timeout, uint64_t repeat, OnTimerCallback callback);
    virtual ~Timer();

    void start();
    void close(TimerOnCloseCallback callback);
    void setTimerRepeat(uint64_t ms);
	TimerPrivate* private_ = nullptr;
};

#endif
