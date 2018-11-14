// 网络聊天_服务器端.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#define MAXCLIENT 3//允许的最大客户端数目
using namespace std;

char recvBuf[100];//接受缓冲区
char sendBuf[100];//发送缓冲区

struct SA
{
	SOCKET socket;//客户端套接字
	sockaddr_in addr;//客户端地址（协议、端口号、ip地址）
};

int iClient = 0;
SA saCli[MAXCLIENT];

//接收客户端发来的数据的进程
void recvProc(SA* sa)
{
	ZeroMemory(recvBuf, 100);
	ZeroMemory(sendBuf, 100);
	while (true)
	{
		if (SOCKET_ERROR == recv(sa->socket, recvBuf, sizeof(recvBuf), 0))//接受客户端发来的数据失败
		{
			cout << inet_ntoa(sa->addr.sin_addr) << "已断开连接" << endl;//只在服务器端显示
			return;
		}
		if (recvBuf[0] != 0)//接受客户端发来的数据成功且数据不为空
		{
			sprintf_s(sendBuf, "[群聊]<%s>说：%s", inet_ntoa(sa->addr.sin_addr), recvBuf);
			cout << sendBuf << endl;//将该客户端发来的信息打印到服务器端的控制台
			for (int i = 0; i <= iClient; i++)
			{
				send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//将该客户端发来的数据发送给每一个客户端（包括该客户端自身）
			}
		}
	}
	closesocket(sa->socket);
}

//服务器接收键盘输入信息的线程
void ProcessGetKeyIn()
{
	while (true)
	{
		char temp[90];
		ZeroMemory(temp, 90);
		ZeroMemory(sendBuf, 100);
		cin >> temp;//服务器端输入数据
		sprintf_s(sendBuf, "服务器说：%s", temp);
		cout << sendBuf << endl;

		for (int i = 0; i < iClient; i++) {
			send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//数据发送给每一个客户端
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	HANDLE threads[MAXCLIENT + 1];//+1的含义：服务器接收键盘输入信息的线程

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//启动Winsock库
	{
		return -1;
	}
	if (HIBYTE(wsaData.wVersion != 2) || LOBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}
	SOCKET socketSer = socket(AF_INET, SOCK_STREAM, 0);//创建服务器流套接字
	if (INVALID_SOCKET == socketSer)
	{
		closesocket(socketSer);
		WSACleanup();
		return -1;
	}

	sockaddr_in addrSer;//服务器地址（协议、端口号、ip）
	addrSer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ip地址取为INADDR_ANY，以允许服务器应用监听主机上每个网络接口的客户机活动
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(8888);//端口号设置为8888
								   /*
								   调用bind函数的目的：将本地地址绑定到所创建的套接字上以便在网络上标识该套接字(即将套接字与地址关联起来)。

								   该函数的函数原型：int bind(SOCKET s,const struct sockaddr FAR *name,int nameLen);

								   s:标识一个未捆绑的套接字的句柄

								   name:赋予套接字的地址

								   nemeLen:标识name参数的长度

								   返回值：若函数成功调用则返回0,否则返回错误信息.
								   */
	bind(socketSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));

	/*
	调用listen函数的目的：让一个套接字等待进入连接。

	该函数的函数原型：int listen(SOCKET s,int backlog);

	s:标识一个已捆绑未连接的套接字的句柄，用其来等待客户机的连接

	backlog:用于指定正在等待连接的最大队列的长度

	返回值：若函数成功调用则返回0,否则返回错误信息.
	*/
	listen(socketSer, MAXCLIENT);
	cout << "服务器已启动" << endl;

	while (true)
	{
		sockaddr_in addrCli;//客户端地址
		int len = sizeof(SOCKADDR);//sockaddr_in结构的长度

								   /*
								   调用accept函数的目的：使套接字做好接受客户连接的准备。

								   该函数的函数原型：SOCKET accept(SOCKET s,struct sockadddr FAR *addr,int FAR * addrlen);

								   s:处于监听模式的套接字

								   addr:该函数返回后，该参数会包含发出连接请求的那个客户机的地址信息（协议、端口号、ip地址）

								   addrlen:sockaddr_in结构的长度

								   返回值：SOCKET:一个新的套接字句柄，对应于那个客户机连接，对于该客户机后续的所有操作都是使用该套接字
								   */
		SOCKET socketCon = accept(socketSer, (SOCKADDR*)&addrCli, &len);

		if (iClient < MAXCLIENT) {
			saCli[iClient].socket = socketCon;
			saCli[iClient].addr = addrCli;

			ZeroMemory(sendBuf, 100);//清空发送缓冲区
									 /*
									 函数原型：
									 int sprintf_s(
									 char *buffer,
									 size_t sizeOfBuffer,
									 const char *format [,
									 argument] ...
									 );
									 sprintf_s()是sprintf()的安全版本，通过指定缓冲区长度来避免sprintf()存在的溢出风险
									 */
			sprintf_s(sendBuf, "%s 已连接", inet_ntoa(addrCli.sin_addr));
			cout << sendBuf << endl;//将该客户端已连接的消息打印到服务器端的控制台

			for (int i = 0; i <= iClient; i++) {
				send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//将该客户端已连接的消息发送给每一个客户端（包括该客户端自身）
			}

			if (iClient == 0) {
				threads[MAXCLIENT] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessGetKeyIn, NULL, 0, NULL); //服务器接收键盘输入信息的线程
				threads[iClient] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvProc, (void*)&saCli[iClient], 0, NULL);//创建一个线程来接收客户端发来的数据
			}
			else {
				threads[iClient] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvProc, (void*)&saCli[iClient], 0, NULL);//创建一个线程来接收客户端发来的数据
			}

			iClient++;
		}
		else {
			closesocket(socketCon);//超过最大客户机数目，则关闭该套接字
								   //释放资源
			break;
		}
	}

	for (int i = 0; i<MAXCLIENT + 1; i++)
	{
		CloseHandle(threads[i]); //清理线程资源
	}

	WSACleanup();//终结Winsock库
	system("pause");
	return 0;
}
/*作者：huangxiang360729
来源：CSDN
原文：https ://blog.csdn.net/huangxiang360729/article/details/51038954 
版权声明：本文为博主原创文章，转载请附上博文链接！*/