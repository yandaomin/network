
#if defined(_WIN32)
#else
#include "string.h"
#endif // defined

#include "udpTest.h"
#include "tcpTest.h"

int main(int argc, char** args)
{
	//testShareFromThis();
	//testMap();
	testTcp();
	//testUdp();
	return 0;
}