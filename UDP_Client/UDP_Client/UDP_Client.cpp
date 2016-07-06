#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#define SERVER_PORT  2306
#define SERVER_ADDR "127.0.0.1"

const int CREATE_PROCESS_ERROR = 1;
const int WSA_STARTUP_FAIL = 2;
const int SOCKET_FAIL = 3;
const int BIND_FAIL = 4;
const int LISTEN_FAIL = 5;
const int ACCEPT_FAIL = 6;
const int RECV_ERROR = 7;
const int CAR_STOP_WHEN_NOT_STARTED = 8;

//server
int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	try {
		while (1) {
			wVersionRequested = MAKEWORD(2, 2);

			err = WSAStartup(wVersionRequested, &wsaData);
			if (err != 0) {
				throw WSA_STARTUP_FAIL;
			}

			if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
				WSACleanup();
				throw SOCKET_FAIL;
			}

			SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);

			SOCKADDR_IN  addrServ;
			addrServ.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR);
			addrServ.sin_family = AF_INET;
			addrServ.sin_port = htons(SERVER_PORT);
			for (int i = 0; i < 10; ++i) {
				sendto(sockClient, "Hello\n", strlen("Hello\n") + 1, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
			}
			closesocket(sockClient);

			WSACleanup();

			return 0;
		}
	}
	catch (int e) {
		cout << "(Errorcode(" << e << ")\n";
		exit(1);
	}
	catch (...) {
		cout << "Unknown Error\n";
		exit(1);
	}

}