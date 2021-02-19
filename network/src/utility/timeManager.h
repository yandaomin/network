#pragma once
#if defined(_WIN32)
#include <sys\timeb.h>
#else
#include <sys/timeb.h>
#endif 
#include <time.h>
#include <mutex>
#include <atomic>
#include "export_network.h"

class NETWORK_EXPORT TimeManager
{
public:
	TimeManager();
	~TimeManager();
	static TimeManager& instance();
public:
	void start();
	time_t getTime();
	time_t getTimeSeconds();
	std::string getDate();
private:
	std::mutex mutex_;
	timeb beginTime_;
};

