#pragma comment(lib, "ws2_32.lib")

#include<iostream>
#include<winsock.h>


using namespace std;

#define PORT 9909

int nClientSocket;
struct sockaddr_in srv;


int main() {

	int nRet = 0;
	WSADATA ws;

	if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
		cout << endl << "WSA Failed";
		WSACleanup();
		exit(EXIT_FAILURE);

	}
	

	nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (nClientSocket < 0) {
		cout << endl << "socket call failed";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORT);
	srv.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(&(srv.sin_zero), 0, 8);


	nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
	if (nRet < 0) {
		cout << endl << "connect failed";
		WSACleanup();
		return (EXIT_FAILURE);
	}
	else {
		cout << endl << "Connected to the client ";
		char buff[255] = { 0, };
		recv(nClientSocket, buff, 255, 0); //blocking call
		
		cout << endl << buff;
	}

}