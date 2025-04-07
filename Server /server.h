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
#include <time.h>

#define SOCKET int
#define CLOSESOCKET(s) close(s)
#define ISVALIDSOCKET(s) (s >= 0)
#define ISVALIDMEMORY(s) (s != NULL)

struct client_info *get_client(SOCKET s);
struct client_info *global_client;
SOCKET get_connection(char *host, char *port);
int active_connection(fd_set *master, SOCKET server, SOCKET *max);
int add_client(SOCKET client, struct sockaddr_storage clientinfo);
void free_clients();
void check_for_request(fd_set *reads, fd_set *master);

#endif