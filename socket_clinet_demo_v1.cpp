/* �ͻ��˰汾1
   ��ȷ���������޷�ͬʱ�������������������
   ͨ����ʼ��¼��֤ �Լ�ÿ����һ�����ľͶϿ����ӵķ�ʽ�������
   �ؼ����ڷ������˵�¼״̬�ĸ��� �ͳ�ʼsocket�������� */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "string"
#include "iostream"
#include "client_user.h"
using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN] = {0};
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int funcRes;
    int recvbufLen = DEFAULT_BUFLEN;

    cout << "�������������ǰIP��ַ" << endl;
    string ipAddrServer;
    getline(cin, ipAddrServer);
    system("pause");
    system("cls");

    funcRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (funcRes != 0) {
        printf("WSAStartup failed with error: %d\n", funcRes);
        return 1;
    }
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    //�ӷ������˷��صĵ�ַЭ���Ƿ�ָ����
    //AF_INETָ��ipv4
    //AF_INET6ָ��ipv6
    hints.ai_socktype = SOCK_STREAM;//STREAMģʽ
    hints.ai_protocol = IPPROTO_TCP;//TCPЭ��
    //����SOCKET��������

    string input;
    input.clear();
    //�û������ַ���
    
    do
    {
        getLoginInfo(&input);
        funcRes = firstTimeLogin(&ipAddrServer, input, &hints, &result);
    } while(funcRes != 0);


    while (input != "2  ")
    {
        input.clear();
        string temp;
        cout << "���������Ϣ" << endl;
        getline(cin, temp);
        input += "2";
        input += temp;
        funcRes = sendMessageToServer(&ipAddrServer, input, &hints, &result);
        system("pause");
    }
}
