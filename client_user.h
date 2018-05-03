#pragma once
#define WIN32_LEAN_AND_MEAN

#include "winsock2.h"
#include "ws2tcpip.h"
#include "string"
#include "iostream"
#include "cstring"
#include "windows.h"
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define LOGIN_FLAG '1'
#define FRIMES_FLAG '2'
#define INQU_FLAG '3'

#define DEFAULT_PORT "27015"
//�ϰ汾�������շ�һ��Ķ˿�
//��v3�汾��ֻ���ڷ��������պͿͻ��˵ķ�
#define SUB_PORT "27016"
//v3���̰߳汾�е��Ӷ˿�
//���ڷ������ķ��Ϳͻ��˵���



void getLoginInfo(string* sendMes)
/* �������ܣ��û��������룬�����ձ��ĸ�ʽд��������ַ���ָ��
   �������룺������������ָ�� */
{
    (*sendMes).clear();
    (*sendMes) += "1";
    string temp, temp1;
    cout << "�������û�����" << endl;
    getline(cin, temp);
    temp.append("|");
    cout << "���������룺 " << endl;
    getline(cin, temp1);
    temp.append(temp1);
    temp.append("|");
    temp1.clear();
    cout << "������IP��ַ�� " << endl;
    getline(cin, temp1);
    temp.append(temp1);
    *sendMes = LOGIN_FLAG;
    (*sendMes).append(temp);
    system("pause");
    // system("cls");
}

int firstTimeLogin(string* ipAddrServer, string loginInfo, addrinfo* hint, addrinfo **result)
/* ���ͳ�ʼ��¼��Ϣ���� ���ĸ�ʽΪ1�û���|����|IP��ַ
   ����ֵ-1Ϊ��ַ����
   ����ֵ-2ΪSOCKET�������� 
   ����ֵ-3Ϊ���ӷ�����ʧ��
   ����ֵ-4Ϊ���͵�¼��Ϣʧ��
   ����ֵ0 ��¼�ɹ�
   ����ֵ1 �û������������
   ����ֵ2 �����ڸ��û�*/
{
    int iRes = getaddrinfo((*ipAddrServer).c_str(), "27015", hint, result);
    if(iRes != 0){
        cout << "��ַ���� " << iRes << endl;
        // WSACleanup();
        return -1;
    }
    
    struct addrinfo* ptr = *result;
    SOCKET ConnectSocket;

    for (; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                                      ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("SOCKET��������: %ld\n", WSAGetLastError());
            // WSACleanup();
            return -2;
        }

        iRes = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iRes == SOCKET_ERROR) {
            //һ��ʧ�� �˴�����ٳ�������һ��
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("����ʧ��!\n");
        // WSACleanup();
        return -3;
    }

    iRes = send(ConnectSocket, loginInfo.c_str(), loginInfo.length(), 0);
    //���ͳ�ʼ��¼��Ϣ

    if (iRes == SOCKET_ERROR)
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        // WSACleanup();
        return -4;
    }

    char recvCache[DEFAULT_BUFLEN] = {0};
    iRes = recv(ConnectSocket, recvCache, DEFAULT_BUFLEN, 0);
    string mesFromCache = recvCache;
    mesFromCache[0] = ' ';
    if(iRes > 0)
    {
        if(recvCache[0] == '0') {cout << "��¼�ɹ�" << endl; cout << mesFromCache << endl;return 0;}
        if(recvCache[0] == '1') {cout << "�û������������" << endl; return 1;}
        if(recvCache[0] == '2') {cout << "�����ڸ��û�" << endl; return 2;}
    }
    else if (iRes == 0) printf("�����ѹر�\n");
    else printf("����ʧ��: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    system("pause");
    // system("cls");
}

int sendMessageToServer(string* ipAddrServer, string message, 
                addrinfo* hint, addrinfo **result)
/* �������ܣ�������ͨ�Ի���Ϣ 
   ���ĸ�ʽΪ flag������ID|������ID|����ʱ��|��Ϣ*/
{
    int iRes = getaddrinfo((*ipAddrServer).c_str(), "27015", hint, result);
    if(iRes != 0){
        cout << "��ַ���� " << iRes << endl;
        // WSACleanup();
        return -1;
    }
    
    struct addrinfo* ptr = *result;
    SOCKET ConnectSocket;

    for (; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                                      ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("SOCKET��������: %ld\n", WSAGetLastError());
            // WSACleanup();
            return -2;
        }

        iRes = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iRes == SOCKET_ERROR) {
            //һ��ʧ�� �˴�����ٳ�������һ��
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("����ʧ��!\n");
        // WSACleanup();
        return -3;
    }

    iRes = send(ConnectSocket, message.c_str(), message.length(), 0);
    //���ͳ�ʼ��¼��Ϣ

    if (iRes == SOCKET_ERROR)
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        // WSACleanup();
        return -4;
    }

    char recvCache[DEFAULT_BUFLEN] = {0};
    iRes = recv(ConnectSocket, recvCache, DEFAULT_BUFLEN, 0);
    if(iRes > 0)
    {
        cout << "���ͳɹ�" << endl;
        cout << recvCache << endl;
    }
    else if (iRes == 0) printf("�����ѹر�\n");
    else printf("����ʧ��: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
}

