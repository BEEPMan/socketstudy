#pragma comment(lib,"ws2_32")
#include<iostream>
#include<WinSock2.h>
#include<memory.h>
#include<string>
#include<thread>
#include<vector>

#define MAX_LEN 512
#define SERVER_PORT 23000

using namespace std;

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[MAX_LEN];
	int receiveBytes;
	int sendBytes;
};

void sendEcho(LPVOID hIOCP);

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

	HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int threadCount = systemInfo.dwNumberOfProcessors * 2;

	vector<thread> threadPool;
	for (int i = 0; i < threadCount; i++)
	{
		threadPool.emplace_back(thread{ sendEcho, &hIOCP });
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	char readBuffer[MAX_LEN];
	int bufLen;
	SOCKETINFO* socketInfo;
	DWORD receiveBytes;
	DWORD flags;

	while (1)
	{
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			MessageBox(NULL, L"Accept failed", L"ERROR", MB_OK | MB_ICONERROR);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		cout << "Accept Success" << endl;

		socketInfo = (struct SOCKETINFO *)malloc(sizeof(struct SOCKETINFO));
		memset((void *)socketInfo, 0, sizeof(struct SOCKETINFO));
		socketInfo->socket = clientSocket;
		socketInfo->receiveBytes = 0;
		socketInfo->sendBytes = 0;
		socketInfo->dataBuffer.len = MAX_LEN;
		socketInfo->dataBuffer.buf = socketInfo->messageBuffer;
		flags = 0;

		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)socketInfo, 0);

		if (WSARecv(socketInfo->socket, &(socketInfo->dataBuffer), 1, &receiveBytes, &flags, &(socketInfo->overlapped), NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				MessageBox(NULL, L"IO pending failed", L"ERROR", MB_OK | MB_ICONERROR);
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}
		}
	}

	for (auto &t : threadPool)
	{
		t.join();
	}

	closesocket(listenSocket);
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}

void sendEcho(LPVOID hIOCP)
{
	DWORD recieveBytes;
	DWORD sendBytes;
	DWORD completionKey;
	DWORD flags;
	SOCKETINFO* eventSocket;

	while (1)
	{
		if (GetQueuedCompletionStatus(*((HANDLE *) hIOCP), &recieveBytes, (PULONG_PTR)&completionKey, (LPOVERLAPPED *)&eventSocket, INFINITE) == 0)
		{
			cout << "ERROR - GetQueuedCompletionStatus Failure" << endl;
			closesocket(eventSocket->socket);
			free(eventSocket);
			return;
		}

		eventSocket->dataBuffer.len = recieveBytes;

		if (recieveBytes == 0)
		{
			closesocket(eventSocket->socket);
			free(eventSocket);
			return;
		}
		else
		{
			cout << "TRACE - Receive message : " << eventSocket->dataBuffer.buf << " (" << eventSocket->dataBuffer.len << " bytes)" << endl;

			if (WSASend(eventSocket->socket, &(eventSocket->dataBuffer), 1, &sendBytes, 0, NULL, NULL) == SOCKET_ERROR)
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

			if (WSARecv(eventSocket->socket, &(eventSocket->dataBuffer), 1, &recieveBytes, &flags, &(eventSocket->overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					cout << "ERROR - Fail WSARecv(error_code : " << WSAGetLastError() << endl;
				}
			}
		}
	}
}