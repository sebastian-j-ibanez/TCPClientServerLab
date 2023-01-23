//
// By Sebastian IBanez
// 22-01-2023
//

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>

#define FILENAME "Rx.txt"

using namespace std;

int main()
{
	char ESCCHAR = '\x03';
	char QUIT = '\x04';
	char BYE = '\x1b';

	//Start Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//Create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 0;
	}

	//Bind socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	//Listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}
	cout << "Waiting for client connection\n" << endl;

	//Accept a connection from a client in a loop
	SOCKET ConnectionSocket;

	char* RxBuffer = new char;
	char TxBuffer[] = { "Received Message" };

	bool connectionLoop = true;
	while (connectionLoop == true)
	{

		ConnectionSocket = SOCKET_ERROR;
		if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR)
		{
			closesocket(ServerSocket);
			WSACleanup();
			return 0;
		}
		cout << "Connection Established" << endl;

		bool recvLoop = true;
		while (recvLoop == true)
		{
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			send(ConnectionSocket, TxBuffer, sizeof(TxBuffer), 0);

			char fileBuffer = *RxBuffer;

			if (fileBuffer == QUIT)
			{
				cout << "Quitting..." << endl;
				recvLoop = false;
				connectionLoop = false;
			}
			else if (fileBuffer == BYE)
			{
				cout << "Client has disconnected..." << endl;
				recvLoop = false;
			}
			else if (fileBuffer == ESCCHAR)
			{
				cout << "End of File..." << endl;
			}
			else
			{
				cout << "Msg Rx: " << fileBuffer << endl;

				ofstream file;
				file.open(FILENAME, ofstream::app);

				if (!file.is_open())
				{
					cout << "Error opening file." << endl;
					return 0;
				}

				file << fileBuffer;
				file.close();
			}
		}
	}

	closesocket(ConnectionSocket);	//Close incoming socket
	closesocket(ServerSocket);	    //Close server socket	
	WSACleanup();					//Free Winsock resources

	return 1;
}