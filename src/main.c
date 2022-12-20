#include "app/gui_main.h"
#include "system/system.h"
#include "draw_grid.h"
#include "logic/solver.h"
#include "logic/neural_network.h"
#include "image/postprocess.h"
#include "image/detection/cell.h"

#include <pthread.h>


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 450


#define TRAIN_EPOCHS (6)
#define TRAIN_LR_FAST (0.1)
#define TRAIN_LR_SLOW (0.01)


void errx(int code, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(code);
}


static char* def_ai_filename = "nn_data.bin";
static char* ai_filename;

static char* img_in_filename;
static char* grid_in_filename;
static grid_out_filename[256] = {};
image_t origin_img;
image_t current_img;
char sodoku_data[81] = {};

bool is_training = false;
pthread_t train_thr_id = 0;



cells_t cut_in_cells(image_t img){
    grid_t grid = grid_detect(img);

    int32_t cell_w = (grid.downright.x - grid.upleft.x) / 9;
    int32_t cell_h = (grid.downright.y - grid.upleft.y) / 9;

    int32_t off_x = grid.upleft.x;
    int32_t off_y = grid.upleft.y;

    cells_t cells = {
        .cells = (image_t*) malloc(sizeof(image_t) * 9 * 9),
        .nb_cells = 9 * 9,
        .grid = grid
    };

    for (int32_t x = 0; x < 9; x++) {
        for (int32_t y = 0; y < 9; y++) {
            int32_t rx = off_x + x * cell_w;
            int32_t ry = off_y + y * cell_h;
            cells.cells[y * 9 + x] =
                img_cut(img, rx, ry, rx + cell_w, ry + cell_h);
        }
    }

    return cells;
}



void set_status(const wchar_t* txt) {
    mprintf("%ls\n", txt);
    if (!txt_status->text) str_free(txt_status->text);
    txt_status->text = str_from_wchars_cp(txt);
}


void* train_thread(void *vargp) {
    train(TRAIN_EPOCHS, TRAIN_LR_FAST, ai_filename);
    is_training = false;
}


void start_training() {
    if (!is_training && ai_filename) {
        is_training = true;
        pthread_create(&train_thr_id, NULL, train_thread, NULL);
    }
}



void on_load_ai(void* userdata) {
    mprintf("Load AI\n");
    char* filename = sys_pick_file();
    if (!ai_filename) free(ai_filename);
    ai_filename = filename;
}

void on_save_image(void* userdata) {
    mprintf("Save image\n");
    char* filename = sys_save_file();
    img_save_to_file(current_img, filename);
    free(filename);
}

void on_load_image(void* userdata) {
    mprintf("Load image\n");
    char* filename = sys_pick_file();
    if (current_img) img_free(current_img);
    if (origin_img) img_free(origin_img);
    current_img = img_load_from_file(filename);
    origin_img = img_clone(current_img);
    pic_preview->img = current_img;
    free(filename);
}

void on_solve(void* userdata) {
    mprintf("Solve grid\n");

    set_status(L"Detecting grid ...");
    cells_t cells = cut_in_cells(current_img);

    set_status(L"Reconizing characters ...");

    // Biases
    double hiddenLayerBias[NUM_HIDDEN_NODES];
    double outputLayerBias[NUM_OUTPUTS];

    // Weights
    double hiddenWeights[NUM_INPUTS][NUM_HIDDEN_NODES];
    double outputWeights[NUM_HIDDEN_NODES][NUM_OUTPUTS];

    if (!load(hiddenWeights, outputWeights, hiddenLayerBias, outputLayerBias,
        ai_filename)
    ) {
        set_status(L"Impossible de charger les poids et les biais\n");
        return;
    }

    for (size_t i = 0; i < 81; i++) {
        img_blackandwhite(cells.cells[i]);
        dataset_img_t image = dataset_from_img(cells.cells[i]);

        size_t nb_black = 0;
        for (size_t j = 0; j < (28*28); j++) {
            if (image.data[j] > 200) {
                nb_black++;
            }
        }

        if (nb_black > 15) {
            double *img_data = malloc(sizeof(double) * NUM_INPUTS);
            for (size_t j = 0; j < NUM_INPUTS; j++) {
                img_data[j] = (image.data[j] > 127) ? 0 : 1;
            }

            char predicted = predict(hiddenWeights, outputWeights, hiddenLayerBias,
                    outputLayerBias, img_data);
            free(img_data);

            mprintf("Position %d : %c\n", i, predicted);
            sodoku_data[i] = (predicted > 0) ? (predicted - '0') : '.';
        } else {
            sodoku_data[i] = '.';
        }
    }


    set_status(L"Solving grid ...");
    //read_grid(grid_in_filename, sodoku_data);
    solve(sodoku_data);
    sprintf(grid_out_filename, "%s.result", grid_in_filename);
    write_grid(grid_out_filename, sodoku_data);

    set_status(L"Draw grid ...");
    image_t drawed = draw_grid(grid_in_filename, grid_out_filename, 1, true);
    if (current_img) img_free(current_img);
    if (origin_img) img_free(origin_img);
    current_img = drawed;
    origin_img = img_clone(current_img);
    pic_preview->img = current_img;
}

