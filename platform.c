#include "platform.h"

/* Constructor equivalent */
bool platform_init(Platform *p,
                   const char *title,
                   int windowWidth, int windowHeight,
                   int textureWidth, int textureHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    p->window = SDL_CreateWindow(
        title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!p->window) return false;

    p->renderer = SDL_CreateRenderer(
        p->window, -1, SDL_RENDERER_ACCELERATED);
    if (!p->renderer) return false;

    p->texture = SDL_CreateTexture(
        p->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth, textureHeight);
    if (!p->texture) return false;

    return true;
}

/* Input handling */
bool platform_process_input(Platform *p, uint8_t keys[16])
{
    (void)p; // unused
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            uint8_t pressed = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: quit = true; break;
            case SDLK_x: keys[0x0] = pressed; break;
            case SDLK_1: keys[0x1] = pressed; break;
            case SDLK_2: keys[0x2] = pressed; break;
            case SDLK_3: keys[0x3] = pressed; break;
            case SDLK_q: keys[0x4] = pressed; break;
            case SDLK_w: keys[0x5] = pressed; break;
            case SDLK_e: keys[0x6] = pressed; break;
            case SDLK_a: keys[0x7] = pressed; break;
            case SDLK_s: keys[0x8] = pressed; break;
            case SDLK_d: keys[0x9] = pressed; break;
            case SDLK_z: keys[0xA] = pressed; break;
            case SDLK_c: keys[0xB] = pressed; break;
            case SDLK_4: keys[0xC] = pressed; break;
            case SDLK_r: keys[0xD] = pressed; break;
            case SDLK_f: keys[0xE] = pressed; break;
            case SDLK_v: keys[0xF] = pressed; break;
            }
            break;
        }
        }
    }

    return quit;
}

/* Rendering */
void platform_update(Platform *p, const void *buffer, int pitch)
{
    SDL_UpdateTexture(p->texture, NULL, buffer, pitch);
    SDL_RenderClear(p->renderer);
    SDL_RenderCopy(p->renderer, p->texture, NULL, NULL);
    SDL_RenderPresent(p->renderer);
}

/* Destructor equivalent */
void platform_destroy(Platform *p)
{
    if (p->texture)  SDL_DestroyTexture(p->texture);
    if (p->renderer) SDL_DestroyRenderer(p->renderer);
    if (p->window)   SDL_DestroyWindow(p->window);
    SDL_Quit();
}
