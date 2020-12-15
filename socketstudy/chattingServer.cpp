#pragma comment(lib,"ws2_32")
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<memory.h>
#include<string>
#include<thread>
#include<vector>

#define MAX_CLIENT 5
#define MAX_LEN 512
#define SERVER_PORT 23000

using namespace std;

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	char messageBuffer[MAX_LEN];
	int receiveBytes;
	int sendBytes;
};

struct CLIENTINFO
{
	SOCKET socket;
	SOCKADDR_IN address;
	char ip[22];
};

vector<CLIENTINFO*> userList;

void workerThread(LPVOID hIOCP);

int main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "ERROR - Can't Load 'winsock.dll' File" << endl;
		return 1;
	}

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "ERROR - Invalid Socket" << endl;
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
		cout << "ERROR - Binding Failure" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "ERROR - Listening Failure" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "Listening for Accept..." << endl;

	HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int threadCount = systemInfo.dwNumberOfProcessors * 2;

	vector<thread> threadPool;
	for (int i = 0; i < threadCount; i++)
	{
		threadPool.emplace_back(thread{ workerThread, &hIOCP });
	}

	int addrLen = sizeof(SOCKADDR_IN);
	char readBuffer[MAX_LEN];
	int bufLen;
	SOCKETINFO* socketInfo = (struct SOCKETINFO*)malloc(sizeof(struct SOCKETINFO));
	DWORD receiveBytes;
	DWORD flags;
	CLIENTINFO* tempClient;

	while (1)
	{
		tempClient = (struct CLIENTINFO *)malloc(sizeof(struct CLIENTINFO));
		tempClient->socket = accept(listenSocket, (struct sockaddr*)&(tempClient->address), &addrLen);
		if (tempClient->socket == INVALID_SOCKET)
		{
			cout << "ERROR - Accept Failure" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		inet_ntop(tempClient->address.sin_family, &(tempClient->address.sin_addr.S_un.S_addr), tempClient->ip, INET_ADDRSTRLEN);
		userList.push_back(tempClient);
		cout << "Accept Success (User IP : " << tempClient->ip << ")" << endl;

		memset((void*)socketInfo, 0, sizeof(struct SOCKETINFO));
		socketInfo->dataBuffer.len = MAX_LEN;
		socketInfo->dataBuffer.buf = socketInfo->messageBuffer;
		flags = 0;
		hIOCP = CreateIoCompletionPort((HANDLE)tempClient->socket, hIOCP, (DWORD)tempClient, 0);

		if (WSARecv(tempClient->socket, &(socketInfo->dataBuffer), 1, &receiveBytes, &flags, &(socketInfo->overlapped), NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				cout << "ERROR - IO Pending Failure" << endl;
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}
		}
	}

	for (auto& t : threadPool)
	{
		t.join();
	}
	for (auto& s : userList)
	{
		closesocket(s->socket);
	}
	closesocket(listenSocket);
	WSACleanup();
	return 0;
}

void workerThread(LPVOID hIOCP)
{
	DWORD recieveBytes;
	DWORD sendBytes;
	CLIENTINFO* clientInfo;
	DWORD flags;
	SOCKETINFO* eventSocket;

	while (1)
	{
		if (GetQueuedCompletionStatus(*((HANDLE*)hIOCP), &recieveBytes, (PULONG_PTR)&clientInfo, (LPOVERLAPPED*)&eventSocket, INFINITE) == 0)
		{
			cout << "ERROR - GetQueuedCompletionStatus Failure" << endl;
			closesocket(clientInfo->socket);
			free(clientInfo);
			free(eventSocket);
			return;
		}

		eventSocket->dataBuffer.len = recieveBytes;

		if (recieveBytes == 0)
		{
			closesocket(clientInfo->socket);
			free(clientInfo);
			free(eventSocket);
			return;
		}
		else
		{
			cout << "TRACE - Receive message : " << eventSocket->dataBuffer.buf << " (" << eventSocket->dataBuffer.len << " bytes)" << endl;

			if (WSASend(clientInfo->socket, &(eventSocket->dataBuffer), 1, &sendBytes, 0, NULL, NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					cout << "ERROR - Fail WSASend(error_code : " << WSAGetLastError() << endl;
				}
			}

			cout << "TRACE - Send message : " << eventSocket->dataBuffer.buf << " (" << eventSocket->dataBuffer.len << " bytes)" << endl;

			memset(eventSocket->messageBuffer, 0, MAX_LEN);
			eventSocket->receiveBytes = 0;
			eventSocket->sendBytes = 0;
			eventSocket->dataBuffer.len = MAX_LEN;
			eventSocket->dataBuffer.buf = eventSocket->messageBuffer;
			flags = 0;

			if (WSARecv(clientInfo->socket, &(eventSocket->dataBuffer), 1, &recieveBytes, &flags, &(eventSocket->overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					cout << "ERROR - Fail WSARecv(error_code : " << WSAGetLastError() << endl;
				}
			}
		}
	}
}