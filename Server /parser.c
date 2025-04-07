#include "parser.h"
typedef enum Mode
{
    DELETE,
    UPDATE,
    ADD

} mode;
char *parsedate(char *string) // to check if our request date match with our database
{

    char *dat = strstr(string, "DATE");

    if (!VALIDPOINTER(dat))
    {
        return "ERROR";
    }
    int index = 0;

    while (!isdigit(dat[index]))
    {
        index++;
    }

    char *date = malloc(10);
    for (int i = 0; i < 8; i++)
    {

        date[i] = dat[i + index];
    }
    date[8] = '\0';
    printf("the date is %s \n ", date);

    return date;
}
int updateandelete(int mode, char *string) // reutrn 1 on sucess nad 0 if no date is availbe of that time
{
    char temp[1024];
    sprintf(temp, "%s", string);
    char *newfile = "newfile.csv";
    FILE *newfil = fopen("newfile.csv", "w");
    if (!VALIDFILE(newfil))
    {
        fprintf(stdout, "ERRRO ON OPening the file .. \n");
    }
    char *date = strtok(temp, "|") + 4;
    fprintf(stdout, "The date in the updateanddelete is %s \n", date);
    char updated_task[1024];

    char *state = strstr(string, "!STATE");

    char *task = strtok(NULL, "!") + 4;
    fprintf(stdout, "the task is %s \n", task);
    sprintf(updated_task, "%s,%s\n", task, state + 6);
    FILE *fp = fopen(filename, "r");
    if (!VALIDPOINTER(fp))
    {
        fprintf(stderr, "Invalid File  ");
        return -1;
    }

    char buffer[1024];
    while ((fgets(buffer, sizeof(buffer), fp)))
    {
        if (strstr(buffer, date))
        {

            fputs(buffer, newfil);
            char internal_buffer[1024];
            if (mode == ADD)
            {
                fprintf(stdout, "the update task is %s\n", updated_task);
                fputs(updated_task, newfil);
            }

            while (fgets(internal_buffer, sizeof(internal_buffer), fp))
            {

                fprintf(stdout, "The internal buffer of the given date is %s \n", internal_buffer);
                if (strstr(internal_buffer, "DATE,"))
                {

                    fputs(internal_buffer, newfil);

                    break;
                }
                if (strstr(internal_buffer, task))
                {
                    // 1 for update task , 0 for delete
                    if (mode == UPDATE)
                    {
                        fputs(updated_task, newfil);
                    }
                    else if (mode == ADD)
                    {
                        fputs(internal_buffer, newfil);
                    }
                }
                else
                {
                    fputs(internal_buffer, newfil);
                }
            }
        }
        else
        {
            fputs(buffer, newfil);
        }
    }
    remove("data.csv");
    rename(newfile, "data.csv");
    fclose(newfil);
    fclose(fp);
    return 0;
}
int addatask(char *string)
{
    return updateandelete(ADD, string);
}
int deletetask(char *string)
{
    return updateandelete(DELETE, string);
}
int updatetask(char *string)
{
    return updateandelete(UPDATE, string);
}

char *get_data(char *string) // this is to get database from server
{
    FILE *fp = fopen(filename, "r");
    if (!VALIDFILE(fp))
    {
        fprintf(stderr, "ERROR \n");
        exit(1);
    }
    char *date = string + 4;
    static char write_buffer[5124];
    memset(write_buffer, 0, sizeof(write_buffer));
    int index = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (strstr(buffer, date))
        {
            char internal_buffer[2048];
            sprintf(write_buffer + index, "%s", buffer);
            index += (strlen(buffer));
            while (fgets(internal_buffer, sizeof(write_buffer) - index, fp))
            {

                if (strstr(internal_buffer, "DATE"))
                {

                    return write_buffer;
                }
                else
                {

                    sprintf(write_buffer + index, "%s", internal_buffer);
                    index += (strlen(internal_buffer));
                }
            }
            return write_buffer;
        }
    }
    return "NULL";
}
void write_file(char *file_name, char *string) // to write dat to file
{
    FILE *file = fopen(filename, "a");

    if (!VALIDFILE(file))
    {
        fprintf(stderr, "ERROR ON FILE CREATION .. \n");
        exit(1);
    }
    int bytes_written = fprintf(file, "%s\n", string);
    if (bytes_written == 0)
    {
        fprintf(stderr, "NO data was given to write .. \n");
    }
    fclose(file);
}
int date_checker(char *date)
{
    FILE *fp = fopen(filename, "r");
    if (!VALIDFILE(fp))
    {
        return -1;
    }
    char buffer[1024];
    while ((fgets(buffer, sizeof(buffer), fp)))
    {
        if (strstr(buffer, date))
        {
            return 0;
        }
    }
    return -1;
}
char *getall()
{
    FILE *fp = fopen(filename, "r");
    if (!VALIDFILE(fp))
    {
        fprintf(stderr, "ERROR \n");
        exit(1);
    }
    static char write_buffer[5124];
    memset(write_buffer, 0, sizeof(write_buffer));
    int index = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp))
    {
        sprintf(write_buffer + index, "%s", buffer);
        index += (strlen(buffer));
    }
    if (strlen(write_buffer))
    {
        return write_buffer;
    }
    return "EMPTY FILE";
}
char *data_parser(char *string) // this is to check wherether the request is send or receive

