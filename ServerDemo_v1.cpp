/* �ͻ��˰汾1
   ��ȷ���������޷�ͬʱ�������������������
   ͨ����ʼ��¼��֤ �Լ�ÿ����һ�����ľͶϿ����ӵķ�ʽ�������
   �ؼ����ڷ������˵�¼״̬�ĸ��� �ͳ�ʼsocket�������� */

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>


#include <stdio.h>
#include "iostream"
#include "string"
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void)
{
    WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	char recvBuf[DEFAULT_BUFLEN];

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//��Socket�汾��������
	if (iResult != 0) {
		printf("Socket�汾����: %d\n", iResult);
		return 1;
	}

    cout << "Socket�汾��ʼ�����" << endl;

	ZeroMemory(&hints, sizeof(hints));
	//������
	hints.ai_family = AF_INET;//�޶�Ϊipv4
	hints.ai_socktype = SOCK_STREAM;//�޶�Ϊstreamģʽ����
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//��¼�������ӵ�ip��ַ

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	//�ѷ������˿ڡ���ַ�����ò���д��result hints�൱��һ���н�
	if (iResult != 0) {
		printf("������ʼ������: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	cout << "Socket������ʼ�����" << endl;


    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//��ListenSocket�����ճ�ʼ����socket�ĸ�����Ϣ
	if (ListenSocket == INVALID_SOCKET) {
		printf("������ʼ������: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	cout << "Socket������ʼ�����" << endl;

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);


	if (iResult == SOCKET_ERROR) {
		printf("�󶨹���: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Socket�󶨳�ʼ�����" << endl;

	// freeaddrinfo(result);
    //������ô����Ҳ��ȷ���� 2018.3.17 13:16

    while (1)
    {
        iResult = listen(ListenSocket, SOMAXCONN);

        //ͨ���������ֻ�д��ѭ�� ֱ�������ӽ���Ϊֹ
        if (iResult == SOCKET_ERROR)
        {
            printf("����ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        //ѭ������

        cout << "���ڵȴ�����..." << endl;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
		{
			cout << "����ʧ��" << endl;
			continue;
		}
		cout << "�յ�һ����Ϣ: " << endl;

        if (ClientSocket == INVALID_SOCKET)
        {
            printf("����ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            // WSACleanup();
            return 1;
        }

        //����ط�����Ҫȥ��
	    //closesocket(ListenSocket);

        for (int i = 0; i < DEFAULT_BUFLEN; i++)
			recvBuf[i] = 0;
        //��ս��ճ�

		iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            printf("%s\n", recvBuf);
            int res = send(ClientSocket, "0", iResult, 0);
            if (res == SOCKET_ERROR)
            {
                printf("����ʧ��: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0)
            printf("���Ӽ����ر�n");
        else
        {
            printf("���ӷ�������", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        // cleanup
        closesocket(ClientSocket);
    }
}