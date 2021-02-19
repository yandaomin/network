#ifndef REF_H
#define REF_H
#include <memory>
#include "uv.h"

#define PARENT_DECLARE \
public: \
	void setParent(void* parent){\
		parent_ = parent;\
	}\
	void* getParent(){\
		return parent_;\
	}\
private:\
	void* parent_ = nullptr;

using UVTcpPtr = std::shared_ptr<uv_tcp_t>;




#endif