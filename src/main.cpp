
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "cpu.h"
#include "e8910.h"
#include "vecx.h"

#define EMU_TIMER 20

static SDL_Window   *screen           = NULL;
static SDL_Renderer *renderer         = NULL;
static SDL_Surface  *overlay_original = NULL;
static SDL_Surface  *overlay          = NULL;

static long  scl_factor;
static long  offx;
static long  offy;
static char *cartfilename = NULL;


CPU *cpu = new CPU();

void osint_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int v = 0; v < vector_draw_cnt; v++) {
        uint8_t c = vectors_draw[v].color * 256 / VECTREX_COLORS;
        aalineRGBA(renderer, offx + vectors_draw[v].x0 / scl_factor, offy + vectors_draw[v].y0 / scl_factor,
                   offx + vectors_draw[v].x1 / scl_factor, offy + vectors_draw[v].y1 / scl_factor, c, c, c, 0xff);
    }

    SDL_RenderPresent(renderer);
}

void resize(int width, int height)
{
    long sclx, scly;
    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &screen, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: %s", SDL_GetError());
    }
    overlay_original = SDL_GetWindowSurface(screen);

    sclx = ALG_MAX_X / overlay_original->w;
    scly = ALG_MAX_Y / overlay_original->h;

    scl_factor = sclx > scly ? sclx : scly;

    offx = (width - ALG_MAX_X / scl_factor) / 2;
    offy = (height - ALG_MAX_Y / scl_factor) / 2;
}
static void readevents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            // case SDL_VIDEORESIZE:
            //     resize(e.resize.w, e.resize.h);
            //     break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit(EXIT_SUCCESS);
                    case SDLK_a:
                        snd_regs[14] &= ~0x01;
                        break;
                    case SDLK_s:
                        snd_regs[14] &= ~0x02;
                        break;
                    case SDLK_d:
                        snd_regs[14] &= ~0x04;
                        break;
                    case SDLK_f:
                        snd_regs[14] &= ~0x08;
                        break;
                    case SDLK_LEFT:
                        alg_jch0 = 0x00;
                        break;
                    case SDLK_RIGHT:
                        alg_jch0 = 0xff;
                        break;
                    case SDLK_UP:
                        alg_jch1 = 0xff;
                        break;
                    case SDLK_DOWN:
                        alg_jch1 = 0x00;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_a:
                        snd_regs[14] |= 0x01;
                        break;
                    case SDLK_s:
                        snd_regs[14] |= 0x02;
                        break;
                    case SDLK_d:
                        snd_regs[14] |= 0x04;
                        break;
                    case SDLK_f:
                        snd_regs[14] |= 0x08;
                        break;
                    case SDLK_LEFT:
                        alg_jch0 = 0x80;
                        break;
                    case SDLK_RIGHT:
                        alg_jch0 = 0x80;
                        break;
                    case SDLK_UP:
                        alg_jch1 = 0x80;
                        break;
                    case SDLK_DOWN:
                        alg_jch1 = 0x80;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}
void load_overlay(const char *filename)
{
    SDL_Surface *image;
    image = IMG_Load(filename);
    if (image) {
        overlay_original = image;
    } else {
        fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
    }
}
static void init()
{
    FILE *f;
    if (!(f = fopen("rom.dat", "rb"))) {
        exit(EXIT_FAILURE);
    }
    if (fread(rom, 1, sizeof(rom), f) != sizeof(rom)) {
        printf("Invalid rom length\n");
        exit(EXIT_FAILURE);
    }
    fclose(f);

    memset(cart, 0, sizeof(cart));
    if (cartfilename) {
        FILE *f;
        if (!(f = fopen(cartfilename, "rb"))) {
            perror(cartfilename);
            exit(EXIT_FAILURE);
        }

        fread(cart, 1, sizeof(cart), f);
        fclose(f);
    }
}
void osint_emuloop()
{
    Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
    vecx_reset();

    for (;;) {
        vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER);
        readevents();

        {
            Uint32 now = SDL_GetTicks();
            if (now < next_time)
                SDL_Delay(next_time - now);
            else
                next_time = now;

            next_time += EMU_TIMER;
        }
    }
}
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    resize(330 * 3 / 2, 410 * 3 / 2);

    if (argc > 1)
        cartfilename = argv[1];

    init();
    e8910_init_sound();
    osint_emuloop();

    e8910_done_sound();
    SDL_Quit();
    return 0;
}