void on_slider_change(void* userdata, float value) {
    if (!sw_corection->stats) {
        //mprintf("Slider changed to %f%%\n", value * 100);
        wchar_t buff[128];
        swprintf(buff, 128, L"%i%%", (int) (value * 100));
        str_free(txt_rotation->text);
        txt_rotation->text = str_from_wchars_cp(buff);
        img_rotate_to(origin_img, current_img, (value - 0.5) * 2 * PI);
        pic_preview->img = current_img;
    } else {
        str_free(txt_rotation->text);
        txt_rotation->text = str_from_wchars_cp(L"50%");
        slider->value = 0.5;
    }
}

void on_train(void* userdata) {
    mprintf("Train\n");
    start_training();
}

void on_autocorrect(void* userdata) {
    mprintf("Auto correct switched\n");
    if (sw_corection->stats) {
        slider->value = 0.5;
    }
}

void on_crop(void* userdata) {
    mprintf("Crop\n");
}


int main(int argc, char** args) {
    // Inits

    SDL_Init(SDL_INIT_VIDEO);
    input_init();
    font_init();

    current_img = img_new(100, 100, RGB_TO_COLOR(0,0,0));
    origin_img = img_clone(current_img);
    ai_filename = def_ai_filename;


    // Window

    vect2i32_t ssize = sys_get_screen_size();
    window_t win = win_create(
        "REYNET",
        (ssize.x - WINDOW_WIDTH) / 2, (ssize.y - WINDOW_HEIGHT) / 2,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0 //WIN_FLAG_RESIZEABLE
    );
    renderer_t rndr = win->renderer;


    // Load font

    font1 = font_load_from_file("assets/fonts/consolas.ttf");
    if (!font1) {
        mprintf("Failed to load font file\n");
        return 1;
    }


    // Atlas

    atlas_t atlas = atlas_new(rndr);


    // Setup GUI

    gui_t gui = gui_new(rndr, atlas, 120);
    configure_gui(gui);

    btn_load_ai->onclick = on_load_ai;
    btn_solve->onclick = on_solve;
    btn_save->onclick = on_save_image;
    slider->onchange = on_slider_change;
    sw_corection->onclick = on_autocorrect;
    btn_train_ai->onclick = on_train;
    btn_crop->onclick = on_crop;
    btn_load_img->onclick = on_load_image;

    pic_preview->img = current_img;


    // Main loop

    bool quit = false;
    while (!quit) {
        // Render everythings
        rndr_clear(rndr, HEX_TO_COLOR(0xFF2626));
        gui_render(gui);
        win_update(win);

        // Main event loop
        event_t ev;
        while (input_get(&ev) && !quit) {
            switch (ev.type)
            {
                case EVENT_QUIT:
                    quit = true;
                    break;

                case EVENT_WIN_RESIZED:
                    gui_invalidate(gui, gui->root);
                    break;

                default:
                    gui_process_event(gui, &ev);
                    break;
            }
        }

        // Caps framerate
        sys_delay(1000 / 30);
    }


    // Frees

    gui_free(gui);
    font_free(font1);
    win_free(win);


    // Destroys

    font_destroy();
    input_destroy();
    SDL_Quit();


    return EXIT_SUCCESS;
}
