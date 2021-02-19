#ifndef UDPTEST_H
#define UDPTEST_H
#include "udpNetwork.h"
#include <iostream>
#include <memory>
#include <list>
#include <string>
#include "timer.h"

std::shared_ptr<UdpNetwork> test_runUdpClient(const std::string& addr, int port) {
	std::cout << "addr = " << addr << ",port = " << port << std::endl;
	Loop* loop = Loop::defaultLoop();
	std::shared_ptr<UdpNetwork> udp = std::make_shared<UdpNetwork>(loop);
	udp->setAddr(addr);
	udp->setPort(port);
	udp->setOnDataCallback([udp](const std::string& ip, int port, const char *buf, long size) {
		udp->write(ip, port, buf, size);
	});
	udp->start();
	return udp;
}
std::atomic<uint64_t> dataSize(0);
std::shared_ptr<UdpNetwork> runUdpServer() {
	Loop* loop = Loop::defaultLoop();
	std::shared_ptr<UdpNetwork> udpServer = std::make_shared<UdpNetwork>(loop);
	int serverPort = 10000;
	udpServer->setAddr("0.0.0.0");
	udpServer->setPort(serverPort);
	udpServer->setOnDataCallback([&](const std::string& ip, int port, const char *buf, long size) {
		std::string str(buf, size);
		while (true) {
			int pos = str.find("\r\n");
			if (pos == std::string::npos) {
				break;;
			}
			dataSize++;
			str = str.substr(pos + 2);
		}
		udpServer->write(ip, port, buf, size);
	});
	udpServer->start();

	return udpServer;
}
void runUdpClients(std::list<std::shared_ptr<UdpNetwork>>& udpSockets) {
	int clientCount = 10000;
	int port = 10001;
	for (int i = 0; i < clientCount; i++) {
		std::cout << "client: " << i << std::endl;
		std::shared_ptr<UdpNetwork> client = test_runUdpClient("0.0.0.0", port + i);
		udpSockets.push_back(client);
		std::string msg = "abcdefghijklmnopqrstuvywxzabcdefghijklmnopqrstuvwxyz\r\n";
		//client->write("127.0.0.1", 10000, msg.data(), msg.length());
		client->write("192.168.233.130", 10000, msg.data(), msg.length());
	}
}
std::shared_ptr<Timer> runTimer() {
	Loop* loop = Loop::defaultLoop();
	std::shared_ptr<Timer> timer = std::make_shared<Timer>(loop, 1000, 1000, [&](Timer* ptr)
	{
		std::cout << "cnt = " << dataSize << std::endl;
		//std::cout << "clientCount = " << udpSockets.size() << std::endl;
		dataSize = 0;
	});
	timer->start();
	return timer;
}

void beginRunUdpServer() {
	//run server
	std::shared_ptr<UdpNetwork> udpServer = runUdpServer();
	std::shared_ptr<Timer> timer = runTimer();
}

void beginRunUdpClient() {
	//run client
	std::list<std::shared_ptr<UdpNetwork>> udpSockets;
	Loop* loop = Loop::defaultLoop();
	runUdpClients(udpSockets);
	loop->run();
}
void testUdp() {
	beginRunUdpClient();
}
#endif