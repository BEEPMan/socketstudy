#pragma comment(lib,"ws2_32")
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<memory.h>
#include<string>

#define MAX_LEN 512
#define SERVER_PORT 23000
#define SERVER_IP "127.0.0.1"

using namespace std;

int main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		return 1;
	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Invalid socket", L"ERROR", MB_OK | MB_ICONERROR);
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN clientAddr;
	memset(&clientAddr, 0, sizeof(SOCKADDR_IN));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(SERVER_PORT);
	inet_pton(clientAddr.sin_family, SERVER_IP, &clientAddr.sin_addr.S_un.S_addr);
	if (connect(clientSocket, (sockaddr*)&clientAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MessageBox(NULL, L"Connect failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	char messageBuffer[MAX_LEN];
	int bufLen;
	int sendLen;
	int recvLen;
	while (1)
	{
		cout << "Send message: ";
		int i;
		for (i = 0; 1; i++)
		{
			messageBuffer[i] = getchar();
			if (messageBuffer[i] == '\n')
			{
				messageBuffer[i++] = '\0';
				break;
			}
			if (i == MAX_LEN - 1)
			{
				messageBuffer[i] = '\0';
				break;
			}
		}
		bufLen = i;
		sendLen = send(clientSocket, messageBuffer, bufLen, 0);
		if (sendLen > 0)
		{
			recvLen = recv(clientSocket, messageBuffer, MAX_LEN, 0);
			if (recvLen > 0)
			{
				cout << "Receive message: " << messageBuffer << endl;
			}
		}
	}

	closesocket(clientSocket);
	WSACleanup();
}