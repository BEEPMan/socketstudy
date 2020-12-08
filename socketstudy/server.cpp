#pragma comment(lib,"ws2_32")
#include<iostream>
#include<WinSock2.h>
#include<memory.h>
#include<string>

#define MAX_LEN 6
#define SERVER_PORT 23000

using namespace std;

int main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		MessageBox(NULL, L"Can not load 'winsock.dll' file", L"ERROR", MB_OK | MB_ICONERROR);
		return 1;
	}

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Invalid socket", L"ERROR", MB_OK | MB_ICONERROR);
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MessageBox(NULL, L"Bind failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		MessageBox(NULL, L"Listen failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "Listening for accept..." << endl;

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	char readBuffer[MAX_LEN];
	int bufLen;
	clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		MessageBox(NULL, L"Accept failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "Accept Success" << endl;

	while(1)
	{
		memset(&clientAddr, 0, sizeof(SOCKADDR_IN));
		bufLen = recv(clientSocket, readBuffer, MAX_LEN, 0);
		if (bufLen > 0)
		{
			cout << "Receive message: " << readBuffer << endl;
			send(clientSocket, readBuffer, bufLen, 0);
		}
		else
		{
			break;
		}
	}

	closesocket(listenSocket);
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}