/*�������˺�̨��������
�����κδ�Socket�Ĺ��� */

#pragma once
#include "User.h"
#include "string.h"
#include "string"
#include "windows.h"
#define WIN32_LEAN_AND_MEAN
#define INQUIRY_PORT "27016"
#define DEFAULT_BUFLEN 512
#define INQ_FLAG 3


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

using namespace std;

class MesCacheInfo
{
	public:
	int size;
	//���Ļ����Ĵ�С
	vector<Message>* mesCache;
	//���Ļ����ĵ�ַ

	MesCacheInfo(int i, vector<Message>* m)
	{
		size = i;
		mesCache = m;
	}

};


int check_user(vector<User>& user, string s);
string checkMesCache (MesCacheInfo* mesCacheInfo, int userid);

string OutOfNetwork(string rualMessage, vector<User>& dataBase, 
                    MesCacheInfo* mesCacheInfo)
//�˳�Ҫ��������ֱ���ͷ����
{
	vector<Message>* mesCache= mesCacheInfo -> mesCache;
	int k=1;
    string temp="";
	while(rualMessage[k]!='|')
	{ 
	  temp=temp+rualMessage[k++];

	}	

	cout << temp << " user" << endl;

	int userid = atoi(temp.c_str());

	if(rualMessage[0] == LOGIN_FLAG) //��¼��֤
    {
		int checkRes = check_user(dataBase, rualMessage);
        if(checkRes == 0)//���ƥ��ɹ�
        {
            string retVal = to_string(userid);
			cout << retVal << "�û� LOGIN" << endl;
			retVal = '0';
			
            return retVal;
        }
        else if(checkRes == -1)
        {
            string retVal = "1";
            return retVal;
        }
        else
		{
			string retVal = "2";
            return retVal;
        }
    }

	else if(rualMessage[0] == INQ_FLAG)
	{
		//��¼����Ҫ�ĳ���϶��̵߳�
		int inquiryOrigin = 0;
		string temp = "";
		//��̽��Դ�û���
		for(int i = 1; i < rualMessage.size(); i ++)
			temp += rualMessage[i];
		inquiryOrigin = atoi(temp.c_str());
		string recvMessage = checkMesCache(mesCacheInfo, inquiryOrigin);
		return recvMessage;
	}

    else if (rualMessage[0] == FRIMES_FLAG)//��ͨ����
	// ����д�������
    {
		Message curMesg = Message(rualMessage); //��ǰ��Ϣ
		(*mesCache).push_back(curMesg);			//���뻺���
		mesCacheInfo -> size ++;
		//��װ����Ҫ����д��ʱ�Ĵ�Сͬ������

		// string idChar = rualMessage;
		// int posi = idChar.find('|');
		// idChar = idChar.substr(posi, idChar.length() - posi- 1);
		//�õ�������id���ַ���
		
		// string idChar = rualMessage;
		// int posi = idChar.find('|');
		// idChar = idChar.substr(0, posi - 1);
		// int userid = atoi(idChar.c_str());

		// string retVal = checkMesCache(mesCache, userid);
		// cout << retVal << " FRI_MES" << endl;
		string retVal = "";
		return retVal;
		//����һ�����ַ���
	}
}

int check_user(vector<User>& user, string s)
//�û������֤����
{
	int i = 0;
	i++; //i=1
	string temp = "";
	//1ID|code|ip
	while (s[i] != '|')
	{
		temp += s[i];
		i++;
	}
	int inID = atoi(temp.c_str());
	//PASS_WORD
	i++;
	temp = "";
	while (s[i] != '|')
	{
		temp += s[i];
		i++;
	}
	char inCode[7];
	for (int i = 0;i < 6;i++)
		inCode[i] = temp[i];
	inCode[6] = 0;
	cout << "code in the database:" << inCode << endl;
	//IP
	string ip = "";


	while (s[i] != 0)
	{
		ip += s[i];
		i++;
	}


	//check
	int ok = 0;
	for (int i = 0; i < (user).size(); i++)
	{
		if ((user)[i].id == inID && strcmp((user)[i].cipher, inCode) != 0)
		{
			return (-1); //pass_word����
		}
		else if ((user)[i].id == inID && strcmp((user)[i].cipher, inCode) == 0)
		{
			(user)[i].IPaddress = ip;
			(user)[i].onlineFlag = true;
			return (0); //��֤�ɹ�
		}
	}
	return (-2); //�����ڸ��û�
}


