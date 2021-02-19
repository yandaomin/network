#ifndef ASYNC_H
#define ASYNC_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include "common.h"

class Loop;
class AsyncPrivate;
class NETWORK_EXPORT Async
{
public:
    Async(Loop* loop);
    void init();
    virtual ~Async();
    void run(ActionCallback callback);
    void close(AsyncOnCloseCallBack callback);
	Loop* loop();
private:
	AsyncPrivate* private_;
};

#endif
