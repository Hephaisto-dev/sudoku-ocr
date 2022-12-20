#include "graphic/image.h"
#include "image/detection/cell.h"

#include "working_SDL2/SDL.h"
#include "working_SDL2/SDL_image.h"


#include <stdio.h>
#include <string.h>


int main(int argc, char** args) {
    char* filename = "samples/img_06.jpeg";

    if (argc == 2) {
        filename = args[1];
    }

    image_t img = img_load_from_file(filename);

    if (img == NULL) {
        fprintf(stderr, "ERROR: Can't read image file '%s'\n", filename);
        return 1;
    }

    mprintf("Start detecting cells...\n");
    cells_t cells = cells_detect(img);
    mprintf("Finished deteting cells\n\n");

    mprintf("Upper left corner : (%i, %i)\n", (int) cells.grid.upleft.x, (int) cells.grid.upleft.y);
    mprintf("Upper right corner : (%i, %i)\n", (int) cells.grid.upright.x, (int) cells.grid.upright.y);
    mprintf("Lower left corner : (%i, %i)\n", (int) cells.grid.downleft.x, (int) cells.grid.downleft.y);
    mprintf("Lower right corner : (%i, %i)\n\n", (int) cells.grid.downright.x, (int) cells.grid.downright.y);

    char dstfilename[64];
    for (size_t i = 0; i < cells.nb_cells; i++) {
        sprintf(dstfilename, "tests/out/cells/cell_%02zu.png", i);
        img_save_to_file(cells.cells[i], dstfilename);
        mprintf("Saved cell %zu to file '%s'\n", i, dstfilename);
    }

    return 0;
}
