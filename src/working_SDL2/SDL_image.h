/*
Here #define __NO_INLINE__ is used because there is a bug in wchar_t.h
used by SDL2 thar create a warning during compilation on NixOS:

In file included from /run/current-system/sw/include/SDL2/SDL_stdinc.h:72,
                from /run/current-system/sw/include/SDL2/SDL_main.h:25,
                from /run/current-system/sw/include/SDL2/SDL.h:32,
                from grayscale.c:2:
/run/current-system/sw/include/wchar.h: In function ‘wctob’:
/run/current-system/sw/include/wchar.h:326:47: warning: comparison of unsigned expression in ‘>= 0’ is always true [-Wtype-limits]
326 | { return (builtin_constant_p (wc) && wc >= L'\0' && wc <= L'\x7f'
*/

#ifndef __NO_INLINE__
#define __NO_INLINE__
#include <SDL2/SDL_image.h>
#undef __NO_INLINE__
#else
#include <SDL2/SDL_image.h>
#endif
