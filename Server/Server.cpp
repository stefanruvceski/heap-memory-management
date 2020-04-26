#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "..\HeapMemoryManagement\ClientMemory.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "conio.h"
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include"..\HeapMemoryManagement\ClientServer.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_PORT 27016
#define BUFFER_SIZE 256

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define SERVER_SLEEP_TIME 50

int ServerTest(TMasterHeapManager* masterHeapManager);
DWORD WINAPI fun1(LPVOID lpParam);
DWORD WINAPI fun2(LPVOID lpParam);
DWORD WINAPI fun3(LPVOID lpParam);
void BigDataTest();
void PartitionTestWithoutThread();
void PartitionTestWithThread();



int main() 
{

#pragma region Init
	// Inicijalizacija MasterHeapManagera
	TMasterHeapManager* masterHeapManager = NULL;
	_initMasterHeap(&masterHeapManager, 2);

#pragma endregion
	// TESTOVI
	//1
	ServerTest(masterHeapManager);
	
	//2
	//BigDataTest();
	
	//3
	//PartitionTestWithoutThread();
	
	//4
	//PartitionTestWithThread();

	getch();

	_print(masterHeapManager);
	
	printf("");

	_deinitMasterHeap(masterHeapManager);
	
	printf("");

	system("pause");

    return 0;
}

#pragma region Tests

void BigDataTest() {
	// TEST
	//alokacija 8192 blokova od 128KB
	void* addrs[8192];
	clock_t t = clock();
	for (int i = 0; i < 8192; i++)
	{
		addrs[i] = _alloc((int)pow(2, 17));
	}

	for (int i = 0; i < 8192; i++)
	{
		_free(addrs[i]);
	}
	t = clock() - t;

	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Vreme izvrsavanja BigDataTestWithoutThread funkcije je %f sekundi\n", time_taken);
}

void PartitionTestWithoutThread() {
	//TEST
	//Alokacija 1GB memorije preko particija od 32KB (32KB*32768 = 1GB)
	void* addrs[32768];
	clock_t t = clock();
	for (int i = 0; i < (32768); i++)
	{
		addrs[i] = _alloc((unsigned)pow(2, 15));  // PROVERITI ZASTO NAPRAVI NOVI SEGMENT ZA POSLEDNJU ALOKACIJU
	}

	for (int i = 0; i < 32768; i++)
	{
		if(i == 30000)
			_free((void*)addrs[i]);

		_free((void*)addrs[i]);
	}
	t = clock() - t;

	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Vreme izvrsavanja PartitionTestWithoutThread funkcije je %f sekundi\n", time_taken);

}

