#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h> // to get the dns information
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SOCKET int
#define CLOSESOCKET(s) close(s)
#define ISVALIDSOCKET(s) (s >= 0)
#define ISVALIDMEMORY(s) (s != NULL)
#endif