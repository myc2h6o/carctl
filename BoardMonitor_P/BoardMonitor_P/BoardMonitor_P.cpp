#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

const int WSA_STARTUP_FAIL = 1;
const int INVALID_WNISOCK_VERSION = 2;
const int SOCKET_FAIL = 3;
const int FILE_BUFFER_SIZE = 512;

#define SERVER_PORT 2306

int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKET sClient;
	char recvBuf[FILE_BUFFER_SIZE];

	try {
		int err;
		wVersionRequested = MAKEWORD(2, 2);
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			throw WSA_STARTUP_FAIL;
		}

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
			WSACleanup();
			throw INVALID_WNISOCK_VERSION;
		}

		sClient = socket(AF_INET, SOCK_DGRAM, 0);
		if (sClient == INVALID_SOCKET) {
			WSACleanup();
			throw SOCKET_FAIL;
		}

		SOCKADDR_IN  addrServ;
		addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		addrServ.sin_family = AF_INET;
		addrServ.sin_port = htons(SERVER_PORT);

		bind(sClient, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));

		SOCKADDR_IN  addrClient;
		int length = sizeof(SOCKADDR);
		while (1) {
			int ret = recvfrom(sClient, recvBuf, FILE_BUFFER_SIZE, 0, (SOCKADDR*)&addrClient, &length);
			//[TODO] add process here
			printf("%d %s\n", ret, recvBuf);
		}
		closesocket(sClient);
		WSACleanup();
		return 0;
	}
	catch (int e) {
		closesocket(sClient);
		WSACleanup();
		cout << "Something wrong(Error Code " << e << ")\n";
		system("pause");
		exit(1);
	}
	catch (...) {
		closesocket(sClient);
		WSACleanup();
		cout << "Unknown Error!\n";
		system("pause");
		exit(1);
	}
}

