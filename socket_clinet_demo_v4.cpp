/* �ͻ��˰汾4
    �ļ�����demo
    ��Ҫ�½�һ�ֱ�������
    ��ʱ��ʹ��27015�����ļ�*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "string"
#include "iostream"
#include "client_user.h"
#include "time.h"
#include "fstream"
using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 8188

#define DEFAULT_PORT "27015"
//�ϰ汾�������շ�һ��Ķ˿�
//��v3�汾��ֻ���ڷ��������պͿͻ��˵ķ�
#define INQUIRY_PORT "27016"
//v3���̰߳汾�е��Ӷ˿�
//���ڷ������ķ��Ϳͻ��˵���

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
    
    int userName = 0;
    string userip="";

    do
    {
        //1name|code|ip
        getLoginInfo(&input);
        //�и� 
        int i=1;
        string temp="";
        while(input[i]!='|')
        {
            temp+=input[i++];
        }
        userName=atoi(temp.c_str());
        i++;//��ʱ |
        while(input[i]!='|')
        {
           i++;
        }
        i++;
      
        while(input[i]!= 0)
        {
            userip+=input[i++];
        }
        funcRes = firstTimeLogin(&ipAddrServer, input, &hints, &result);
    } while(funcRes != 0);
    //�û������֤���

    //����һ��ѯ���û��������������Ϣ
    InqClitInfo clinetInfo(userName, ipAddrServer);
    //�øö�������ʼ��һ����ѯ�߳�
    //���̼߳����û������֤��ɺ�ͷ��͵�һ����Ϣ����ǰ
    //HANDLE inquiryThread = CreateThread(NULL, 0, inquiry, (void*)&clinetInfo, 0, NULL);


    while (input != "2  ")
    //���Ͳ���
    {

        input.clear();

        string userNameStr = to_string(userName);

        // fakeListen(&ipAddrServer, userNameStr, &hints, &result);
        string temp = "";
        int recvFri = 0;

        int fileOrNot = 0;
        cout << "��Ҫ�����ļ�����������" << endl;
        cin >> fileOrNot;

        if(fileOrNot)
        //�����ļ�
        {
            char fileName[100] = "C:\\Users\\Mark.Wen\\Desktop\\SupremeChat\\ConsoleApplication2\\Debug\\1.png";
            unsigned long fileSize = 0;
            //cout << "�������ļ���" << endl;
            //cin >> fileName;

            // ifstream fileToSend (fileName, ios::binary | ios:: in | ios::ate);
            FILE* fp = fopen(fileName, "rb");
            fseek(fp, 0, SEEK_END);
            fileSize = ftell(fp);
            rewind(fp);
            
            // fileSize = fileToSend.tellg();
			// fileSize++;
            // fileToSend.seekg(0, ios::beg);

            char* fileInMem = new char [fileSize];
            memset(fileInMem, 0, fileSize);
            fread(fileInMem, fileSize, 1, fp);
            fclose(fp);
            // //�¿���һ���ڴ�ռ������մ��ļ�
            // fileToSend.read(fileInMem, fileSize);
            // fileToSend.close();

            // string binFileToStr(fileInMem);
            // cout << "�ļ����ݣ� " << binFileToStr << endl;
            //������͵������������
            //��ʱ�Ȳ�дר�õ��ļ����ͺ����� ���������˼һ��
            sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, fileSize);
            sendMessageToServer(&ipAddrServer, to_string(fileSize), &hints, &result);
            // for(int i = 0; i < fileSize; i ++) 
            //     cout << (int)fileInMem[i] << endl;
        }
        

        cout << "�����뷢�Ͷ���" << endl;
        cin >> recvFri;
        getline(cin, temp);

        input += "2";
        input =input + to_string(userName)+"|";
        //��ȡʱ��
        time_t timep;
        time(&timep);
        char tmp[64];
        strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
        input = input + to_string(recvFri) + "|" + tmp + '|';
        input += temp;

        funcRes = sendMessageToServer(&ipAddrServer, input, &hints, &result);
    }
    system("pause");
}

