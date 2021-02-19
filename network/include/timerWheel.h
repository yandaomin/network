#ifndef   TIMER_WHEEL_H
#define   TIMER_WHEEL_H

#include <vector>
#include <set>
#include <memory>
#include "timer.h"
#include "loop.h"

//algorithm complexity  o(1).
template<typename Type>
class NETWORK_EXPORT TimerWheel
{
public:
    TimerWheel(Loop* loop);
    TimerWheel(Loop* loop,unsigned int timeout);
    void setTimeout(unsigned int seconds);
    int getTimeout();
    void start();
    void insert(std::shared_ptr<Type> value);
private:
    unsigned int index_;
    unsigned int timeoutSec_;
    Timer timer_;
    std::vector<std::set<std::shared_ptr<Type>>> wheel_;
    void wheelCallback();
};

template<typename Type>
inline TimerWheel<Type>::TimerWheel(Loop* loop)
    :TimerWheel(loop, 0)
{

}

template<typename Type>
inline TimerWheel<Type>::TimerWheel(Loop* loop, unsigned int timeout)
    :index_(0),
    timeoutSec_(timeout),
    timer_(loop, 1000, 1000, std::bind(&TimerWheel::wheelCallback, this))
{

}

template<typename Type>
inline void TimerWheel<Type>::setTimeout(unsigned int seconds) {
    timeoutSec_ = seconds;
}

template<typename Type>
inline void TimerWheel<Type>::start() {
    if (timeoutSec_) {
        wheel_.resize(timeoutSec_);
        timer_.start();
    }
}

template<typename Type>
inline void TimerWheel<Type>::insert(std::shared_ptr<Type> value) {
    if (timeoutSec_ > 0 && nullptr != value) {
        wheel_[index_].insert(value);
    }
}

template<typename Type>
inline int TimerWheel<Type>::getTimeout() {
    return timeoutSec_;
}

template<typename Type>
inline void TimerWheel<Type>::wheelCallback() {
    if (!timeoutSec_)
        return;
    if (++index_ == timeoutSec_) {
        index_ = 0;
    }
    wheel_[index_].clear();
}
#endif
