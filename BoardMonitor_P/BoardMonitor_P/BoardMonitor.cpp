#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>  
#include <iostream>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#define SERVER_PORT  2306

using namespace std;

const int CREATE_PROCESS_ERROR = 1;
const int WSA_STARTUP_FAIL = 2;
const int SOCKET_FAIL = 3;
const int BIND_FAIL = 4;
const int LISTEN_FAIL = 5;
const int ACCEPT_FAIL = 6;
const int RECV_ERROR = 7;
const int CAR_STOP_WHEN_NOT_STARTED = 8;

int main() {
	try {
		//server initial
		WORD wVersionRequested;
		WSADATA wsaData;
		int ret, nLeft, length;
		SOCKET sListen, sServer;
		struct sockaddr_in saServer, saClient;
		char *ptr;
		wVersionRequested = MAKEWORD(2, 2);
		ret = WSAStartup(wVersionRequested, &wsaData);
		if (ret != 0) {
			throw WSA_STARTUP_FAIL;
		}

		sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sListen == INVALID_SOCKET) {
			WSACleanup();
			throw SOCKET_FAIL;
		}

		saServer.sin_family = AF_INET;
		saServer.sin_port = htons(SERVER_PORT);
		saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		ret = bind(sListen, (struct sockaddr*)&saServer, sizeof(saServer));
		if (ret == SOCKET_ERROR) {
			closesocket(sListen);
			WSACleanup();
			throw BIND_FAIL;
		}

		ret = listen(sListen, 5);
		if (ret == SOCKET_ERROR) {
			closesocket(sListen);
			throw LISTEN_FAIL;
		}
		
		cout << "Server listen at " << SERVER_PORT;
		//server run
		while (1) {
			length = sizeof(saClient);
			sServer = accept(sListen, (struct sockaddr*)&saClient, &length);
			if (sServer == INVALID_SOCKET) {
				closesocket(sListen);
				WSACleanup();
				throw ACCEPT_FAIL;
			}
			char sendMessage[] = "request received";
			send(sServer, sendMessage, strlen(sendMessage) + 1, 0);

			char receiveMessage[5000];
			nLeft = sizeof(receiveMessage);
			ptr = (char*)&receiveMessage;

			while (nLeft > 0) {
				ret = recv(sServer, ptr, 5000, 0);
				if (ret == SOCKET_ERROR) {
					throw RECV_ERROR;
				}
				if (ret == 0) {
					break;
				}
				nLeft -= ret;
				ptr += ret;
			}
			string statusSetStr = string(receiveMessage);

			//update graph

		}
	}
	catch (int e) {
		cout << "(Errorcode(" << e << ")\n";
		exit(1);
	}
	catch (...) {
		cout << GetLastError();
		exit(1);
	}
}