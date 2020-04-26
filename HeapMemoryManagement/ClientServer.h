#pragma once
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>

int _send(SOCKET socket, char* buff, int size);
int _recv(SOCKET socket, char* buff);
int _select(SOCKET socket, int send);
