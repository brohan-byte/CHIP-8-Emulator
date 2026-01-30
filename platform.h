#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
} Platform;

/* Initialize SDL and create window/renderer/texture */
bool platform_init(Platform *p,
                   const char *title,
                   int windowWidth, int windowHeight,
                   int textureWidth, int textureHeight);

/* Handle keyboard input and update CHIP-8 keypad */
bool platform_process_input(Platform *p, uint8_t keys[16]);

/* Render the CHIP-8 framebuffer */
void platform_update(Platform *p, const void *buffer, int pitch);

/* Destroy SDL resources and quit SDL */
void platform_destroy(Platform *p);

#endif
