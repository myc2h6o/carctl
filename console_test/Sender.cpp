#include "Sender.h""
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDR "192.168.1.100"
//#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT  2306

const int WSA_STARTUP_FAIL = 2;
const int SOCKET_FAIL = 3;
const int MAX_BUFFER_SIZE = 512;
char buffer[MAX_BUFFER_SIZE];

//server
void Sender::send(char *data, int len) {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	try {
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

		char *ptr = buffer;
		while(len >= MAX_BUFFER_SIZE){
			sendto(sockClient, ptr, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
			len -= MAX_BUFFER_SIZE;
			ptr += MAX_BUFFER_SIZE;
		}
		if (len > 0) {
			sendto(sockClient, ptr, len, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
		}

		closesocket(sockClient);
		WSACleanup();
	}
	catch (int e) {
		cout << "(Errorcode(" << e << ")\n";
	}
	catch (...) {
		cout << "Unknown Error\n";
	}

}