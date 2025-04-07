#include "client.h"
#define VALIDPOINTER(s) (s != NULL)
char *sendtoserver(char *respond, SOCKET client)
{
    int bytes_send = 0;
    int datatobesent = strlen(respond);
    while (datatobesent != bytes_send)
    {
        int datasent = send(client, respond + bytes_send, strlen(respond), 0);
        bytes_send += datasent;
    }
    if (bytes_send > 0)
    {
        fprintf(stdout, "Data sent sucessfully ..\n\n \n");
    }
    else
    {
        fprintf(stdout, "ERROR in sending data .. \n\n");
    }
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
    printf("START RECVING DATA FROM THESERVER  .. \n\n\n");
    char temp_recv[1024];
    int data_recv;
    while ((data_recv = recv(client, temp_recv, 1024, 0)) > 0)
    {
        if (strstr(temp_recv, "|END"))
        {
            memcpy(mainbuffer + index, temp_recv, data_recv - 4);
            return mainbuffer;
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
    printf("RECEVING COMPLETED . \n");

    mainbuffer[index - 4] = '\0';
    printf("the data received is %s \n", mainbuffer);
    return mainbuffer;
}

int main()
{
    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));

    server_info.sin_port = htons(8080);
    if ((inet_pton(AF_INET, "127.0.0.1", &server_info.sin_addr)) <= 0)
    {
        fprintf(stderr, "Invalid address. \n");
        exit(1);
    }
    SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
    printf("Connecting... \n");
    if (connect(client, (struct sockaddr *)&server_info, sizeof(server_info)) < 0)
    {
        fprintf(stderr, "ERROR ON CONNECTION TO THE client");
        return 0;
    }
    printf("CLIENT: Connected .. \n\n");
    char *buffer = "UPDATE|DATE20291212|TASKWALKTODAY!STATEtrue|END";
    sendtoserver(buffer, client);
    char *respond = recvdata(client);
    fprintf(stdout, "the respond from the server was %s \n", respond);
    char *getdata = "GET|DATE20291212|END";
    sendtoserver(getdata, client);
    respond = recvdata(client);
    fprintf(stdout, "the second respond from the server was %s \n", respond);

    // printf("CLIENT:SENDING DATA TO SERVER .. \n");
    // sendtoserver(buffer, client);

    // printf("CLIENT:RECVING DATA FROM SERVER .. \n");
    // char *datarecv = recvdata(client);
    // printf("SERVER: %s \n", datarecv);
    // free(datarecv);

    // char *request = "GET 20250502END";
    // printf("CLIENT: SENDING DATA TO SERVER .. \n");
    // sendtoserver(request, client);

    // printf("CLIENT: RECVING DATA FROM SERVER .. \n");
    // datarecv = recvdata(client);
    // printf("SERVER: %s \n", datarecv);
    // free(datarecv);

    // printf("CLIENT: Connected .. \n\n");
    // buffer = "SEND DATE:2025052|TASK#Today i will not run !state:true#JOG!state:trueEND";

    // printf("CLIENT:SENDING DATA TO SERVER .. \n");

    CLOSESOCKET(client);
}