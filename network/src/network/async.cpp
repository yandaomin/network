
#include "async.h"
#include "ref.h"
#include "asyncPrivate.h"
#include "loop.h"

Async::Async(Loop * loop)
	:private_(new AsyncPrivate(loop))
{
	private_->setParent(this);
}

void Async::init() {
	private_->init();
}

Async::~Async() {
	if (private_) {
		delete private_;
		private_ = nullptr;
	}
}

void Async::run(ActionCallback callback) {
	private_->run(callback);
}

void Async::close(AsyncOnCloseCallBack callback) {
	private_->close(callback);
}

Loop* Async::loop() {
	return private_->loop();
}