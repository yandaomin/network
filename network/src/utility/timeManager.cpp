#include "timeManager.h"
#include <time.h>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

TimeManager::TimeManager() {
	start();
}


TimeManager::~TimeManager() {
}

TimeManager& TimeManager::instance()
{
	static TimeManager manager;
	return manager;
}

void TimeManager::start() {
	ftime(&beginTime_);
	//mBeginTime = time(0);
	//localtime(&mBeginTime);
	//mBeginTime = clock();
}

//获取毫秒
time_t TimeManager::getTime() {
	timeb now;
	time_t rlt;
	ftime(&now);
	rlt = (now.time - beginTime_.time) * 1000 + (now.millitm - beginTime_.millitm);
	return rlt;
}

time_t TimeManager::getTimeSeconds() {
	time_t rlt;
	time(&rlt);
	rlt = rlt - 100 * 24 * 3600;
	return rlt;
}

std::string TimeManager::getDate() {
	// 获取本地时间
	auto now = std::chrono::system_clock::now();
	auto ticks = std::chrono::system_clock::to_time_t(now);
	auto local_time = std::localtime(&ticks);

	// 格式化日期时间
	std::ostringstream oss;

	oss << std::put_time(local_time, "%F");
	oss << std::put_time(local_time, " %T");
	std::string rlt = oss.str();

	return rlt;
}