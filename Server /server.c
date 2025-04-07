#include "server.h"
#include <signal.h>
#include <assert.h>
#include "parser.h"
char *sendtoclient(char *respond, SOCKET client);
char *recvdata(SOCKET client);

struct client_info
{
    SOCKET client;
    char *host_name; // ip address of the clinet
    char *service;
    time_t client_active_time; // protocol used by the client to join
    struct client_info *next;  // maked the linked_list so that i can track of my connection
};

struct client_info *get_client(SOCKET s)
{
    struct client_info *temp = global_client;
    while (temp)
    {
        if (temp->client == s)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void handle_signal()
{

    printf("\nReceived SIGINT (Ctrl+C). Cleaning up...\n");

    // Close all client sockets
    struct client_info *temp = global_client;
    while (temp)
    {
        CLOSESOCKET(temp->client); // close client sockets
        temp = temp->next;
    }

    // Free the linked list
    free_clients();

    // Close the server socket (if it's global, close it here; else, flag it)
    // Example: CLOSESOCKET(server_socket); if you save it globally

    printf("Server shutdown gracefully.\n");
    exit(0); // Exit cleanly
}
char *sendtoclient(char *respond, SOCKET client)
{
    fprintf(stdout, ".... teh respond sedn to client is %s \n", respond);
    printf("SENDING DATA TO CLIENT .. \n");
    printf("The respond that sendin to client is %s\n", respond);
    int bytes_send = 0;
    int datatobesent = strlen(respond);
    while (datatobesent != bytes_send)
    {
        int datasent = send(client, respond + bytes_send, strlen(respond), 0);
        bytes_send += datasent;
    }
    send(client, "|END", 4, 0);
    fprintf(stdout, "Data sent sucessfully .. \n");
    return "SUCESS";
}
char *recvdata(SOCKET client)
{

    char *mainbuffer = malloc(4018);
    int index = 0;

    if (!VALIDPOINTER(mainbuffer))
    {
        fprintf(stderr, "Error to get the valid pointer to recv data .. \n");
        return "NULL";
    }
    printf("START RECVING DATA FROM THE CLIENT .. \n");
    char temp_recv[1024];
    int data_recv;
    while ((data_recv = recv(client, temp_recv, 1024, 0)) > 0)
    {

        if (strstr(temp_recv, "|END") || strstr(temp_recv, "\r\n\r\n"))
        {
            memcpy(mainbuffer + index, temp_recv, data_recv - 4);
            index += data_recv;
            index -= 4;

            break;
        }
        printf("RECEIVING .. \n");

        if (index + data_recv > 4017)
        {
            fprintf(stderr, "ERROR , request recive out of bound .. \n");
            return "ERROR";
        }

        memcpy(mainbuffer + index, temp_recv, data_recv);
        index += data_recv;
    }
    if (data_recv == 0)
    {
        sprintf(mainbuffer, "%s", "CLIENT CLOSED");
    }
    else if (data_recv < 0)
    {
        sprintf(mainbuffer, "%s", "ERROR");
    }
    else
    {
        printf("RECEVING COMPLETED . \n");
        mainbuffer[index] = '\0';
    }
    printf("TEH REQUEST IS %s \n", mainbuffer);

    return mainbuffer;
}

void check_for_request(fd_set *reads, fd_set *master)
{
    struct client_info *temp = global_client;
    while (temp)
    {

        if (FD_ISSET(temp->client, reads))
        {

            // printf("CLIENT NUMBER : %d \n", temp->client);
            assert(temp != NULL);
            char *buffer = recvdata(temp->client);
            fprintf(stdout, "the buffer is %s \n", buffer);

            if (strcmp(buffer, "CLIENT CLOSED") == 0)
            {
                fprintf(stderr, "SERVER: CLIENT HAVE CLOSE THE SERVER .. \n");
                free(buffer);
                FD_CLR(temp->client, master);
                temp = temp->next;
                continue;
            }
            else if (strcmp(buffer, "ERROR") == 0)
            {
                fprintf(stderr, "SERVER: ERROR IN RECVING DATA FROM SERVER \n");
                free(buffer);
                FD_CLR(temp->client, master);
                temp = temp->next;
                continue;
            }
            else
            {
                printf("CLIENT: %s \n", buffer);

                char *request_respond = data_parser(buffer);
                printf("SERVER RESPOND: %s  ", request_respond);
                // to parse my data
                char *respond = sendtoclient(request_respond, temp->client);
                printf("SERVER RESPOND: %s\n", respond);
            }
            free(buffer);
        }
        temp = temp->next;
    }
}

void free_clients()
{
    struct client_info *temp = global_client;
    struct client_info *delete;
    while (temp)
    {
        delete = temp;
        temp = temp->next;
        free(delete);
        delete = NULL;
    }
}
int add_client(SOCKET client, struct sockaddr_storage client_info)
{
    struct client_info *newclient = malloc(sizeof(*newclient));
    memset(newclient, 0, sizeof(*newclient));
    char address[1024], service[1024];
    if (getnameinfo((struct sockaddr *)&client_info, sizeof(client_info), address,
                    sizeof(address), service, sizeof(service), NI_NUMERICHOST) != 0)
    {
        fprintf(stderr, "ERROR ON GETTING TEH CLIENT IP ADDRESS ... \n");
        return -1;
    }
    newclient->client = client;
    newclient->host_name = address;
    newclient->service = service;
    newclient->next = global_client;
    newclient->client_active_time = time(NULL);
    global_client = newclient;
    return 0;
}
int active_connection(fd_set *master, SOCKET server, SOCKET *max)

{
    printf("Active connection have been callled.. \n");
    struct sockaddr_storage client_info;
    socklen_t client_len = sizeof(client_info);

    SOCKET client = accept(server, (struct sockaddr *)&client_info, &client_len);
    if (client > (*max))
    {
        *max = client;
    }
    if (!ISVALIDSOCKET(client))
    {
        fprintf(stderr, "ERROR ON ACCEPTING THE CONNECTION");
        return -1;
    }
    FD_SET(client, master);
    return add_client(client, client_info);
}
// this will give me socket where i can listen for the new conecytion and accpet tthem
SOCKET get_connection(char *host, char *port)
{
    struct addrinfo hints = {};
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // this help me to use any of  my avialbe address in the service.
    struct addrinfo *server_address;
    printf("Getting server ip address .. \n");
    if (getaddrinfo(host, port, &hints, &server_address) != 0)
    {
        fprintf(stderr, "ERROR: COULDNOT GET THE IP ADDRESS FOR SERVER .. \n");
        freeaddrinfo(server_address);
        return -1;
    }
    printf("Creating the socket... \n");
    SOCKET server = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
    if (!ISVALIDSOCKET(server))
    {
        fprintf(stderr, "ERROR ON CREATING THE SOCKET... \n");
        freeaddrinfo(server_address);
        return -1;
    }
    int option = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    printf("Binding the socket.. \n");
    if (bind(server, server_address->ai_addr, server_address->ai_addrlen) < 0)
    {
        fprintf(stderr, "ERROR ON BINDING THE SOCKET TO SERVER.. \n");
        freeaddrinfo(server_address);
        return -1;
    }
    printf("Listening to the new connection... \n");
    if (listen(server, 10) < 0)
    {
        fprintf(stderr, "ERROR ON LISTENING TO THE SOCKET ....\n");
        freeaddrinfo(server_address);
        return -1;
    }

    freeaddrinfo(server_address);
    return server;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "ERROR: ./server ipadddress port");
        exit(1);
    }
    signal(SIGINT, handle_signal);
    SOCKET connection = get_connection(argv[1], argv[2]);
    if (!ISVALIDSOCKET(connection))
    {
        fprintf(stderr, "ERROR: ISSUE WITH THE CONNECTION\n");
        exit(1);
    }

    // creating the set so it will give me any socket aht are ready to be listen or read.
    fd_set master;
    FD_ZERO(&master);
    FD_SET(connection, &master);
    SOCKET max = connection;
    // now listen for teh active conncetion
    while (1)
    {
        printf("this is the loop running.. \n");
        fd_set reads = master;
        struct timeval ts = {0, 500000};

        if (select(max + 1, &reads, 0, 0, &ts) < 0)
        {
            fprintf(stderr, "ERROR ON GETTING NEW CONNECTION . \n");
            break;
        }
        // if my server is the part of the reads it mean new connnection is read
        if (FD_ISSET(connection, &reads))
        {
            printf("New connection have been established.. \n");
            int returnvalue = active_connection(&master, connection, &max);
            if (returnvalue != 0)
            {
                fprintf(stderr, "ERROR ON GETTING NEW CONNECTION .. \n");
                break;
            }
        }
        check_for_request(&reads, &master);
    }

    printf("Closign socket.. \n");
    CLOSESOCKET(connection);
    free_clients();
    printf("Finished .. \n");
    return 0;
}