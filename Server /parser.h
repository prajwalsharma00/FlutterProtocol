#ifndef PARSER_H
#define PARSER_H

#define filename "data.csv"

char *parsedate(char *string);
int updateandelete(int mode, char *string);
int addatask(char *string);
int deletetask(char *string);
int updatetask(char *string);
char *get_data(char *string);
void write_file(char *file_name, char *string);
int date_checker(char *date);
char *getall();
char *data_parser(char *string);

#define VALIDPOINTER(s) (s != NULL)
#define VALIDFILE(s) (s != NULL)

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#endif