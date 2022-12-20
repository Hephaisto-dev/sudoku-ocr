#include "system.h"

#include "working_SDL2/SDL.h"

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef WIN32_LEAN_AND_MEAN
#else
    #include <time.h>
#endif


void sys_delay(uint32_t delay) {
    #ifdef WIN32
        Sleep(delay);
    #else
        struct timespec amount, remain;
        amount.tv_sec  = delay / 1000;
        amount.tv_nsec = (delay % 1000) * 1000000L;
        nanosleep(&amount, &remain);
    #endif
}


vect2i32_t sys_get_screen_size() {
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    vect2i32_t r = {
        .x = dm.w,
        .y = dm.h
    };
    return r;
}


#include <gtk/gtk.h>

char* sys_pick_file() {

    gtk_init(NULL, NULL);

    // Create a new file filter to only show image files
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "image/*");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.png");

    // Create a new file chooser dialog with the title "Open File" and the action "Open"
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Open", GTK_RESPONSE_ACCEPT, NULL);

    // Set the file filter for the file chooser
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Show the dialog and wait for a response
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get the filename chosen by the user
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_widget_destroy(dialog);
        return filename;
    }

    // Destroy the dialog
    gtk_widget_destroy(dialog);
    return NULL;
}

char* sys_save_file() {

    gtk_init(NULL, NULL);

    // Create a new file chooser dialog with the given title and parent window
    GtkWidget *file_chooser = gtk_file_chooser_dialog_new("Save File",
                                                        NULL,
                                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                        NULL);

    // Set the default filename for the file to be saved
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(file_chooser), "solved.png");

    // Show the dialog and wait for the user's response
    if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
    {
        // The user selected a file, so get the filename
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
        gtk_widget_destroy(file_chooser);
        return filename;
    }

    // Destroy the file chooser widget
    gtk_widget_destroy(file_chooser);
    return NULL;
}