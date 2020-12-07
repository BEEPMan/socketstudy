#pragma comment(lib,"ws2_32")
#include<iostream>
#include<WinSock2.h>
#include<memory.h>

#define MAX_LEN 512

using namespace std;

int main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		return 1;
	//SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(23000);

	if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MessageBox(NULL, L"Bind failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		MessageBox(NULL, L"Listen failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	char readBuffer[MAX_LEN];
	int bufLen;
	while (1)
	{
		clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			MessageBox(NULL, L"Accept failed", L"ERROR", MB_OK | MB_ICONERROR);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		cout << "Accept Success" << endl;
		bufLen = recv(clientSocket, readBuffer, MAX_LEN, 0);
		if (bufLen > 0)
		{
			send(clientSocket, readBuffer, bufLen, 0);
		}
		else
		{
			cout << "Read ERROR" << endl;
		}
		closesocket(clientSocket);
	}

	closesocket(listenSocket);
	WSACleanup();
	return 0;
}