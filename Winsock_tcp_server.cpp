// ��������_��������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <string>

#pragma comment(lib,"ws2_32.lib")

#define MAXCLIENT 40//��������ͻ�����Ŀ
using namespace std;

char recvBuf[1000];//���ܻ�����
char sendBuf[1000];//���ͻ�����
struct SA
{
	SOCKET socket;//�ͻ����׽���
	sockaddr_in addr;//�ͻ��˵�ַ��Э�顢�˿ںš�ip��ַ��
	int room_number;//��ŵ�ǰ�ͻ��˽����������
};
//vector<SA> cli_list;//���ڴ�ſͻ��˵��׽��ֺ͵�ַ�ı�

int iClient = 0;
SA saCli[MAXCLIENT];

//���տͻ��˷��������ݵĽ���
void recvProc(SA* sa)
{
	while (true)
	{
		ZeroMemory(recvBuf, 1000);
		ZeroMemory(sendBuf, 1000);
		if (SOCKET_ERROR == recv(sa->socket, recvBuf,  sizeof(recvBuf), 0))//���ܿͻ��˷���������ʧ��
		{
			cout << inet_ntoa(sa->addr.sin_addr) << "�ѶϿ�����" << endl;//ֻ�ڷ���������ʾ
			return;
		}
		if (recvBuf[0] != 0)//���ܿͻ��˷��������ݳɹ������ݲ�Ϊ��
		{
			string s = recvBuf;
			cout << s << endl;
			//if (flag_first) {
			//	saCli[iClient].room_number = atoi(recvBuf);//��ӵ�ǰ�ͻ���ѡ���������
			//	sa->room_number = atoi(recvBuf);
			//	flag_first = false;
			//	sprintf_s(sendBuf, "�û�<%s>����������%s", inet_ntoa(sa->addr.sin_addr), recvBuf);
			//}
			//else
			//	sprintf_s(sendBuf, "[�û�]<%s>˵��%s", inet_ntoa(sa->addr.sin_addr), recvBuf);
			//cout << sendBuf << endl;//���ÿͻ��˷�������Ϣ��ӡ���������˵Ŀ���̨
			//for (int i = 0; i < iClient; ++i) {
			//	//����Ϣ���͵�ͬһ����������ͻ��ˣ�������⣩
			//	if (saCli[i].room_number == sa->room_number && (saCli[i].addr.sin_port != sa->addr.sin_port 
			//		|| inet_ntoa(saCli[i].addr.sin_addr) != inet_ntoa(sa->addr.sin_addr))) {
			//		send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);
			//	}
			//}
			//else {
			//	for (int i = 0; i <= iClient; i++)
			//		send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//���˿ͻ��˷��������ݷ��͸�ÿһ���ͻ��ˣ������ÿͻ�������
			//}
		}
	}
	closesocket(sa->socket);
}

//���������ռ���������Ϣ���߳�
void ProcessGetKeyIn()
{
	while (true)
	{
		ZeroMemory(sendBuf, 1000);
		scanf("%[^\n]", &sendBuf);//����������������
		cout << "������˵��"<<sendBuf << endl;
		for (int i = 0; i < iClient; i++) {
			send(saCli[i].socket, sendBuf, sizeof(sendBuf), 0);//���ݷ��͸�ÿһ���ͻ���
		}
	}
}

//wchar_t ToUnicode(char * szSource, int nEncoding) {
//
//}

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
	// 0.0.0.0  <=>  127.0.0.1  |  192.168.  | 49
	// 
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
			ZeroMemory(sendBuf, 1000);//��շ��ͻ�����
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
			sprintf_s(sendBuf, "#0$%s$Connected#", inet_ntoa(addrCli.sin_addr));
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