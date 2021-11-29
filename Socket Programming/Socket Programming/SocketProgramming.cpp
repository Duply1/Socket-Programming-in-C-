#pragma comment(lib, "ws2_32.lib")

#include<iostream>
#include<winsock.h>

using namespace std;
#define PORT 9909



struct sockaddr_in srv;
fd_set fr, fw, fe;
int nMaxFd;
int nSocket;
int nArrClient[5];

char message[255] = "Got the message";


void ProcessNewMessageRequest(int nClientSocket) {
	char buff[256 + 1] = { 0, };
	int nRet = recv(nClientSocket, buff, 256, 0);
	if(nRet < 0) {
		cout << endl << "Something is wrong!!";
		closesocket(nClientSocket);
		for (int nIndex = 0; nIndex < 5; nIndex++) {
			if (nArrClient[nIndex] == nClientSocket) {
				nArrClient[nIndex] = 0;
				break;
			}
		}
	}
	else {
		cout << endl << "The message received from client is: " << buff;
		//response to client
		send(nClientSocket, "Processed your request", 23, 0);
		
	}
}


void ProcessNewRequest() {
	if (FD_ISSET(nSocket, &fr)) {
		int nLen = sizeof(struct sockaddr);
		int nClientSocket = accept(nSocket, NULL, &nLen);
		cout << endl << "Client accepted server";
		if (nClientSocket > 0) {
			int nIndex;
			for (nIndex = 0; nIndex < 5; nIndex++) {
				if (nArrClient[nIndex] == 0 ) {
					nArrClient[nIndex] = nClientSocket;
					send(nClientSocket, "Got the connection done successfully", 37, 0);
					break;
				}
			}
			if (nIndex == 5) {
				cout << endl << " No Space availabel";
			}
		}
	}
	else {
		for (int nIndex = 0; nIndex < 5; nIndex++) {
			if (FD_ISSET(nArrClient[nIndex], &fr)) {
				//new message from the client 
				//then queuing that 
				ProcessNewMessageRequest(nArrClient[nIndex]);
			}
		}
	}
}

int main() {

	int nRet = 0;

	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
		cout << endl << "WSA Failed";
	}
	else {
		cout << endl << "WSA initialized";
	}


	nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nSocket < 0) {
		cout << endl << "Not opened";
		WSACleanup();
		exit(EXIT_FAILURE);

	}
	else {
		cout << endl << "Opened"<<nSocket;
	}


	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORT);
	srv.sin_addr.s_addr = inet_addr("127.0.0.1"); // if you want to use the ip address directly
	//srv.sin_addr.s_addr = INADDR_ANY;
	memset(&(srv.sin_zero), 0, 8);

	int nOptval = 0;
	int nOptLen = sizeof(nOptval);
	nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptval, nOptLen);
	if (!nRet) {
		cout << endl << "setsock succesfull";
	}
	else {
		cout << endl << "setcok unsucessfull";
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	u_long optval = 0; // 1 for non-blocking socket or else blocking socket
	nRet = ioctlsocket(nSocket, FIONBIO, &optval);
	if (nRet != 0) {
		cout << endl << "Ioctlsocket failed";
	}
	else {
		cout << endl << "Succesfully call passed Ioctlsocket";
	}



	nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
	if (nRet < 0) {
		cout << endl << "Fail to bind";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << endl << "Successfully bind";
	}



	nRet = listen(nSocket, 5);  //multi clients

	if (nRet < 0) {
		cout << endl << "Fail to start listen";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << endl << "Successfully started listen";
	}

	nMaxFd = nSocket;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;


	while (1) {
		FD_ZERO(&fr);
		FD_ZERO(&fw);
		FD_ZERO(&fe);


		FD_SET(nSocket, &fr);
		FD_SET(nSocket, &fe);


		for (int nIndex = 0; nIndex < 5; nIndex++) {
			if (nArrClient[nIndex] != 0) {
				FD_SET(nArrClient[nIndex], &fr);
				FD_SET(nArrClient[nIndex], &fe);
			}
		}

		//cout << endl << "Be fore select call: " << fr.fd_count;


		//nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
		if (nRet > 0) {
			//When someone connects, or communicates with a message 
			//over a dedicated connection
			cout << endl << "Data on port";

			

			ProcessNewRequest();
			//break;
		}
		else if (nRet == 0) {
			//No connections or no new request made 
			//none of the scoket descriptors are ready
			//cout << endl << "Nothing in port " << PORT;

		}
		else {
			//it failed and your application show ssome useful message 
			WSACleanup();
			exit(EXIT_FAILURE);
		}

		//cout << endl << "After select call: " << fr.fd_count;
		
	}
}
