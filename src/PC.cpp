#include "PC.h"
#include "cpu.h"
#include "e8910.h"
#include "vecx.h"
#include "rom_dat.h"


#define EMU_TIMER 20


PC::PC()
{
    cpu  = new CPU(this);
    vecx = new VECX(this);
    psg  = new AY8910(this);
}
PC::~PC()
{
    delete cpu;
    delete vecx;
    delete psg;
}
void PC::osint_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int v = 0; v < vecx->vector_draw_cnt; v++) {
        uint8_t c = vecx->vectors_draw[v].color * 256 / vecx->VECTREX_COLORS;
        aalineRGBA(renderer, offx + vecx->vectors_draw[v].x0 / scl_factor, offy + vecx->vectors_draw[v].y0 / scl_factor,
                   offx + vecx->vectors_draw[v].x1 / scl_factor, offy + vecx->vectors_draw[v].y1 / scl_factor, c, c, c,
                   0xff);
    }

    SDL_RenderPresent(renderer);
}
void PC::resize(int width, int height)
{
    long sclx, scly;

    if (screen)
        SDL_DestroyWindow(screen);
    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &screen, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: %s", SDL_GetError());
    }
    overlay_original = SDL_GetWindowSurface(screen);

    sclx = vecx->ALG_MAX_X / overlay_original->w;
    scly = vecx->ALG_MAX_Y / overlay_original->h;

    scl_factor = sclx > scly ? sclx : scly;

    offx = (width - vecx->ALG_MAX_X / scl_factor) / 2;
    offy = (height - vecx->ALG_MAX_Y / scl_factor) / 2;
}
void PC::readevents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            // case SDL_WINDOWEVENT_RESIZED:
            //     resize(e.window.data1, e.window.data2);
            //     break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit(EXIT_SUCCESS);
                    case SDLK_a:
                        vecx->snd_regs[14] &= ~0x01;
                        break;
                    case SDLK_s:
                        vecx->snd_regs[14] &= ~0x02;
                        break;
                    case SDLK_d:
                        vecx->snd_regs[14] &= ~0x04;
                        break;
                    case SDLK_f:
                        vecx->snd_regs[14] &= ~0x08;
                        break;
                    case SDLK_LEFT:
                        vecx->alg_jch0 = 0x00;
                        break;
                    case SDLK_RIGHT:
                        vecx->alg_jch0 = 0xff;
                        break;
                    case SDLK_UP:
                        vecx->alg_jch1 = 0xff;
                        break;
                    case SDLK_DOWN:
                        vecx->alg_jch1 = 0x00;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_a:
                        vecx->snd_regs[14] |= 0x01;
                        break;
                    case SDLK_s:
                        vecx->snd_regs[14] |= 0x02;
                        break;
                    case SDLK_d:
                        vecx->snd_regs[14] |= 0x04;
                        break;
                    case SDLK_f:
                        vecx->snd_regs[14] |= 0x08;
                        break;
                    case SDLK_LEFT:
                        vecx->alg_jch0 = 0x80;
                        break;
                    case SDLK_RIGHT:
                        vecx->alg_jch0 = 0x80;
                        break;
                    case SDLK_UP:
                        vecx->alg_jch1 = 0x80;
                        break;
                    case SDLK_DOWN:
                        vecx->alg_jch1 = 0x80;
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
void PC::load_overlay(const char *filename)
{
    SDL_Surface *image;
    image = IMG_Load(filename);
    if (image) {
        overlay_original = image;
    } else {
        fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
    }
}
void PC::load_rom()
{
    for (int i = 0; i < 8192; ++i)
        vecx->rom[i] = rom_dat[i];

    memset(vecx->cart, 0, sizeof(vecx->cart));
    if (cartfilename) {
        FILE *f;
        if (!(f = fopen(cartfilename, "rb"))) {
            perror(cartfilename);
            exit(EXIT_FAILURE);
        }

        fread(vecx->cart, 1, sizeof(vecx->cart), f);
        fclose(f);
    }
}
void PC::osint_emuloop()
{
    Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
    vecx->vecx_reset();

    for (;;) {
        vecx->vecx_emu((vecx->VECTREX_MHZ / 1000) * EMU_TIMER);
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
int PC::init(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    resize(330 * 3 / 2, 410 * 3 / 2);

    if (argc > 1)
        cartfilename = argv[1];

    load_rom();
    // psg-> e8910_init_sound();
    osint_emuloop();

    psg->e8910_done_sound();
    SDL_Quit();
    return 0;
}
