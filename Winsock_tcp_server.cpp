
// TCPserver.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#define SERVERIP "192.168.3.23"//IP��ַ
#define SERVERPORT 5050//�˿ں�

int main()
{
	WORD verision = MAKEWORD(2, 2);
	WSADATA lpData;
	int intEr = WSAStartup(verision, &lpData);//ָ��winsock�汾����ʼ��
	if (intEr != 0)
	{
		cout << "winsock init failed!" << endl;
		return 0;
	}
	else
		cout << "winsock init success!" << endl;

	//��������socket
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
	hostAddr.sin_port = htons(SERVERPORT);//ת���������ֽ���
										  //hostAddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);//ת���������ֽ���
										  //cout << "net IP:" << hostAddr.sin_addr.S_un.S_addr << endl;
										  /*
										  inet_addr()�汾̫�ͣ�������ʹ��inet_pton(Э���壬�ַ���IP��ַ��voidĿ��in_addr*)
										  ͷ�ļ���WS2tcpip.h
										  */
	in_addr addr;
	inet_pton(AF_INET, SERVERIP, (void*)&addr);
	hostAddr.sin_addr = addr;
	cout << "ip:" << addr.S_un.S_addr << endl;


	//�����׽���listenSock�󶨱�����ַ��Ϣ
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
		int len = sizeof(struct sockaddr);//����ָ�����ȣ�����ᵼ��accept����10014����

										  //accept��ѭ���ȴ��ͻ������� 
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




