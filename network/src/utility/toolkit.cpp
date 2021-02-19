#include "toolkit.h"
#include <sstream>

TOOLKIT_NAMESPACE_BEGIN

std::string int2str(int val) {
	std::ostringstream out;
	out << val;
	return out.str();
}

int str2int(const std::string & val) {
	std::istringstream in(val.c_str());
	int ret = 0;
	in >> ret;
	return ret;
}

TOOLKIT_NAMESPACE_END