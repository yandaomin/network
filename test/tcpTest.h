#ifndef TCPTEST_H
#define TCPTEST_H
#include "tcpClient.h"
#include <iostream>
#include "timeManager.h"
#include "tcpServer.h"
#include "tcpConnection.h"
#include "toolkit.h"
#include "timer.h"

void testServer() {
	std::atomic<uint64_t> dataSize(0);
	Loop* loop = Loop::defaultLoop();
	TcpServer server(loop);
	server.setAddr("0.0.0.0");
	server.setPort(10002);
	//server.setPort(10005);
	server.setOnDataCallback([&dataSize](TcpConnectionPtr ptr, const char* data, long size)
	{
		std::string str(data, size);
		while (true) {
			int pos = str.find("\r\n");
			if (pos == std::string::npos) {
				break;;
			}
			dataSize++;
			str = str.substr(pos + 2);
		}

		//dataSize ++;
		char* buf = new char[size];
		memcpy(buf, data, size);
		ptr->writeInLoop(buf, size, [&](WriteResult& info)
		{
			//write message error.
			if (0 != info.status)
			{
				std::cout << "Write error: " << Loop::getErrorMessage(info.status) << std::endl;
			}
			delete[] info.buf;
		});
		return;
		ptr->write(data, size, nullptr);
	});

	//ÐÄÌø³¬Ê±
	server.setTimeout(15);

	server.start();

	Timer timer1(loop, 1000, 1000, [&server](Timer* ptr)
	{
		std::string msg = "pingpong\r\n";
		std::map<std::string, TcpConnectionPtr> sockets = server.sockets();
		std::cout << "sockets.size() = " << sockets.size() << std::endl;
		for (auto kv : sockets) {
			if (kv.second->isConnected())
				kv.second->write(msg.data(), msg.length(), nullptr);
		}
		//std::cout << "cnt = " << dataSize << std::endl;
		//std::cout << "send data:" << (dataSize >> 10) << " kbyte/s" << std::endl;
		//dataSize = 0;
	});
	timer1.start();


	Timer timer(loop, 1000, 1000, [&dataSize, &server](Timer* ptr)
	{
		std::cout << "cnt = " << dataSize << std::endl;
		std::cout << "clientCount = " << server.getClientCount() << std::endl;
		//std::cout << "send data:" << (dataSize >> 10) << " kbyte/s" << std::endl;

		dataSize = 0;

	});

	timer.start();

	loop->run();
}
void testClients() {
	Loop* loop = Loop::defaultLoop();
	std::vector<TcpClient*> sockets;
	for (int i = 0; i < 1000; i++) {
		TcpClient* client = new TcpClient(loop);
		//client->setSockets(&sockets);
		//client->setLoop(loop);
		//client->connect("127.0.0.1", 10002);
		client->connect("192.168.233.130", 10002);
		sockets.push_back(client);
	}
	Timer timer(loop, 1000, 1000, [&sockets](Timer* tim)
	{
		std::string msg = "pingpong-abcdefghijklmnopqrstuvwxyz\r\n";
		for (auto socket : sockets)
		{
			if (socket->getConnection())
				socket->getConnection()->write(msg.data(), msg.length());
		}
	});
	timer.start();
	loop->run();

	delete loop;
}

void testTcp() {
	std::string ff;
	std::cout << ff.size() << "," << ff.capacity() << std::endl;
	ff.resize(3000);
	std::cout << ff.size() << "," << ff.capacity() << std::endl;
	//testClients();
	testServer();
}
#endif