int fakeListen(string* ipAddrServer, string userid, addrinfo* hint, addrinfo **result)
/* ̽�麯��
   ÿ��ѭ����Ҫ�����������ѯ�ʱ���
   ���������յ�flagλѯ�ʱ��ĵ���Ϣʱ
   �����Ϣ����
   ���ĸ�ʽΪ 3.userid*/
{
    int iRes = getaddrinfo((*ipAddrServer).c_str(), "27015", hint, result);
    if(iRes != 0){
        cout << "��ַ���� " << iRes << endl;
        // WSACleanup();
        return -1;
    }
    
    struct addrinfo* ptr = *result;
    SOCKET ConnectSocket;

    for (; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                                      ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("SOCKET��������: %ld\n", WSAGetLastError());
            // WSACleanup();
            return -2;
        }

        iRes = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iRes == SOCKET_ERROR) {
            //һ��ʧ�� �˴�����ٳ�������һ��
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("����ʧ��!\n");
        // WSACleanup();
        return -3;
    }

    userid = "3" + userid;

    iRes = send(ConnectSocket, userid.c_str(), userid.length(), 0);
    //���ͳ�ʼ��¼��Ϣ

    if (iRes == SOCKET_ERROR)
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        // WSACleanup();
        return -4;
    }

    char recvCache[DEFAULT_BUFLEN] = {0};
    iRes = recv(ConnectSocket, recvCache, DEFAULT_BUFLEN, 0);
    if(iRes > 0)
    {
        cout << recvCache << endl;
        return 41;
    }
    else if (iRes == 0) printf("�����ѹر�\n");
    else printf("����ʧ��: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    system("pause");
    // system("cls");
}


class InqClitInfo
//InquiryClientInfo
//ר��Ϊinqury���߳���Ƶ���
{
    public:
     InqClitInfo(int inputName, string inputAddr)
     {
         userName = inputName;
         addr = inputAddr;
     }

     int userName;//�û���
     string addr;//������IP��ַ
};


#define INQUIRY_PORT "27016"
//ѯ�ʱ��Ķ˿�


int sendInquiryMessage(string* ipAddrServer, string message, 
                addrinfo* hint, addrinfo **result)
/* �������ܣ�������ͨ�Ի���Ϣ 
   ���ĸ�ʽΪ flag������ID
   flag��־λΪ3*/
{
    int iRes = getaddrinfo((*ipAddrServer).c_str(), INQUIRY_PORT, hint, result);
    if(iRes != 0){
        cout << "��ַ���� " << iRes << endl;
        // WSACleanup();
        return -1;
    }
    
    struct addrinfo* ptr = *result;
    SOCKET ConnectSocket;

    for (; ptr != NULL; ptr = ptr->ai_next)
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                                      ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("SOCKET��������: %ld\n", WSAGetLastError());
            // WSACleanup();
            return -2;
        }

        iRes = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iRes == SOCKET_ERROR) {
            //һ��ʧ�� �˴�����ٳ�������һ��
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("����ʧ��!\n");
        // WSACleanup();
        return -3;
    }

    iRes = send(ConnectSocket, message.c_str(), message.length(), 0);
    //���ͳ�ʼ��¼��Ϣ

    if (iRes == SOCKET_ERROR)
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        // WSACleanup();
        return -4;
    }

    char recvCache[DEFAULT_BUFLEN] = {0};

    iRes = recv(ConnectSocket, recvCache, DEFAULT_BUFLEN, 0);
    cout << "client_user 350 iRes " << iRes << endl;
    

    if(iRes > 0)
    {
        cout << "��̽�ɹ�" << endl;
        string recvStr(recvCache);
        if(recvStr != " ")
            cout << recvCache << endl;
    }
    else if (iRes == 0) printf("�����ѹر�\n");
    else 
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
    }
   
}




DWORD WINAPI inquiry(LPVOID clientInfo)
//inquiry�߳�
//���ĸ�ʽΪ"3�û���"
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN] = {0};
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int funcRes;
    int recvbufLen = DEFAULT_BUFLEN;

    InqClitInfo* info = (InqClitInfo*) clientInfo;

    string ipAddrServer = info -> addr;
    //���߳�������õ���������ip��ַ

    funcRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (funcRes != 0) {
        printf(" inquiry WSAStartup failed with error: %d\n", funcRes);
    }
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    //�ӷ������˷��صĵ�ַЭ���Ƿ�ָ����
    //AF_INETָ��ipv4
    //AF_INET6ָ��ipv6
    hints.ai_socktype = SOCK_STREAM;//STREAMģʽ
    hints.ai_protocol = IPPROTO_TCP;//TCPЭ��
    //����SOCKET��������


    while (true)
    //���Ͳ���
    {
        string userNameStr = to_string(info -> userName);

		cout << "client_user 407 " << endl;

        // fakeListen(&ipAddrServer, userNameStr, &hints, &result);
        string input = "";
        //���ڷ��͵��ַ���
        input += "3";
        input =input + userNameStr;
        //���ɱ���

        //����ط�Ҫ��������һ�¶˿ں�
        funcRes = sendInquiryMessage(&ipAddrServer, input, &hints, &result);

        Sleep(80);
        //ÿ100����ִ��һ�ε�����������ѯ
    }

}