int ServerTest(TMasterHeapManager* masterHeapManager) {
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;

	// Socket used for communication with client
	SOCKET acceptedSocket[5];

	// Variable used to store function return value
	int iResult, iResultSend;

	// Buffer used for storing incoming data
	char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}


	// Initialize serverAddress structure used by bind
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;				// IPv4 address family
	serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
	serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port


	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol

	// Check if socket is successfully created
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address to socket
	iResult = bind(listenSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

	// Check if socket is successfully binded to address and port from sockaddr_in structure
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Postavljanje soketa u neblokirajuci rezim
	unsigned long mode = 1; //non-blocking mode
	iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR) {
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	// Set listenSocket in listening mode
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	int k = 0;
	bool allocFlag;
	do
	{
		iResult = _select(listenSocket, 0);
		if (iResult == -1)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else if (iResult > 0) {
			acceptedSocket[k] = accept(listenSocket, NULL, NULL);

			if (acceptedSocket[k] == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(acceptedSocket[k]);
				WSACleanup();
				return 1;
			}

			//non blocking
			mode = 1;
			iResult = ioctlsocket(acceptedSocket[k], FIONBIO, &mode);
			if (iResult != NO_ERROR)
				printf("ioctlsocket failed with error: %dld\n", iResult);

			k++;

		}
		else
		{
			for (int i = 0; i < k; i++)
			{
				iResult = _recv(acceptedSocket[i], dataBuffer);
				if (iResult > 0)
				{
					int size = atoi(dataBuffer);

					if (size <= 2 && size > 0) {
						if (size == 1) {
							allocFlag = true;
						}
						else if (size == 2) {
							allocFlag = false;
						}

						memset(dataBuffer, 0, BUFFER_SIZE);
						strcpy_s(dataBuffer, "ACK");
						iResultSend = _send(acceptedSocket[i], dataBuffer, (int)strlen(dataBuffer));
					}
					else if (size > 0) {
						if (allocFlag) {
							printf("Client %d is requesting %d bytes allocated.\n", i + 1, size);
							unsigned int ret = (unsigned int)_alloc(size);
							itoa(ret, dataBuffer, 10);
						}
						else {
							_free((void*)size);
							printf("Client %d is freeing %d allocated address.\n", i + 1, size);
							itoa(size, dataBuffer, 10);
						}
						iResultSend = _send(acceptedSocket[i], dataBuffer, (int)strlen(dataBuffer));
						_print(masterHeapManager);
					}
					else {
						// Connection was closed successfully
						printf("Connection with client %d closed.\n", i + 1);
						closesocket(acceptedSocket[i]);

						for (int j = i; j < k - 1; j++)
						{
							acceptedSocket[j] = acceptedSocket[j + 1];
						}
						acceptedSocket[k - 1] = 0;
						k--;
					}
					continue;
				}
				else if (iResult == 0)	
				{
					printf("Connection with client closed.\n");
					closesocket(acceptedSocket[i]);

					for (int j = i; j < k - 1; j++)
					{
						acceptedSocket[j] = acceptedSocket[j + 1];
					}
					acceptedSocket[k - 1] = 0;
					k--;
				}
				else if(iResult == -1)
				{

					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(acceptedSocket[i]);

					for (int j = i; j < k - 1; j++)
					{
						acceptedSocket[j] = acceptedSocket[j + 1];
					}
					acceptedSocket[k - 1] = 0;
					k--;
				}
				else {
					continue;
				}
			}
		}
		
	} while (strcmp(dataBuffer, "poyy") != 0);

	for (int i = 0; i < k; i++)
	{
		iResult = shutdown(acceptedSocket[i], SD_BOTH);

		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(acceptedSocket[i]);
			WSACleanup();
			return 1;
		}
		closesocket(acceptedSocket[i]);
	}

	//Close listen and accepted sockets
	closesocket(listenSocket);

	printf("graceful shutdown!\n");
	// Deinitialize WSA library
	WSACleanup();

	return 0;
}
HANDLE semaphore2;
void PartitionTestWithThread() {
	semaphore2 = CreateSemaphore(NULL, 0, 1, NULL);
	DWORD fun1ID = NULL, fun2ID = NULL, fun3ID = NULL, fun4ID = NULL;
	HANDLE hfun1 = NULL, hfun2 = NULL, hfun3 = NULL, hfun4 = NULL;

	hfun1 = CreateThread(NULL, 0, &fun1, NULL, 0, &fun1ID);
	hfun2 = CreateThread(NULL, 0, &fun1, NULL, 0, &fun2ID);
	hfun3 = CreateThread(NULL, 0, &fun2, NULL, 0, &fun3ID);
	hfun4 = CreateThread(NULL, 0, &fun3, NULL, 0, &fun4ID);
}

#pragma endregion

#pragma region Functions
int j = 0;
void* addrs[32768];
void partitionHeapFunAlloc() {
	clock_t t = clock();
	for (int i = 0; i < 32768 / 2; i++)
	{
		addrs[j++] = _alloc((unsigned)pow(2, 15));
	}
	t = clock() - t;

	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Vreme izvrsavanja partitionHeapFunAllocHalf funkcije je %f sekundi\n", time_taken);
}

void partitionHeapFunFree1() {
	WaitForSingleObject(semaphore2, 5000);
	ReleaseSemaphore(semaphore2, 1, NULL);
	clock_t t = clock();
	for (int i = 0; i < 32768 / 2; i++)
	{
		_free((void*)addrs[i]);
	}
	t = clock() - t;

	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Vreme izvrsavanja partitionHeapFunFreeHalf funkcije je %f sekundi\n", time_taken);
}

void partitionHeapFunFree2() {
	WaitForSingleObject(semaphore2, 5000);
	ReleaseSemaphore(semaphore2, 1, NULL);
	clock_t t = clock();
	for (int i = 32768 / 2; i < 32768; i++)
	{
		_free((void*)addrs[i]);
	}
	t = clock() - t;

	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Vreme izvrsavanja partitionHeapFunFreeHalf funkcije je %f sekundi\n", time_taken );
}

DWORD WINAPI fun1(LPVOID lpParam) {
	partitionHeapFunAlloc();

	return 0;
}

DWORD WINAPI fun2(LPVOID lpParam) {
	partitionHeapFunFree1();
	
	return 0;
}

DWORD WINAPI fun3(LPVOID lpParam) {
	partitionHeapFunFree2();

	return 0;
}


#pragma endregion