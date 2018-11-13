
// TCPserver.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#define SERVERIP "192.168.3.23"//IP地址
#define SERVERPORT 5050//端口号

int main()
{
	WORD verision = MAKEWORD(2, 2);
	WSADATA lpData;
	int intEr = WSAStartup(verision, &lpData);//指定winsock版本并初始化
	if (intEr != 0)
	{
		cout << "winsock init failed!" << endl;
		return 0;
	}
	else
		cout << "winsock init success!" << endl;

	//创建侦听socket
	SOCKET listenScok = socket(AF_INET, SOCK_STREAM, 0);
	if (listenScok == INVALID_SOCKET)
	{
		cout << "socket error" << endl;
		return 0;
	}
	else
		cout << "create socket success" << endl;

	sockaddr_in hostAddr;
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(SERVERPORT);//转换成网络字节序
										  //hostAddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);//转换成网络字节序
										  //cout << "net IP:" << hostAddr.sin_addr.S_un.S_addr << endl;
										  /*
										  inet_addr()版本太低，被弃用使用inet_pton(协议族，字符串IP地址，void目标in_addr*)
										  头文件：WS2tcpip.h
										  */
	in_addr addr;
	inet_pton(AF_INET, SERVERIP, (void*)&addr);
	hostAddr.sin_addr = addr;
	cout << "ip:" << addr.S_un.S_addr << endl;


	//侦听套接字listenSock绑定本机地址信息
	int err;
	err = bind(listenScok, (struct sockaddr*)&hostAddr, sizeof(sockaddr));
	if (err != 0)
	{
		cout << "hostAddr bind failed!" << endl;
		return 0;
	}

	err = listen(listenScok, 3);
	if (err != 0)
	{
		cout << "listen socket listen failed!" << endl;
		return 0;
	}
	cout << "listening..." << endl;

	int no = 1;
	while (true)
	{
		sockaddr_in clientAddr;
		int len = sizeof(struct sockaddr);//必须指定长度，否则会导致accept返回10014错误

										  //accept会循环等待客户端连接 
		SOCKET clientSock = accept(listenScok, (struct sockaddr*)&clientAddr, &len);
		if (clientSock == INVALID_SOCKET)
		{
			cout << "accept failed!" << endl;
			cout << WSAGetLastError() << endl;
			system("pause");
			return 0;
		}

		char buf[1024] = "\0";
		int buflen = recv(clientSock, buf, 1024, 0);
		if (buflen == SOCKET_ERROR)
		{
			cout << "recv failed!" << endl;
			return 0;
		}

		cout << "recieve data" << no++ << ": " << buf << endl;

		err = shutdown(clientSock, 2);
		if (err == SOCKET_ERROR)
		{
			cout << "shutdown failed!" << endl;
			return 0;
		}
	}

	err = shutdown(listenScok, 2);
	if (err == SOCKET_ERROR)
	{
		cout << "shutdown failed!" << endl;
		return 0;
	}

	err = closesocket(listenScok);
	if (err == SOCKET_ERROR)
	{
		cout << "closesocket failed!" << endl;
		return 0;
	}

	if (WSACleanup() != 0)
	{
		cout << "WSACleanup failed!" << endl;
		return 0;
	}
	system("pause");
	return 0;
}




