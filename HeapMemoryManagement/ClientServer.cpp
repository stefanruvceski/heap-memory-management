#include"ClientServer.h"
#define DEFAULT_BUFLEN 512

/*
#pragma region staro
int Select(fd_set* set, int send)
{
	timeval timeVal;
	int iResult;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	if (send == 1)
	{
		iResult = select(0, NULL, set, NULL, &timeVal);
	}
	else
	{
		iResult = select(0, set, NULL, NULL, &timeVal);
	}

	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
		return iResult;
	}

	if (iResult == 0)
	{
		return iResult;
	}
	else if (iResult > 0)
	{
		return iResult;
	}

	return 0;
}

int innerReceive(SOCKET socket, char* buff, int buffLen, int recvLen) {

	int iResult = 0;

	int i = 0;

	while (iResult != buffLen)
	{
		i++;
		iResult += recv(socket, buff, recvLen, 0);
		if (i == 10000) {
			printf("Received %.2f MB\n", ((float)iResult) / 1000000.0);
			i = 0;
		}
	}
	printf("Received %.2f MB\n", ((float)iResult) / 1000000.0);

	return iResult;
}
int Receive(SOCKET socket, char* buff, int buffLen) {

	char* buff1 = (char*)malloc(sizeof(int));

	innerReceive(socket, buff1, sizeof(int), sizeof(int));

	printf("Bytes received: %d\n", *(int*)buff1);

	return innerReceive(socket, buff, *(int*)buff1, DEFAULT_BUFLEN);

}

int innerSend(SOCKET socket, char* buff, int buffLen) {

	int iResult = 0;

	while (iResult != buffLen)
	{
		iResult += send(socket, buff, buffLen, 0);
	}

	return iResult;
}
int Send(SOCKET socket, char* buff, int buffLen) {

	int iResult1 = 0, iResult2 = 0;


	innerSend(socket, (char*)&buffLen, sizeof(int));


	return innerSend(socket, buff, buffLen);

}
#pragma endregion
*/



int Send(SOCKET connectSocket, char *messageToSend, int length)
{
	int i = 0;
	int sendLength = 0;
	char *pomBuffer = messageToSend;
	int iResult;


	while (sendLength < length)
	{
		iResult = _select(connectSocket, 1);
		if (iResult == -1)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		i = send(connectSocket, pomBuffer, length - sendLength, 0);


		if (i < 1)
		{
			return -1;
		}
		pomBuffer += i;
		sendLength += i;
	}

	return i;
}
int Recv(SOCKET acceptedSocket, char *recvBuffer, int buffLength)
{
	int i;
	char *pomBuffer = recvBuffer;
	int iResult;


	while (buffLength > 0)
	{
		iResult = _select(acceptedSocket, 0);
		if (iResult == -1)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			closesocket(acceptedSocket);
			WSACleanup();
			return iResult;
		}
		else if (iResult > 0) {
			i = recv(acceptedSocket, pomBuffer, buffLength, 0);
			//printf("Server je primio : %d", i);
			if (i < 0)
			{
				return -1;
			}

			pomBuffer += i;
			buffLength -= i;
		}
		else {
			return -2;
		}

	}
	return i;
}

int _select(SOCKET socket, int send)
{
	FD_SET set;
	timeval timeVal;
	int iResult;

	do
	{
		FD_ZERO(&set);
		// Add socket we will wait to read from
		FD_SET(socket, &set);

		// Set timeouts to zero since we want select to return
		// instantaneously
		timeVal.tv_sec = 1;
		timeVal.tv_usec = 0;

		if (send == 1)
		{
			iResult = select(0 /* ignored */, NULL, &set, NULL, &timeVal);
		}
		else
		{
			iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
		}


		// lets check if there was an error during select
		if (iResult == SOCKET_ERROR)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			return iResult;
		}

		// now, lets check if there are any sockets ready
		if (iResult == 0)
		{
			// there are no ready sockets, sleep for a while and check again
			//Sleep(SERVER_SLEEP_TIME);
			return iResult;
		}
		else if (iResult > 0)
		{
			return iResult;
		}

	} while (iResult == 0);

	return iResult;
}
int _send(SOCKET socket, char* buff, int size) {
	int iResult;
	iResult = Send(socket, (char*)&size, sizeof(int));
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
	}
	else {
		iResult = Send(socket, buff, size);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			WSACleanup();
		}
	}
	return iResult;
}
int _recv(SOCKET socket, char* buff) {
	int iResult, size;

	iResult = Recv(socket, (char*)&size, sizeof(int));
	if (iResult > 0)
	{
		//printf("Message received from client[]: %d.\n", (size));
		iResult = Recv(socket, buff, size);
		if (iResult > 0)
		{
			buff[size] = '\0';
			printf("Message received from client: %s.\n", (buff));
		}
		else if (iResult == 0)
		{
			// connection was closed gracefully
			printf("Connection with client closed.\n");
			closesocket(socket);
		}
		else if (iResult == -1)
		{
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
		}
	}
	else if (iResult == 0)
	{
		// connection was closed gracefully
		printf("Connection with client closed.\n");
		closesocket(socket);
	}
	else if (iResult == -1)
	{
		// there was an error during recv
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
	}
	return iResult;
}
