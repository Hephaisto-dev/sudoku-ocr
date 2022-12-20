#include "logic/solver.h"
#include <stdio.h>


void pprint(unsigned char *grid)
{
    printf("+---+---+---+---+---+---+---+---+---+\n");
    for (int i = 0; i< 9; i++)
    {
        for (int j = 0; j<9; j++, grid++)
            printf("| %hhu ",*grid);
        printf("|\n+---+---+---+---+---+---+---+---+---+\n");
    }
    printf("\n");
}


void write_line(unsigned char *grid, FILE* ptr)
{
    for (unsigned char i = 0; i < 2; i++)
    {
        for (unsigned char j = 0; j < 3; j++, grid++)
            fprintf(ptr, "%c", *grid + 48);
        fprintf(ptr, " ");
    }
    for (unsigned char j = 0; j < 3; j++, grid++)
        fprintf(ptr, "%c", *grid + 48);
}


int read_grid(char *filename, unsigned char *grid)
{
	FILE* ptr = fopen(filename, "r");

    if (ptr == NULL)
    {
        printf("Error, read fail!\n");
        return 1;
    }

    int i = 0;
    char ch = fgetc(ptr);
    while (ch != EOF)
    {
        if (ch > 48 && ch < 58)
        {
            *(grid + i) = ch - 48;
            i++;
        }
        if (ch == 46) i++;
        ch = fgetc(ptr);
    }

    fclose(ptr);
    return 0;
}


int write_grid(char *nfilename, unsigned char *grid)
{
	FILE* ptr = fopen(nfilename, "w");

    if  (ptr == NULL)
    {
        printf("Error!");
        return 1;
    }

    for (unsigned char a = 0; a < 9; a++, grid += 9)
    {
        write_line(grid, ptr);
        fprintf(ptr,"\n");
        if (a == 2 || a == 5)
            fprintf(ptr,"\n");
    }

    fclose(ptr);
    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    char *filename = argv[1];
    
    unsigned char g[81] ={};
    unsigned char *grid = g;

    if (read_grid(filename, grid)) return 1;


    pprint(grid);
    solve(grid);
    printf("Solved:\n");
    pprint(grid);


    char nfilename[1000] = {};
    char suffix[] = ".result";
    char *a = filename;
    char *b = nfilename;
    for (;*a != 0; a++, b++)
        *b = *a;
    a = suffix;
    for (;*a != 0; a++, b++)
        *b = *a;
    *b = 0;
    
    if (write_grid(nfilename, grid)) return 1;
    
    return 0;
}

