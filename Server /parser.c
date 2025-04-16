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
        // 1) Fetch your CSV
        char *csv_data = getall();
        if (!csv_data)
        {
            return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to load data.");
        }

        // 2) HTML header + CSS
        const char *html_start =
            "<!DOCTYPE html>"
            "<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>TODOLIST</title>"
            "<style>"
            "  body { background:#f0f2f5; font-family:Arial, sans-serif; margin:0; padding:20px; }"
            "  h1 { text-align:center; color:#333; }"
            "  section { background:#fff; max-width:600px; margin:20px auto; "
            "             border-radius:8px; box-shadow:0 4px 12px rgba(0,0,0,0.1); overflow:hidden; }"
            "  section h2 { margin:0; padding:12px 20px; background:#4a90e2; color:#fff; }"
            "  table { width:100%; border-collapse:collapse; }"
            "  th, td { padding:12px 16px; }"
            "  th { background:#f7f9fa; text-transform:uppercase; font-size:0.85em; }"
            "  tr:nth-child(even) { background:#fbfcfd; }"
            "</style>"
            "</head><body>"
            "<h1>TODOLIST</h1>";

        const char *html_end = "</body></html>";

        // 3) Allocate a buffer for building the HTML body
        size_t buf_size = strlen(html_start) + strlen(csv_data) * 4 + strlen(html_end) + 1024;
        char *body = malloc(buf_size);
        if (!body)
        {
            return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\nMemory allocation failed.");
        }
        strcpy(body, html_start);

        // 4) Parse CSV line-by-line
        char *saveptr, *line = strtok_r(csv_data, "\n", &saveptr);
        int in_table = 0;
        while (line)
        {
            if (strncmp(line, "DATE,", 5) == 0)
            {
                // Close previous table/section if open
                if (in_table)
                {
                    strcat(body, "  </table></section>");
                }
                // Start new section + table
                char *date = line + 5;
                strcat(body,
                       "<section>"
                       "<h2>Date: ");
                strcat(body, date);
                strcat(body, "</h2>"
                             "<table>"
                             "<tr><th>Task</th><th>DONE</th></tr>");
                in_table = 1;
            }
            else
            {
                // This is a task line: "TaskName,true|false"
                char *comma = strchr(line, ',');
                if (comma)
                {
                    *comma = '\0';
                    char *task = line;
                    char *done = comma + 1;
                    strcat(body, "<tr><td>");
                    strcat(body, task);
                    strcat(body, "</td><td>");

                    // Check the done field and output "Completed" or "Incomplete"
                    if (strcmp(done, "true") == 0)
                        strcat(body, "Completed");
                    else
                        strcat(body, "Incomplete");

                    strcat(body, "</td></tr>");
                }
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }
        // Close last table/section if open
        if (in_table)
        {
            strcat(body, "  </table></section>");
        }
        // Append closing HTML
        strcat(body, html_end);

        // 5) Wrap with HTTP headers (not visible in the page)
        int content_length = strlen(body);
        char header_buf[256];
        int header_len = snprintf(header_buf, sizeof(header_buf),
                                  "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html; charset=UTF-8\r\n"
                                  "Content-Length: %d\r\n"
                                  "\r\n",
                                  content_length);

        // 6) Allocate final response
        char *response = malloc(header_len + content_length + 1);
        if (!response)
        {
            free(body);
            return strdup("HTTP/1.1 500 Internal Server Error\r\n\r\nMemory allocation failed.");
        }
        memcpy(response, header_buf, header_len);
        memcpy(response + header_len, body, content_length + 1);

        free(body);
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
