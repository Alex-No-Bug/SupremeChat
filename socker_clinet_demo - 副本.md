#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "string"
#include "iostream"
using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    //���ڳ�ʼ��ַģ�������
    char sendbuf[DEFAULT_BUFLEN] = {0};
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    //�ӷ������˷��صĵ�ַЭ���Ƿ�ָ����
    //AF_INETָ��ipv4
    //AF_INET6ָ��ipv6
    hints.ai_socktype = SOCK_STREAM;//STREAMģʽ
    hints.ai_protocol = IPPROTO_TCP;//TCPЭ��

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    //ͨ�������в����ѷ����������룿
    //�ڷ������˵�һ��������NULL
    //IPV4ʹ�õ㻮ʮ���� ipv6ʹ��ʮ������
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    //�ж��Ƿ���������

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        //result�����һ����ָ��
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        //����Ϊ���ӵ���ַ������Э��ĸ���ϸ��

        //����Ϊ���ӵ�����Ķ˵�
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            //һ��ʧ�� �˴�����ٳ�������һ��
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    //�˴���ERROR��ѭ���ڵ�ERROR�к�����Ϊʲô��Ҫר��д������

    while (true)
    {
        cout << "��������Ϣ" << endl;
		string temp;
        getline(cin, temp);
        if(temp == "  ") break;
        //��������������ո����˳�ѭ��
		for (int i = 0; i < temp.length(); i++)
			sendbuf[i] = temp[i];
        // Send an initial buffer
        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if(temp == "  ")
        {
            cout << "�����رջỰ" << endl;
            break;
        } 
        //��������������ո����˳�ѭ��
        for (int i = 0; i < DEFAULT_BUFLEN; i++)
            sendbuf[i] = 0;
        if (iResult == SOCKET_ERROR)
        {
            printf("����ʧ��: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        for (int i = 0; i < DEFAULT_BUFLEN; i++)
            recvbuf[i] = 0;
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            cout << recvbuf << endl;
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    }

    // // Send an initial buffer
    // iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    // //sendbuf ��char��ָ�� ָ��Ҫ���͵��ַ���
    // if (iResult == SOCKET_ERROR) {
    //     printf("send failed with error: %d\n", WSAGetLastError());
    //     closesocket(ConnectSocket);
    //     WSACleanup();
    //     return 1;
    // }

    // printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    // the client can still use the ConnectingSocket for receiving
    // iResult = shutdown(ConnectSocket, SD_SEND);
    // if (iResult == SOCKET_ERROR) {
    //     printf("shutdown failed with error: %d\n", WSAGetLastError());
    //     closesocket(ConnectSocket);
    //     WSACleanup();
    //     return 1;
    // }

    // Receive until the peer closes the connection
    // do {
    //     for(int i = 0; i < DEFAULT_BUFLEN; i ++)
    //         recvbuf[i] = 0;
    //     iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    //     if ( iResult > 0 )
    //         cout << recvbuf << endl;
    //     else if ( iResult == 0 )
    //         printf("Connection closed\n");
    //     else
    //         printf("recv failed with error: %d\n", WSAGetLastError());

    // } while( iResult > 0 );

    // cleanup
    cout << "�ر���������" << endl;
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}