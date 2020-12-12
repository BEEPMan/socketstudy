//#include<iostream>
//#include<tchar.h>
//#include<winsock2.h>
//
//#pragma comment(lib, "Ws2_32.lib")
//
//#define MAX_BUFFER        1024
//#define SERVER_PORT        3500
//
//struct SOCKETINFO
//{
//    WSAOVERLAPPED overlapped;
//    WSABUF dataBuffer;
//    SOCKET socket;
//    char messageBuffer[MAX_BUFFER];
//    int receiveBytes;
//    int sendBytes;
//};
//
//DWORD WINAPI makeThread(LPVOID hIOCP);
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//    WSADATA WSAData;
//    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
//    {
//        printf("Error - Can not load 'winsock.dll' file\n");
//        return 1;
//    }
//
//    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//    if (listenSocket == INVALID_SOCKET)
//    {
//        printf("Error - Invalid socket\n");
//        return 1;
//    }
//    printf("Socket Created\n");
//
//    SOCKADDR_IN serverAddr;
//    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
//    serverAddr.sin_family = PF_INET;
//    serverAddr.sin_port = htons(SERVER_PORT);
//    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//
//    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
//    {
//        printf("Error - Fail bind\n");
//        closesocket(listenSocket);
//        WSACleanup();
//        return 1;
//    }
//    printf("Bind Success\n");
//
//    if (listen(listenSocket, 5) == SOCKET_ERROR)
//    {
//        printf("Error - Fail listen\n");
//        closesocket(listenSocket);
//        WSACleanup();
//        return 1;
//    }
//    printf("Listen Success\n");
//
//    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    SYSTEM_INFO systemInfo;
//    GetSystemInfo(&systemInfo);
//    int threadCount = systemInfo.dwNumberOfProcessors * 2;
//    unsigned long threadId;
//    HANDLE* hThread = (HANDLE*)malloc(threadCount * sizeof(HANDLE));
//    for (int i = 0; i < threadCount; i++)
//    {
//        hThread[i] = CreateThread(NULL, 0, makeThread, &hIOCP, 0, &threadId);
//    }
//
//    SOCKADDR_IN clientAddr;
//    int addrLen = sizeof(SOCKADDR_IN);
//    memset(&clientAddr, 0, addrLen);
//    SOCKET clientSocket;
//    SOCKETINFO* socketInfo;
//    DWORD receiveBytes;
//    DWORD flags;
//
//    while (1)
//    {
//        clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
//        printf("Accept Success\n");
//        if (clientSocket == INVALID_SOCKET)
//        {
//            printf("Error - Accept Failure\n");
//            return 1;
//        }
//
//        socketInfo = (struct SOCKETINFO*)malloc(sizeof(struct SOCKETINFO));
//        memset((void*)socketInfo, 0x00, sizeof(struct SOCKETINFO));
//        socketInfo->socket = clientSocket;
//        socketInfo->receiveBytes = 0;
//        socketInfo->sendBytes = 0;
//        socketInfo->dataBuffer.len = MAX_BUFFER;
//        socketInfo->dataBuffer.buf = socketInfo->messageBuffer;
//        flags = 0;
//
//        hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)socketInfo, 0);
//
//        if (WSARecv(socketInfo->socket, &socketInfo->dataBuffer, 1, &receiveBytes, &flags, &(socketInfo->overlapped), NULL))
//        {
//            if (WSAGetLastError() != WSA_IO_PENDING)
//            {
//                printf("Error - IO pending Failure\n");
//                return 1;
//            }
//        }
//    }
//
//    closesocket(listenSocket);
//
//    WSACleanup();
//
//    return 0;
//}
//
//DWORD WINAPI makeThread(LPVOID hIOCP)
//{
//    HANDLE threadHandler = *((HANDLE*)hIOCP);
//    DWORD receiveBytes;
//    DWORD sendBytes;
//    DWORD completionKey;
//    DWORD flags;
//    struct SOCKETINFO* eventSocket;
//    while (1)
//    {
//        if (GetQueuedCompletionStatus(threadHandler, &receiveBytes, (PULONG_PTR)&completionKey, (LPOVERLAPPED*)&eventSocket, INFINITE) == 0)
//        {
//            printf("Error - GetQueuedCompletionStatus Failure\n");
//            closesocket(eventSocket->socket);
//            free(eventSocket);
//            return 1;
//        }
//
//        eventSocket->dataBuffer.len = receiveBytes;
//
//        if (receiveBytes == 0)
//        {
//            closesocket(eventSocket->socket);
//            free(eventSocket);
//            break;
//        }
//        else
//        {
//            printf("TRACE - Receive message : %s (%d bytes)\n", eventSocket->dataBuffer.buf, eventSocket->dataBuffer.len);
//
//            if (WSASend(eventSocket->socket, &(eventSocket->dataBuffer), 1, &sendBytes, 0, NULL, NULL) == SOCKET_ERROR)
//            {
//                if (WSAGetLastError() != WSA_IO_PENDING)
//                {
//                    printf("Error - Fail WSASend(error_code : %d)\n", WSAGetLastError());
//                }
//            }
//
//            printf("TRACE - Send message : %s (%d bytes)\n", eventSocket->dataBuffer.buf, eventSocket->dataBuffer.len);
//
//            memset(eventSocket->messageBuffer, 0x00, MAX_BUFFER);
//            eventSocket->receiveBytes = 0;
//            eventSocket->sendBytes = 0;
//            eventSocket->dataBuffer.len = MAX_BUFFER;
//            eventSocket->dataBuffer.buf = eventSocket->messageBuffer;
//            flags = 0;
//
//            if (WSARecv(eventSocket->socket, &(eventSocket->dataBuffer), 1, &receiveBytes, &flags, &eventSocket->overlapped, NULL) == SOCKET_ERROR)
//            {
//                if (WSAGetLastError() != WSA_IO_PENDING)
//                {
//                    printf("Error - Fail WSARecv(error_code : %d)\n", WSAGetLastError());
//                }
//            }
//        }
//    }
//    return 0;
//}