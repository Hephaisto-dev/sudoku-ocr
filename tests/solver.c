#include "logic/solver.h"

#include <stdio.h>


void ppprint(unsigned char *grid)
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

char tests[][58] = 
{
    "tests/in/grids/test_00",
    "tests/in/grids/test_01",
    "tests/in/grids/test_02",
    "tests/in/grids/test_03",
    "tests/in/grids/test_04",
    "tests/in/grids/test_05",
    "tests/in/grids/test_06",
    "tests/in/grids/test_07",
    "tests/in/grids/test_08",
    "tests/in/grids/test_09",
    "tests/in/grids/test_10",
    "tests/in/grids/test_11",
    "tests/in/grids/test_12",
    "tests/in/grids/test_13",
    "tests/in/grids/test_14",
    "tests/in/grids/test_15",
    "tests/in/grids/test_16",
    "tests/in/grids/test_17",
    "tests/in/grids/test_18",
    "tests/in/grids/test_19",
    "tests/in/grids/test_20",
    "tests/in/grids/test_21",
    "tests/in/grids/test_22"
};


int main()
{
    char *filename;
    char nfilename[64];
    for (int k = 0; k<23;k++)
    {
    	filename = tests[k];
    
        unsigned char g[81] ={};
        unsigned char *grid = g;

        if (read_grid(filename, grid)) return 1;

        ppprint(grid);
        solve(grid);
        printf("Solved:\n");
        ppprint(grid);
        
        sprintf(nfilename, "tests/out/grids/result_%02i", k);
        
        if (write_grid(nfilename, grid)) return 1;
         
        printf("############################\n\n\n");
    }
    
    return 0;
}
