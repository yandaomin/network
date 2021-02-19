
#include "asyncPrivate.h"
#include "ref.h"
#include "async.h"

AsyncPrivate::AsyncPrivate(Loop * loop)
	:loop_(loop),
	 handle_(nullptr),
	 onCloseCB_(nullptr)
{

}

void AsyncPrivate::init() {
	if (nullptr == handle_) {
		handle_ = new uv_async_t;
		::uv_async_init((uv_loop_t*)(loop_->handle()), handle_, AsyncPrivate::doProcess);
		handle_->data = static_cast<void*>(this);
		::uv_async_send(handle_);
	}
}

AsyncPrivate::~AsyncPrivate() {

}

void AsyncPrivate::run(ActionCallback callback) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		actions_.push(callback);
	}
	if (handle_ != nullptr)
		::uv_async_send(handle_);
}

void AsyncPrivate::process() {
	std::queue<ActionCallback> actions;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		actions_.swap(actions);
	}
	while (!actions.empty()) {
		auto func = actions.front();
		func();
		actions.pop();
	}
}

void AsyncPrivate::close(AsyncOnCloseCallBack callback) {
	onCloseCB_ = callback;
	if (::uv_is_closing((uv_handle_t*)handle_) == 0) {
		::uv_close((uv_handle_t*)handle_, [](uv_handle_t* handle) {
			auto ptr = static_cast<AsyncPrivate*>(handle->data);
			ptr->onClose();
			delete (uv_async_t*)handle;
		});
	}
}

Loop* AsyncPrivate::loop() {
	return loop_;
}

void AsyncPrivate::setParent(void * parent)
{
	parent_ = parent;
}

void * AsyncPrivate::getParent()
{
	return parent_;
}

void AsyncPrivate::doProcess(uv_async_t* handle) {
	auto async = static_cast<AsyncPrivate*>(handle->data);
	async->process();
}

void AsyncPrivate::onClose(){
	if (nullptr != onCloseCB_) {
		onCloseCB_((Async*)this->getParent());
	}
}
