// ��������_��������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#define MAXCLIENT 3//��������ͻ�����Ŀ
using namespace std;

char recvBuf[100];//���ܻ�����
char sendBuf[100];//���ͻ�����

struct SA
{
	SOCKET socket;//�ͻ����׽���
	sockaddr_in addr;//�ͻ��˵�ַ��Э�顢�˿ںš�ip��ַ��
};

int iClient = 0;
SA saCli[MAXCLIENT];

//���տͻ��˷��������ݵĽ���
void recvProc(SA* sa)
{
	ZeroMemory(recvBuf, 100);
	ZeroMemory(sendBuf, 100);
	while (true)
	{
		if (SOCKET_ERROR == recv(sa->socket, recvBuf, sizeof(recvBuf), 0))//���ܿͻ��˷���������ʧ��
		{
			cout << inet_ntoa(sa->addr.sin_addr) << "�ѶϿ�����" << endl;//ֻ�ڷ���������ʾ
			return;
		}
		if (recvBuf[0] != 0)//���ܿͻ��˷��������ݳɹ������ݲ�Ϊ��
		{
			sprintf_s(sendBuf, "[Ⱥ��]<%s>˵��%s", inet_ntoa(sa->addr.sin_addr), recvBuf);
			cout << sendBuf << endl;//���ÿͻ��˷�������Ϣ��ӡ���������˵Ŀ���̨
			for (int i = 0; i <= iClient; i++)
			{
				send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//���ÿͻ��˷��������ݷ��͸�ÿһ���ͻ��ˣ������ÿͻ�������
			}
		}
	}
	closesocket(sa->socket);
}

//���������ռ���������Ϣ���߳�
void ProcessGetKeyIn()
{
	while (true)
	{
		char temp[90];
		ZeroMemory(temp, 90);
		ZeroMemory(sendBuf, 100);
		cin >> temp;//����������������
		sprintf_s(sendBuf, "������˵��%s", temp);
		cout << sendBuf << endl;

		for (int i = 0; i < iClient; i++) {
			send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//���ݷ��͸�ÿһ���ͻ���
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	HANDLE threads[MAXCLIENT + 1];//+1�ĺ��壺���������ռ���������Ϣ���߳�

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//����Winsock��
	{
		return -1;
	}
	if (HIBYTE(wsaData.wVersion != 2) || LOBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}
	SOCKET socketSer = socket(AF_INET, SOCK_STREAM, 0);//�������������׽���
	if (INVALID_SOCKET == socketSer)
	{
		closesocket(socketSer);
		WSACleanup();
		return -1;
	}

	sockaddr_in addrSer;//��������ַ��Э�顢�˿ںš�ip��
	addrSer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ip��ַȡΪINADDR_ANY�������������Ӧ�ü���������ÿ������ӿڵĿͻ����
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(8888);//�˿ں�����Ϊ8888
								   /*
								   ����bind������Ŀ�ģ������ص�ַ�󶨵����������׽������Ա��������ϱ�ʶ���׽���(�����׽������ַ��������)��

								   �ú����ĺ���ԭ�ͣ�int bind(SOCKET s,const struct sockaddr FAR *name,int nameLen);

								   s:��ʶһ��δ������׽��ֵľ��

								   name:�����׽��ֵĵ�ַ

								   nemeLen:��ʶname�����ĳ���

								   ����ֵ���������ɹ������򷵻�0,���򷵻ش�����Ϣ.
								   */
	bind(socketSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));

	/*
	����listen������Ŀ�ģ���һ���׽��ֵȴ��������ӡ�

	�ú����ĺ���ԭ�ͣ�int listen(SOCKET s,int backlog);

	s:��ʶһ��������δ���ӵ��׽��ֵľ�����������ȴ��ͻ���������

	backlog:����ָ�����ڵȴ����ӵ������еĳ���

	����ֵ���������ɹ������򷵻�0,���򷵻ش�����Ϣ.
	*/
	listen(socketSer, MAXCLIENT);
	cout << "������������" << endl;

	while (true)
	{
		sockaddr_in addrCli;//�ͻ��˵�ַ
		int len = sizeof(SOCKADDR);//sockaddr_in�ṹ�ĳ���

								   /*
								   ����accept������Ŀ�ģ�ʹ�׽������ý��ܿͻ����ӵ�׼����

								   �ú����ĺ���ԭ�ͣ�SOCKET accept(SOCKET s,struct sockadddr FAR *addr,int FAR * addrlen);

								   s:���ڼ���ģʽ���׽���

								   addr:�ú������غ󣬸ò����������������������Ǹ��ͻ����ĵ�ַ��Ϣ��Э�顢�˿ںš�ip��ַ��

								   addrlen:sockaddr_in�ṹ�ĳ���

								   ����ֵ��SOCKET:һ���µ��׽��־������Ӧ���Ǹ��ͻ������ӣ����ڸÿͻ������������в�������ʹ�ø��׽���
								   */
		SOCKET socketCon = accept(socketSer, (SOCKADDR*)&addrCli, &len);

		if (iClient < MAXCLIENT) {
			saCli[iClient].socket = socketCon;
			saCli[iClient].addr = addrCli;

			ZeroMemory(sendBuf, 100);//��շ��ͻ�����
									 /*
									 ����ԭ�ͣ�
									 int sprintf_s(
									 char *buffer,
									 size_t sizeOfBuffer,
									 const char *format [,
									 argument] ...
									 );
									 sprintf_s()��sprintf()�İ�ȫ�汾��ͨ��ָ������������������sprintf()���ڵ��������
									 */
			sprintf_s(sendBuf, "%s ������", inet_ntoa(addrCli.sin_addr));
			cout << sendBuf << endl;//���ÿͻ��������ӵ���Ϣ��ӡ���������˵Ŀ���̨

			for (int i = 0; i <= iClient; i++) {
				send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//���ÿͻ��������ӵ���Ϣ���͸�ÿһ���ͻ��ˣ������ÿͻ�������
			}

			if (iClient == 0) {
				threads[MAXCLIENT] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessGetKeyIn, NULL, 0, NULL); //���������ռ���������Ϣ���߳�
				threads[iClient] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvProc, (void*)&saCli[iClient], 0, NULL);//����һ���߳������տͻ��˷���������
			}
			else {
				threads[iClient] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvProc, (void*)&saCli[iClient], 0, NULL);//����һ���߳������տͻ��˷���������
			}

			iClient++;
		}
		else {
			closesocket(socketCon);//�������ͻ�����Ŀ����رո��׽���
								   //�ͷ���Դ
			break;
		}
	}

	for (int i = 0; i<MAXCLIENT + 1; i++)
	{
		CloseHandle(threads[i]); //�����߳���Դ
	}

	WSACleanup();//�ս�Winsock��
	system("pause");
	return 0;
}
/*���ߣ�huangxiang360729
��Դ��CSDN
ԭ�ģ�https ://blog.csdn.net/huangxiang360729/article/details/51038954 
��Ȩ����������Ϊ����ԭ�����£�ת���븽�ϲ������ӣ�*/