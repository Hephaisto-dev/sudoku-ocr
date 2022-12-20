#pragma once

#include "common.h"

#include "utils/vector.h"

void sys_delay(uint32_t delay);

vect2i32_t sys_get_screen_size();

char* sys_pick_file();

char* sys_save_file();
