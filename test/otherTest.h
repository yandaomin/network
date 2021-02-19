#ifndef OTHERTEST_H
#define OTHERTEST_H
#include <iostream>
#include <cstdlib>
#include <memory>
#include <list>
#include <map>
#include "timeManager.h"

class Test :public std::enable_shared_from_this<Test> {
public:
	std::shared_ptr<Test> getObj() {
		return shared_from_this();
	}
	int ss = 993;
};
void testShareFromThis() {
	std::shared_ptr<Test> tt;
	{
		Test* test = new Test;
		tt = test->getObj();
		test->ss = 9988;
	}
	tt->ss = 88;

	// 	std::shared_ptr<Test> tt;
	// 	{
	// 		std::shared_ptr<Test> test = std::make_shared<Test>();
	// 		tt = test->getObj();
	// 		test->ss = 9988;
	// 	}
	// 	tt->ss = 88;
}
void testMap() {
	std::map<std::string, std::string> testMap;
	for (int i = 0; i < 50000; i++) {
		std::string key = "helloworldabcdefghijklmn:" + toolkit::int2str(i);
		testMap[key] = key;
	}
	std::cout << "map size:" << testMap.size() << std::endl;

	unsigned seed;
	std::cout << "请输入一个整数";
	std::cin >> seed;
	//seed = 50000;
	srand(seed);
	// 	for (int i = 0; i < 100; i++) {
	// 		std::cout << rand() << " ";
	// 	}
	TimeManager::instance().start();
	for (int i = 0; i < 100000; i++) {
		std::string fk = "helloworldabcdefghijklmn:" + toolkit::int2str(rand());
		auto rst = testMap.find(fk);
		if (rst == testMap.end()) {
			std::cout << "not found" << std::endl;
		}
	}
	time_t time = TimeManager::instance().getTime();
	std::cout << "elapse time: " << time << std::endl;
}
#endif