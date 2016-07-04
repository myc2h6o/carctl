#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>  
#include <iostream>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#define SERVER_PORT  2306

using namespace std;

LPCSTR CHILD_PROCESS = "./CarCtl_M.exe";
LPSTR CHILD_PARAMETER_STAY = "./CarCtl_M.exe STAY";
LPSTR CHILD_PARAMETER_FORWARD = "./CarCtl_M.exe FORWARD";


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
		//running info log
		FILE *fp = NULL;
		fp = fopen("BoardCtl.log.txt", "a+");
		if (fp) {
			char timeStr[64];
			time_t currentTime = time(NULL);
			strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
			fprintf(fp, "%s: Server listen at %d\n", timeStr, SERVER_PORT);
			fclose(fp);
			fp = NULL;
		}
		length = sizeof(saClient);
		//server run
		while (1) {
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
					fp = fopen("BoardCtl.log.txt", "a+");
					if (fp) {
						char timeStr[64];
						time_t currentTime = time(NULL);
						strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
						fprintf(fp, "%s: Client disconnected\n", timeStr);
						fclose(fp);
						fp = NULL;
					}
					break;
				}
				nLeft -= ret;
				ptr += ret;
			}
			cout << receiveMessage << endl;
			
			PROCESS_INFORMATION process_info;
			if (strcmp(receiveMessage, "START_CAR_STAY") == 0) {
				fp = fopen("BoardCtl.log.txt", "a+");
				if (fp) {
					char timeStr[64];
					time_t currentTime = time(NULL);
					strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
					fprintf(fp, "%s: Start Car Stay\n", timeStr);
					fclose(fp);
					fp = NULL;
				}
				STARTUPINFO startup_info;
				GetStartupInfo(&startup_info);
				if (!CreateProcess(CHILD_PROCESS, CHILD_PARAMETER_STAY, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &startup_info, &process_info))
				{
					throw CREATE_PROCESS_ERROR;
				}
				
			}else if (strcmp(receiveMessage, "START_CAR_FORWARD") == 0) {
				fp = fopen("BoardCtl.log.txt", "a+");
				if (fp) {
					char timeStr[64];
					time_t currentTime = time(NULL);
					strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
					fprintf(fp, "%s: Start Car Forward\n", timeStr);
					fclose(fp);
					fp = NULL;
				}
				STARTUPINFO startup_info;
				GetStartupInfo(&startup_info);
				if (!CreateProcess(CHILD_PROCESS, CHILD_PARAMETER_FORWARD, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &startup_info, &process_info))
				{
					throw CREATE_PROCESS_ERROR;
				}

			}
			else if (strcmp(receiveMessage, "STOP_CAR") == 0) {
				fp = fopen("BoardCtl.log.txt", "a+");
				if (fp) {
					char timeStr[64];
					time_t currentTime = time(NULL);
					strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
					fprintf(fp, "%s: Stop Car\n", timeStr);
					fclose(fp);
					fp = NULL;
				}
				TerminateProcess(process_info.hProcess, 0);
			}
			else if (strcmp(receiveMessage, "SHUT_DOWN") == 0) {
				system("shutdown /s /t 0");
			}
			else if (strcmp(receiveMessage, "UPLOAD_CAR_CTL") == 0) {
				sServer = accept(sListen, (struct sockaddr*)&saClient, &length);
				if (sServer == INVALID_SOCKET) {
					closesocket(sListen);
					WSACleanup();
					throw ACCEPT_FAIL;
				}
				char sendMessage[] = "request received";
				send(sServer, sendMessage, strlen(sendMessage) + 1, 0);

				char receiveMessage[5000];
				ptr = (char*)&receiveMessage;
				fp = fopen("CarCtl_M.exe", "wb+");
				while (1) {
					ret = recv(sServer, ptr, 5000, 0);
					if (ret == SOCKET_ERROR) {
						fclose(fp);
						fp = NULL;
						throw RECV_ERROR;
					}
					if (ret == 0) {
						fclose(fp);
						fp = NULL;
						break;
					}
					fwrite(ptr, 1, ret, fp);
				}
			}
			else {
				fp = fopen("BoardCtl.log.txt", "a+");
				if (fp) {
					char timeStr[64];
					time_t currentTime = time(NULL);
					strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
					fprintf(fp, "%s: Wrong Command: %s\n", timeStr, receiveMessage);
					fclose(fp);
					fp = NULL;
				}
			}
		}
	}
	catch (int e) {
		FILE *fp = NULL;
		fp = fopen("BoardCtl.log.txt", "a+");
		if (fp) {
			char timeStr[64];
			time_t currentTime = time(NULL);
			strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
			fprintf(fp, "%s:(Errorcode %d) \n", timeStr, e);
			fclose(fp);
			fp = NULL;
		}
		exit(1);
	}
	catch (...) {
		FILE *fp = NULL;
		fp = fopen("BoardCtl.log.txt", "a+");
		if (fp) {
			char timeStr[64];
			time_t currentTime = time(NULL);
			strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %X", localtime(&currentTime));
			fprintf(fp, "%s: Unknown Error\n", timeStr);
			fclose(fp);
			fp = NULL;
		}
		exit(1);
	}
}