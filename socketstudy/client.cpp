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
	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(clientSocket==INVALID_SOCKET)
	{
		MessageBox(NULL, L"Invalid socket", L"ERROR", MB_OK | MB_ICONERROR);
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN clientAddr;
	memset(&clientAddr, 0, sizeof(SOCKADDR_IN));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(23000);
	clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if(connect(clientSocket,(sockaddr *) &clientAddr, sizeof(SOCKADDR_IN))==SOCKET_ERROR)
	{
		MessageBox(NULL, L"Connect failed", L"ERROR", MB_OK | MB_ICONERROR);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
	WSACleanup();
}