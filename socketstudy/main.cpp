#pragma comment(lib,"ws2_32")
#include<WinSock2.h>

void err_quit(LPCWSTR msg);

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	MessageBox(NULL, L"윈도우 초기화 성공", L"알림", MB_OK);

	//////////////////////
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit(L"socket()");
	MessageBox(NULL, L"TCP Socket 성공", L"알림", MB_OK);
	closesocket(tcp_sock);
	//////////////////////

	WSACleanup();
	return 0;
}

void err_quit(LPCWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, nullptr);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}