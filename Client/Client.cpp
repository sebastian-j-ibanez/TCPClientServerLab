//
// By Sebastian IBanez
// 22-01-2023
//

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>


using namespace std;

int main()
{
	char ESCCHAR = '\x03';
	char QUIT = '\x04';
	char BYE = '\x1b';

	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 0;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(ClientSocket);
		WSACleanup();
		return 0;
	}

	//Send txt file to server char by char
	char fileBuffer;
	char* TxBuffer = new char;
	char RxBuffer[128] = {};
	char InputBuffer[128] = {};

	bool continueLoop = true;
	while (continueLoop == true)
	{
		memset(InputBuffer, NULL, sizeof(InputBuffer));

		cout << "Enter the name of a text file to transmit" << endl;
		cin >> InputBuffer;

		if (strcmp(InputBuffer, "quit") == 0)
		{
			TxBuffer = &QUIT;
			continueLoop = false;
		}
		else if (strcmp(InputBuffer, "bye") == 0)
		{
			TxBuffer = &BYE;
			continueLoop = false;
		}
		else
		{
			ifstream file;
			file.open(InputBuffer, ios::binary);

			if (!file.is_open())
			{
				cout << "Error opening file." << endl;
				return 0;
			}

			while (!file.eof())
			{
				file.get(fileBuffer);
				TxBuffer = &fileBuffer;

				send(ClientSocket, TxBuffer, sizeof(TxBuffer), 0);
				recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
				cout << RxBuffer << endl;

				file.peek();
			}

			file.close();
			TxBuffer = &ESCCHAR;
		}
		send(ClientSocket, TxBuffer, sizeof(TxBuffer), 0);
		recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
		cout << RxBuffer << endl;
	}

	closesocket(ClientSocket);	//Close connection and socket
	WSACleanup();				//Free Winsock DLL resources

	return 1;
}