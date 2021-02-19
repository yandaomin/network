#ifndef TOOLKIT_H
#define TOOLKIT_H
#define TOOLKIT_NAMESPACE_BEGIN namespace toolkit{
#define TOOLKIT_NAMESPACE_END }
#include "export_network.h"
#include <string>

TOOLKIT_NAMESPACE_BEGIN
	std::string NETWORK_EXPORT int2str(int val);
	int NETWORK_EXPORT str2int(const std::string &val);
TOOLKIT_NAMESPACE_END
#endif