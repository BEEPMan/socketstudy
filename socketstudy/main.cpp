#pragma comment(lib,"ws2_32")
#include<WinSock2.h>

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	MessageBox(NULL, L"윈도우 초기화 성공", L"알림", MB_OK);

	WSACleanup();
	return 0;
}