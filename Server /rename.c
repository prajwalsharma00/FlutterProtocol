#include "rename.h"
void assert(int condition, char *message)
{
    if (condition)
    {
        fprintf(stdout, "%s", message);
    }
    exit(0);
}
void rename_file(char *oldname, char *newname)
{
    if ((rename(oldname, newname)) == 0)
    {
        assert(1, "Sucessfully Changed name");
    }
    else
    {
        assert(1, "Failed\n");
    }
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        assert(1, "Must have 2 argument.. \n");
    }
    rename_file(argv[1], argv[2]);
}