string checkMesCache (MesCacheInfo* mesCacheInfo, int userid)
//����Ƿ��и��û�����Ϣ����
//��ʽΪ��ʱ�� ������ ���� ��Ϣ ����
//ÿ��ȡ��һ����Ϣ �ͽ�����Ϣ�����ݿ���ɾ��
{
    string retVal = "*";
	if(mesCacheInfo -> size == 0) return retVal;

	vector<Message>* mesCache = mesCacheInfo -> mesCache;

    for(int i = 0; i < (mesCacheInfo -> size); i ++)
    {
        if((*mesCache).at(i).receiver == userid)
        {
            retVal += (*mesCache). at(i).launchTime;
            retVal += " ";
            retVal += to_string((*mesCache). at(i).sender) + "\n"
						+ (*mesCache). at(i).message + "\n";
			vector<Message>:: iterator cur = ((*mesCache).begin());
			(*mesCache).erase(cur + i);
			mesCacheInfo -> size --;
			//��װ����ɾ��һ����Ϣʱ��СҲҪͬ������
			i -= 1;
        }
    }
	cout << retVal << " check" << endl;
	return retVal;
}


DWORD WINAPI handleInquiry(LPVOID mesCacheInfo)
//�������˵ļ����߳�
//����Ĳ���ֻҪ��Ϣ���ݿ�͹���
// bug����mesCache��Ϊһ����ָ�봫������ʱ�� ��size���Ծͱ����ҵ��� ֮����������
// ��������������½�һ���� ��mesCache��size��װ��һ��
// �������� 1. Ҫ�Ķ�checkMesCache
// 2. ������Ϣ������߱�ɾ��ʱ ��α�֤size��ֵ��ʵʱ������

{
	// vector<Message>& tempMesCache = (vector<Message>&) mesCache;

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
		printf("���߳�Socket�汾����: %d\n", iResult);
		return 1;
	}

    cout << "���߳�Socket�汾��ʼ�����" << endl;

	ZeroMemory(&hints, sizeof(hints));
	//������
	hints.ai_family = AF_INET;//�޶�Ϊipv4
	hints.ai_socktype = SOCK_STREAM;//�޶�Ϊstreamģʽ����
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//��¼�������ӵ�ip��ַ

    iResult = getaddrinfo(NULL, INQUIRY_PORT, &hints, &result);
	//�ѷ������˿ڡ���ַ�����ò���д��result hints�൱��һ���н�
	//�˴��Ķ˿ڸ�Ϊ27016
	if (iResult != 0) {
		printf("���̲߳�����ʼ������: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	cout << "���߳�Socket������ʼ�����" << endl;


    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//��ListenSocket�����ճ�ʼ����socket�ĸ�����Ϣ
	if (ListenSocket == INVALID_SOCKET) {
		printf("���̼߳�����ʼ������: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	cout << "���߳�Socket������ʼ�����" << endl;

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	//��
	if (iResult == SOCKET_ERROR) {
		printf("���̰߳󶨹���: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	cout << "���߳�Socket�󶨳�ʼ�����" << endl;

	while(true)
    {
        iResult = listen(ListenSocket, SOMAXCONN);

        //ͨ���������ֻ�д��ѭ�� ֱ�������ӽ���Ϊֹ
        if (iResult == SOCKET_ERROR)
        {
            printf("���̼߳���ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        //ѭ������

        cout << "���߳����ڵȴ�����..." << endl;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
		{
			cout << "���߳̽���ʧ��" << endl;
			continue;
		}
		cout << "���߳��յ�һ����Ϣ: " << endl;

        if (ClientSocket == INVALID_SOCKET)
        {
            printf("���߳̽���ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            // WSACleanup();
            return 1;
        }

        for (int i = 0; i < DEFAULT_BUFLEN; i++)
			recvBuf[i] = 0;
        //�ڽ��ձ���ǰ��ս��ճ�

		iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            printf("%s\n", recvBuf);
            string recvBufToStr = recvBuf;
            string retVal = " ";
			// recvBufToStr��ת��string�ı���

			string temp = "";
			for (int i = 1; i < recvBufToStr.size(); i++)
				temp += recvBufToStr[i];
			int userid = atoi(temp.c_str());
			//��̽��Դ�û���

			retVal = checkMesCache((MesCacheInfo*)mesCacheInfo, userid);

            cout << "���̷߳���" << endl << retVal << endl;

            int res = send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            // ����Ҫ�޸� Ҫ��Ӧ���̵߳��޸� �˴�ֻ���ط��ͳɹ����źž����� Ҫ�Ķ�OutOfNetwork����
            if (res == SOCKET_ERROR)
            {
                printf("����ʧ��: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0)
            printf("���Ӽ����ر�");
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
        // closesocket(ClientSocket);
    }
}
