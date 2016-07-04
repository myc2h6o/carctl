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
const int CONNECT_FAIL = 4;
const int SEND_FAIL = 5;
const int UPLOAD_FLE_NOT_FOUND = 6;
const int FILE_BUFFER_SIZE = 512;

char file_send_buffer[FILE_BUFFER_SIZE];

int main() {
	int SERVER_PORT = 2306;
	string NET_ADDR = "";
	string command = "";
	//client variables
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
	SOCKET sClient;
	struct sockaddr_in saServer;
	char *ptr;
	BOOL fSuccess = TRUE;
	char commandStr[32];
	try {
		//get host
		cout << "host address: ";
		cin >> NET_ADDR;
		cout << "port: ";
		cin >> SERVER_PORT;
		
		
		//send command
		while (1) {//client init
			wVersionRequested = MAKEWORD(2, 2);
			ret = WSAStartup(wVersionRequested, &wsaData);
			if (ret != 0) {
				throw WSA_STARTUP_FAIL;
			}

			if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
				WSACleanup();
				throw INVALID_WNISOCK_VERSION;
			}

			sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (sClient == INVALID_SOCKET) {
				WSACleanup();
				throw SOCKET_FAIL;
			}

			saServer.sin_family = AF_INET;
			saServer.sin_port = htons(SERVER_PORT);
			saServer.sin_addr.S_un.S_addr = inet_addr(NET_ADDR.c_str());
			cout << "command: upload/start_stay/start_forward/stop/shutdown/quit:\n";
			//get command
			cin >> command;
			if (command == "quit") {
				exit(0);
			}
			else if (command == "start_stay")
			{
				command = "START_CAR_STAY";
			}
			else if (command == "start_forward")
			{
				command = "START_CAR_FORWARD";
			}
			else if (command == "stop")
			{
				command = "STOP_CAR";
			}
			else if (command == "shutdown") {
				command = "SHUT_DOWN";
			}
			else if (command == "upload") {
				command = "UPLOAD_CAR_CTL";
			}
			else {
				cout << "wrong command, ";
				continue;
			}

			//connect
			ret = connect(sClient, (struct sockaddr*)&saServer, sizeof(saServer));
			if (ret == SOCKET_ERROR) {
				closesocket(sClient);
				WSACleanup();
				throw CONNECT_FAIL;
			}

			//send command
			sprintf(commandStr, "%s", command.c_str());
			ret = send(sClient, (char*)&commandStr, sizeof(commandStr), 0);
			if (ret == SOCKET_ERROR) {
				throw SEND_FAIL;
			}
			else {
				cout << "command sent\n";
				char recvBuf[100];
				recv(sClient, recvBuf, 100, 0);
				printf("%s\n", recvBuf);
				closesocket(sClient);
				WSACleanup();

				if (command == "UPLOAD_CAR_CTL") {
					//upload file
					wVersionRequested = MAKEWORD(2, 2);
					ret = WSAStartup(wVersionRequested, &wsaData);
					if (ret != 0) {
						throw WSA_STARTUP_FAIL;
					}

					if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
						WSACleanup();
						throw INVALID_WNISOCK_VERSION;
					}

					sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (sClient == INVALID_SOCKET) {
						WSACleanup();
						throw SOCKET_FAIL;
					}

					saServer.sin_family = AF_INET;
					saServer.sin_port = htons(SERVER_PORT);
					saServer.sin_addr.S_un.S_addr = inet_addr(NET_ADDR.c_str());
					//connect
					ret = connect(sClient, (struct sockaddr*)&saServer, sizeof(saServer));
					if (ret == SOCKET_ERROR) {
						closesocket(sClient);
						WSACleanup();
						throw CONNECT_FAIL;
					}
					FILE *fp = NULL;
					fp = fopen("CarCtl_M.exe", "rb");
					if (fp == NULL) {
						throw UPLOAD_FLE_NOT_FOUND;
					}
					while (!feof(fp)) {
						ret = fread(file_send_buffer, 1, FILE_BUFFER_SIZE, fp);
						ret = send(sClient, (char*)&file_send_buffer, ret, 0);
						if (ret == SOCKET_ERROR) {
							throw SEND_FAIL;
						}
					}
					char recvBuf[100];
					recv(sClient, recvBuf, 100, 0);
					closesocket(sClient);
					WSACleanup();
					cout << "upload success\n";
				}
			}
		}
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