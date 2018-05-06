/* �������˰汾4
    �ļ�����demo
    ����Ҫ�½�һ�ֱ�������
    ��ʱ��ʹ��27015�˿������ļ� */

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>


#include <stdio.h>
#include "iostream"
#include "fstream"
#include "cstring"
#include "string"
using namespace std;

#include "OutOfNetWork.h"
#include "User.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 8188
//��֪�����������Ĵ�С��û����

#define DEFAULT_PORT "27015"
//�ϰ汾�������շ�һ��Ķ˿�
//��v3�汾��ֻ���ڷ��������պͿͻ��˵ķ�
#define INQUIRY_PORT "27016"
//v3���̰߳汾�е��Ӷ˿�
//���ڷ������ķ��Ϳͻ��˵���


int __cdecl main(void)
{
    
    vector<User> userData;
    vector<Message> mesCache;
    mesCache.clear();
    MesCacheInfo mesCacheInfo((int)(mesCache.size()), &mesCache);
    //��mesCache�����Сһ���װ    

    //������ݿ�

    // ����Ҫ��һ�����̰߳�һ���µļ����˿�
    //HANDLE inquiryThread = CreateThread(NULL, 0, handleInquiry, (void*) &mesCacheInfo, 0, NULL);
    //����һ����װ��Ϣ��

    initialize(&userData);

    WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	char recvBuf[DEFAULT_BUFLEN] = {0};

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

    char fileFlag = 0;
    //�ļ������ʶ�� ���ò�д�ڴ�ѭ������
    char tempFile[DEFAULT_BUFLEN];
    memset(tempFile, '\0', DEFAULT_BUFLEN);
    //��ǰ�غϽ������ļ� ��recvBuf��ֵת�ƽ���
    //�»غ�recvBuf�������ļ���С
    //�ٰ�tempFileд��
    

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


        // for (int i = 0; i < DEFAULT_BUFLEN; i++)
        //     recvBuf[i] = 0;
        //��ս��ճ�

        memset(recvBuf, 0, DEFAULT_BUFLEN);//recvBuf����

        char fileSizeStr [50] = {0};
        //������ʱ�����ļ���С
        unsigned int fileSize = 0;

        iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        cout << recvBuf << endl;

        if(recvBuf[2] != '|' && fileFlag == 0) fileFlag = 1;
        if(recvBuf[2] == '|') fileFlag = 0;

        if (iResult > 0)
        //����Ϊ�˲����ļ����� ע�͵��˺ܶ��޹ض��� ��Ȼ�ļ�����û������
        {
            // printf("%s\n", recvBuf);
            string recvBufToStr = recvBuf;
            string retVal = "���ͳɹ�";

            //ת��recvBuf

            if(!fileFlag)
            retVal = OutOfNetwork(recvBufToStr, userData, &mesCacheInfo);
            //Ϊ�˲����ļ����� ���ò���OutOfNetwork()��������������ע�͵���

            // if(fileFlag == 1)
            // {
            //     cout << (int) recvBuf[i] << endl;
            // }


            cout << "������Ϣ�� " << retVal << endl;

            int res = send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            // ����Ҫ�޸� Ҫ��Ӧ���̵߳��޸� �˴�ֻ���ط��ͳɹ����źž����� Ҫ�Ķ�OutOfNetwork����

            if (fileFlag == 1)
            {
                for(int i = 0; i < DEFAULT_BUFLEN; i ++)
                {
                    tempFile[i] = recvBuf[i];
                }
                cout << "tempFile: \n" << tempFile << endl;
            }

            //fileFlagΪ1 ��ǰ���յ����ļ�
            //fileFlagΪ2 ��ǰҪ�����ļ���С
            
            if (res == SOCKET_ERROR)
            {
                printf("����ʧ��: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            // if (fileFlag)
            // //�����ļ����Ļ�����Ϣ/��¼����
            // //����֮�� Ϊ�˸�ʱ����demo
            // {
            //     string retVal = "���յ��ļ���С";
            //     int fres = recv(ClientSocket, fileSizeStr, 50, 0);
            //     send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            //     fileSize = atoi(fileSizeStr);
            //     cout << "�ļ���С���� " << fileSizeStr << endl;
            //     memset(fileSizeStr, '\0', 50);
            // }
        }
        if (iResult == 0)
        //����ԭ����else if (iResult == 0)
        {
            printf("���Ӽ����ر�");
            //return 1;
        }

 
        // else
        // {
        //     printf("���ӷ�������", WSAGetLastError());
        //     closesocket(ClientSocket);
        //     WSACleanup();
        //     return 1;
        // }

        // if (iResult == SOCKET_ERROR)
        // {
        //     printf("shutdown failed with error: %d\n", WSAGetLastError());
        //     closesocket(ClientSocket);
        //     WSACleanup();
        //     return 1;
        // }
        // cout << recvBuf << endl;

        if (fileFlag == 2)
        {
            fileSize = atoi(recvBuf);
            //cout << "�ļ���С���� " << tempFile << endl;
            FILE* fout = fopen("C:\\Users\\Mark.Wen\\Desktop\\SupremeChat\\ConsoleApplication1\\Debug\\10.png", "wb");
            fwrite(tempFile, fileSize, 1, fout);
            fclose(fout);
            memset(tempFile, '\0', DEFAULT_BUFLEN);
            memset(recvBuf, '\0', DEFAULT_BUFLEN);
        }

        if(fileFlag == 1) fileFlag ++;
        // cleanup
        // closesocket(ClientSocket);
    }
}