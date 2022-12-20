#include "logic/solver.h"

#include "common.h"
#include <stdio.h>


static const unsigned short column_index[81] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
};

static const unsigned short row_index[81] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 8,
};

static const unsigned short box_start[81] = {
    0, 0, 0, 3, 3, 3, 6, 6, 6,
    0, 0, 0, 3, 3, 3, 6, 6, 6,
    0, 0, 0, 3, 3, 3, 6, 6, 6,
    27, 27, 27, 30, 30, 30, 33, 33, 33,
    27, 27, 27, 30, 30, 30, 33, 33, 33,
    27, 27, 27, 30, 30, 30, 33, 33, 33,
    54, 54, 54, 57, 57, 57, 60, 60, 60,
    54, 54, 54, 57, 57, 57, 60, 60, 60,
    54, 54, 54, 57, 57, 57, 60, 60, 60
};


void refresh(unsigned char *possibilities, unsigned short i, unsigned char val)
{
    unsigned short c = column_index[i];
    unsigned short r = row_index[i];
    unsigned short s = box_start[i];
    for (unsigned short j = 0; j < 9; j++)
    {
        *(possibilities + 9 * (c + 9 * j) + (val - 1)) = 1;
        *(possibilities + 9 * (9 * r + j) + (val - 1)) = 1;
    }
    for (unsigned short j = 0; j < 3; j++)
    {
        *(possibilities + 9 * (s + j * 9) + (val - 1)) = 1;
        *(possibilities + 9 * (s + j * 9 + 1) + (val - 1)) = 1;
        *(possibilities + 9 * (s + j * 9 + 2) + (val - 1)) = 1;
    }
}


void build_possibilities(unsigned char *grid, unsigned char *possibilities)
{
    for(unsigned short i = 0; i < 81; i++, grid++)
        if (*grid)
            refresh(possibilities, i, *grid);
}


unsigned char count_possibilities(unsigned char *possibilities)
{
    unsigned char c = 9;
    for (unsigned char i = 0; i < 9; i++, possibilities++)
        c-=*possibilities;
    return c;
}


unsigned char search_candidate(unsigned char *grid, unsigned char *possibilities)
{
    unsigned char m = 10;
    unsigned char mi = 81;
    unsigned char tmp;
    for (unsigned char i = 0; i < 81; i++, grid++, possibilities += 9)
        if (!*grid)
        {
            tmp = count_possibilities(possibilities);
            if (tmp < m)
            {
                m = tmp;
                mi = i;
            }
        }
    return mi;
}


void copy_p(unsigned char *p1, unsigned char *p2)
{
    for (unsigned short i = 0; i < 729; i++, p1++, p2++)
        *p2 = *p1;
}


void copy(unsigned char *g1, unsigned char *g2)
{
    for (unsigned char i = 0; i < 81; i++, g1++, g2++)
        *g2 = *g1;
}


unsigned char is_naked_single(unsigned char *possibilities)
{
    unsigned char found = 0;
    for (unsigned short j = 0; j < 9; j++)
    if (!*(possibilities + j))
    {
        if (found) return 0;
        found = j + 1;
    }
    return found;
}


unsigned char naked_singles(unsigned char *grid, unsigned char *possibilities)
{
    unsigned char modified = 0;
    for (unsigned short i = 0; i < 81; i++)
    {
        if (!*(grid + i))
        {
            unsigned char val = is_naked_single(possibilities + i * 9);
            if (val)
            {
                *(grid + i) = val;
                refresh(possibilities, i, val);
                modified = 1;
            }
        }
    }
    return modified;
}


unsigned char _solve(unsigned char *grid,
unsigned char *possibilities)
{
    while (naked_singles(grid, possibilities));
    unsigned char i = search_candidate(grid, possibilities);
    if (i == 81) return 1;
    for (unsigned short j = 0; j < 9; j++)
        if (!*(possibilities + i * 9 + j))
        {
            unsigned char ng[81];
            unsigned char *new_grid = ng;
            unsigned char np[729];
            unsigned char *new_possibilities = np;
            copy(grid, new_grid);
            copy_p(possibilities, new_possibilities);
            new_grid[i] = j + 1;
            refresh(new_possibilities, i, j + 1);
            if (_solve(new_grid, new_possibilities))
            {
                copy(new_grid, grid);
                return 1;
            }
        }
    return 0;
}


int solve(unsigned char *grid)
{
    unsigned char p[729] = {};
    unsigned char *possibilities = p;
    build_possibilities(grid, possibilities);
    return _solve(grid, possibilities);
}




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
