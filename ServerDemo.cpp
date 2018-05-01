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

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

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

	// sockaddr_in temp;
	// temp.sin_family = AF_INET;
	// temp.sin_port = htons(27015);
	// temp.sin_addr.S_un.S_addr = INADDR_ANY;S


								// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	//�ѷ������˿ڡ���ַ�����ò���д��result hints�൱��һ���н�
	if (iResult != 0) {
		printf("������ʼ������: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	cout << "Socket������ʼ�����" << endl;


	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//��ListenSocket�����ճ�ʼ����socket�ĸ�����Ϣ
	if (ListenSocket == INVALID_SOCKET) {
		printf("������ʼ������: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	cout << "Socket������ʼ�����" << endl;
	
	// Setup the TCP listening socket

	//����bind()
	//һ��socket�������е�ַ�� ������ַ �˿ں����
	//socket����ָ��һ����ַ�� bind����ָ����ַ�Ͷ˿ں�
	//���bind����ֻ�����ڻ�û�����ӵ�socket ֻ����connect()��listen()֮ǰ����
	//һ��socketֻ�ܵ���һ��bind
	//������һ��bind֮��socket�Ͳ����ٸ�����
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);


	if (iResult == SOCKET_ERROR) {
		printf("�󶨹���: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Socket�󶨳�ʼ�����" << endl;

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);

	//ͨ���������ֻ�д��ѭ�� ֱ�������ӽ���Ϊֹ
	if (iResult == SOCKET_ERROR) {
		printf("����ʧ��: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	while(1)
	{
		cout << "waiting..." << endl;
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "����ʧ��" << endl;
			continue;
		}
		
		cout << "�յ�һ������: ";
		break;
	}


	// Accept a client socket
	// ClientSocket = accept(ListenSocket, NULL, NULL);

	// cout << "oop  " << iResult << endl;
	
	//�������ӷ���ʱ ����accept �����ǵ��̵߳�
	//һ˵�����ӷ���accept�����ú� ����������Ὺһ�����߳������������
	//������������ż���˳��ִ�л��߼���������������
	if (ClientSocket == INVALID_SOCKET) {
		printf("����ʧ��: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//����ط�����Ҫȥ��
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {
		for (int i = 0; i < DEFAULT_BUFLEN; i++)
			recvbuf[i] = 0;
		//��ս��ճ�
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		string check_string = recvbuf;
		if(check_string == "  ") break;
		if (iResult > 0) {
			printf("%s\n", recvbuf);
			iSendResult = send(ClientSocket, "0", iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("����ʧ��: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}
		else if (iResult == 0)
			printf("���Ӽ����ر�n");
		else {
			printf("���ӷ�������", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		cout << "���ڵȴ�����" << endl;

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}