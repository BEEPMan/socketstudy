//#pragma comment(lib,"ws2_32")
//#include<iostream>
//#include<WinSock2.h>
//#include<WS2tcpip.h>
//#include<memory.h>
//#include<cstring>
//
//#define MAX_LEN 512
//#define SERVER_PORT 23000
//#define SERVER_IP "127.0.0.1"
//
//using namespace std;
//
//int main()
//{
//	WSADATA WSAData;
//	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
//	{
//		MessageBox(NULL, L"Can not load 'winsock.dll' file", L"ERROR", MB_OK | MB_ICONERROR);
//		return 1;
//	}
//
//	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	if (clientSocket == INVALID_SOCKET)
//	{
//		MessageBox(NULL, L"Invalid socket", L"ERROR", MB_OK | MB_ICONERROR);
//		WSACleanup();
//		return 1;
//	}
//
//	SOCKADDR_IN serverAddr;
//	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_port = htons(SERVER_PORT);
//	inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr.S_un.S_addr));
//
//	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
//	{
//		MessageBox(NULL, L"Connect failed", L"ERROR", MB_OK | MB_ICONERROR);
//		closesocket(clientSocket);
//		WSACleanup();
//		return 1;
//	}
//	cout << "Connect Success" << endl;
//
//	char messageBuffer[MAX_LEN];
//	int bufLen;
//	int sendBufLen;
//	int recvBufLen;
//	while (1)
//	{
//		cout << "Send message: ";
//		cin.getline(messageBuffer, MAX_LEN, '\n');
//		if (cin.fail())
//		{
//			cin.clear();
//			cin.ignore(LLONG_MAX, '\n');
//		}
//		bufLen = strlen(messageBuffer) + 1;
//		sendBufLen = send(clientSocket, messageBuffer, bufLen, 0);
//		if (sendBufLen > 0)
//		{
//			recvBufLen = recv(clientSocket, messageBuffer, MAX_LEN, 0);
//			if (recvBufLen > 0)
//			{
//				cout << "Receive message: " << messageBuffer << endl;
//			}
//		}
//	}
//
//	closesocket(clientSocket);
//	WSACleanup();
//}