{

    // printf("\n\n\n THE SERVER REQUEST WAS \n\n\n%s ", string);
    char datas[4018];
    sprintf(datas, "%s", string);
    if (strstr(string, "GET /download HTTP/1.1"))
    {
        FILE *log = fopen("server.log", "a");
        if (!VALIDFILE(log))
        {
            fprintf(stdout, "couldnot open server log .. \n");

            return "ERROR";
        }
        fwrite("\n\n", 2, 1, log);
        fwrite(string, strlen(string), sizeof(char), log);
        fclose(log);
        char *csv_data = getall(); // assume this returns a pointer to static/global buffer
        int length = strlen(csv_data);

        char *response = malloc(length + 512); // enough room for headers
        if (!response)
        {
            return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\nMemory allocation failed.");
        }

        sprintf(response,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"

                "Content-Length: %d\r\n"
                "\r\n"
                "<html><body><pre><h1>%s</h1></pre></body></html>",
                length + 46, csv_data);

        return response;
    }

    char *command = strtok(datas, "|");
    if (strncmp(command, "GETALL", 6) == 0)
    {
        return getall();
    }
    char *date = strtok(NULL, "|");
    fprintf(stdout, "the command is %s and teh date is %s \n", command, date);
    if (!VALIDPOINTER(date))
    {
        fprintf(stderr, "EROR on date.. \n");
        return "ERROR";
    }
    char *other_request = strstr(string, "|") + 1;
    fprintf(stdout, "The other request is %s \n", other_request);
    if (strncmp(command, "UPDATE", 6) == 0)
    {
        fprintf(stdout, "SERVER:UPDATE COMMAND RECEIVED .. \n");
        int return_value = updatetask(other_request);
        if (!return_value)
            return "SUCESSFULL";
        return "ERROR";
    }
    // DELETE
    else if (strncmp(command, "DELETE", 6) == 0)
    {
        fprintf(stdout, "SERVER:DELETE COMMAND RECEIVED .. \n");
        int return_value = deletetask(other_request);
        if (!return_value)
            return "SUCESSFULL";
        return "ERROR";
    }
    // ADD
    else if (strncmp(command, "ADD", 3) == 0)
    {
        fprintf(stdout, "SERVER:ADD COMMAND RECEIVED .. \n");

        char *parse_date = parsedate(date);
        char buffer[1024];
        sprintf(buffer, "%s,%s", "DATE", parse_date);
        fprintf(stdout, "the date is %s \n", buffer);
        free(parse_date);
        int isdateavailabe = date_checker(buffer);
        if (isdateavailabe) // TODO check this funciton
        {
            fprintf(stdout, "NO date was found.. \n");
            write_file(filename, buffer);
        }
        int return_value = addatask(other_request);

        if (!return_value)
            return "SUCESSFULL";
        return "ERROR";
    }
    else if (strncmp(command, "GET", 3) == 0)
    {
        return get_data(other_request);
    }
    else
    {
        fprintf(stdout, "SERVER: CLIENT REQUEST WAS ERROR\n");
        return "ERROR";
    }
    return "ERROR";
}
// int main()
// {
//     fprintf(stdout, "%s \n", getall());
// }

// TODO MAKE THE ADD FUCNRTION MORE GOOD SOME BUGS LIKE IF YOU ADD SOMEHTINGTHAT ISA LAREADY THERE IT DLETE THE WHOLE FILE