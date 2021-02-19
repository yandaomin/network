#ifndef ASYNC_PRIVATE_H
#define ASYNC_PRIVATE_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>

#include "loop.h"
#include "ref.h"

class AsyncPrivate
{
public:
	AsyncPrivate(Loop* loop);
    void init();
    virtual ~AsyncPrivate();
    void run(ActionCallback callback);
    void close(AsyncOnCloseCallBack callback);
	Loop* loop();
public:
	void setParent(void* parent);
	void* getParent();
private:
	void process();
	static void doProcess(uv_async_t* handle);
	void onClose();
private:
	Loop* loop_;
    std::mutex mutex_;
    uv_async_t* handle_;
    std::queue<ActionCallback> actions_;
    AsyncOnCloseCallBack onCloseCB_;
	void* parent_ = nullptr;
};

#endif
