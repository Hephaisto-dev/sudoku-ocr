#pragma once

#include "gui/gui.h"



static font_t font1;
static gui_picture_t pic_preview;
static gui_btn_t btn_solve;
static gui_btn_t btn_load_ai;
static gui_btn_t btn_load_img;
static gui_btn_t btn_save;
static gui_slider_t slider;
static gui_label_t txt_rotation;
static gui_switch_t sw_corection;
static gui_label_t txt_status;
static gui_btn_t btn_train_ai;
static gui_btn_t btn_crop;





static void configue_btn(gui_t gui, gui_btn_t btn, const wchar_t* txt, image_t img, color_t fg, font_t font, float size) {
    gui_picture_t icon = gui_new_picture();
    icon->img = img;
    GUI_SET_X1(icon, 0, -1, 0, GUI_ABS_SELF,   GUI_REL_RATIO);
    GUI_SET_X2(icon, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(icon, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(icon, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, btn, icon);

    gui_label_t label = gui_new_label();
    label->text = str_from_wchars_cp(txt);
    label->style.color = fg;
    label->style.font = font;
    label->style.words_warp = false;
    label->style.size = size;
    GUI_SET_X1(label, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_X2(label, 0,  0, 0, GUI_ABS_PREV,   GUI_REL_PARENT);
    GUI_SET_Y1(label, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(label, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, btn, label);
}



void configure_gui(gui_t gui) {
    // Bottom bar full
    gui_frame_t bottom_bar = gui_new_frame();
    bottom_bar->bg = RGB_TO_COLOR(33,46,46);
    GUI_SET_X1(bottom_bar, 0,   0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_X2(bottom_bar, 0,   1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(bottom_bar, 0, 0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(bottom_bar, 0,   1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, bottom_bar);

    // Bottom Bar Right
    gui_label_t bottom_bar_right = gui_new_label();
    bottom_bar_right->bg = RGB_TO_COLOR(63,77,77);
    bottom_bar_right->text = str_from_wchars_cp(L"REYNET");
    bottom_bar_right->style.color = RGB_TO_COLOR(255,255,255);
    bottom_bar_right->style.font = font1;
    bottom_bar_right->style.words_warp = false;
    bottom_bar_right->style.size = 16;
    GUI_SET_X1(bottom_bar_right, 0, -3, 0, GUI_ABS_SELF,   GUI_REL_RATIO);
    GUI_SET_X2(bottom_bar_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(bottom_bar_right, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(bottom_bar_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, bottom_bar, bottom_bar_right);

    // Main Right
    gui_frame_t main_right = gui_new_frame();
    main_right->bg = RGB_TO_COLOR(28, 38, 38);
    GUI_SET_X1(main_right, 0, 0.9, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(main_right, 0,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(main_right, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(main_right, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, main_right);

    // Image Preview
    pic_preview = gui_new_picture();
    pic_preview->bg = RGB_TO_COLOR(0, 0, 0);
    GUI_SET_X1(pic_preview, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(pic_preview, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(pic_preview, 10,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(pic_preview, -10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_right, pic_preview);

    // Logs
    txt_status = gui_new_label();
    txt_status->style.color = HEX_TO_COLOR(0xFFFFFF);
    txt_status->style.font = font1;
    txt_status->style.size = 11;
    txt_status->style.words_warp = false;
    txt_status->text = str_from_wchars_cp(L"...");
    GUI_SET_X1(txt_status, 0, -1, 0,  GUI_ABS_PREV,   GUI_REL_PREV);
    GUI_SET_X2(txt_status, 0,  1, 0, GUI_ABS_SELF, GUI_REL_PREV);
    GUI_SET_Y1(txt_status, 0,  0, 0,  GUI_ABS_PREV,   GUI_REL_PREV);
    GUI_SET_Y2(txt_status, 30,  0, 0, GUI_ABS_SELF, GUI_REL_PARENT);
    gui_append_elem(gui, main_right, txt_status);

    // Solve Sudoku
    btn_solve = gui_new_btn();
    btn_solve->normal = RGB_TO_COLOR(135, 166, 166);
    GUI_SET_X1(btn_solve, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(btn_solve, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_solve, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_solve, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_solve, L"Solve", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_right, btn_solve);

    // Save Image
    btn_save = gui_new_btn();
    btn_save->normal = RGB_TO_COLOR(85, 45, 150);
    GUI_SET_X1(btn_save, 10, 0.5, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(btn_save, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_save, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_save, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_save, L"Save", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_right, btn_save);

    // Main Left
    gui_frame_t main_left = gui_new_frame();
    main_left->bg = RGB_TO_COLOR(28, 38, 38);
    GUI_SET_X1(main_left, 0, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(main_left, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_RATIO);
    GUI_SET_Y1(main_left, 0,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(main_left, 0,  0.9, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, gui->root, main_left);

    // Train AI
    btn_train_ai = gui_new_btn();
    btn_train_ai->bg = HEX_TO_COLOR(0x552D96);
    btn_train_ai->normal = HEX_TO_COLOR(0x552D96);
    GUI_SET_X1(btn_train_ai, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(btn_train_ai, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_train_ai, 10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_train_ai, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_train_ai, L"Train", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_left, btn_train_ai);

    // Crop
    btn_crop = gui_new_btn();
    btn_crop->bg = HEX_TO_COLOR(0x552D96);
    btn_crop->normal = HEX_TO_COLOR(0x552D96);
    GUI_SET_X1(btn_crop, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(btn_crop, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_crop, 10,  0.65, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_crop, -10,  0.8, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_crop, L"Crop", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_left, btn_crop);

    // Load Image
    btn_load_img = gui_new_btn();
    btn_load_img->normal = RGB_TO_COLOR(40, 40, 40);
    GUI_SET_X1(btn_load_img, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(btn_load_img, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_load_img, 10,  0.15, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_load_img, -10,  0.25, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_load_img, L"Load image", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_left, btn_load_img);

    // Load Settings
    btn_load_ai = gui_new_btn();
    btn_load_ai->normal = RGB_TO_COLOR(135, 166, 166);
    GUI_SET_X1(btn_load_ai, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_RATIO);
    GUI_SET_X2(btn_load_ai, -10,  0.5, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(btn_load_ai, 10,  0, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(btn_load_ai, -10,  0.15, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    configue_btn(gui, btn_load_ai, L"Load AI", NULL, RGB_TO_COLOR(255, 255, 255), font1, 16);
    gui_append_elem(gui, main_left, btn_load_ai);

    // Slider
    slider = gui_new_slider();
    slider->handle = HEX_TO_COLOR(0xEADDFF);
    slider->left = HEX_TO_COLOR(0x552D96);
    slider->right = HEX_TO_COLOR(0x445858);
    GUI_SET_X1(slider, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(slider, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(slider, 10,  0.55, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(slider, -10,  0.65, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, slider);

    // Percent rotation
    txt_rotation = gui_new_label();
    txt_rotation->style.color = HEX_TO_COLOR(0xFFFFFF);
    txt_rotation->style.font = font1;
    txt_rotation->style.size = 16;
    txt_rotation->style.words_warp = false;
    txt_rotation->text = str_from_wchars_cp(L"50%");
    GUI_SET_X1(txt_rotation, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(txt_rotation, -10,  0.2, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(txt_rotation, 10,  0.45, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(txt_rotation, -15,  0.6, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, txt_rotation);

    // Button correction
    sw_corection = gui_new_switch();
    sw_corection->bg = HEX_TO_COLOR(0xEADDFF);
    sw_corection->enabled = HEX_TO_COLOR(0x552D96);
    sw_corection->disabled = HEX_TO_COLOR(0x445858);
    GUI_SET_X1(sw_corection, 10, 0, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(sw_corection, -10,  0.2, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(sw_corection, 10,  0.35, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(sw_corection, -15,  0.50, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, sw_corection);

    // Text rotation
    gui_label_t text_autocorect = gui_new_label();
    text_autocorect->style.color = HEX_TO_COLOR(0xFFFFFF);
    text_autocorect->style.font = font1;
    text_autocorect->style.size = 10;
    text_autocorect->style.words_warp = false;
    text_autocorect->text = str_from_wchars_cp(L"Automatic spacial correction");
    GUI_SET_X1(text_autocorect, 10, 0.25, 0, GUI_ABS_PARENT,   GUI_REL_PARENT);
    GUI_SET_X2(text_autocorect, -10,  1, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y1(text_autocorect, 10,  0.3, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    GUI_SET_Y2(text_autocorect, -15,  0.50, 0, GUI_ABS_PARENT, GUI_REL_PARENT);
    gui_append_elem(gui, main_left, text_autocorect);
}

