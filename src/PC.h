#ifndef __PC_H
#define __PC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

class CPU;
class VECX;
class AY8910;

class PC {
  public:
    SDL_Window   *screen           = NULL;
    SDL_Renderer *renderer         = NULL;
    SDL_Surface  *overlay_original = NULL;
    SDL_Surface  *overlay          = NULL;

    long  scl_factor;
    long  offx;
    long  offy;
    char *cartfilename = NULL;

    CPU    *cpu  = nullptr;
    VECX   *vecx = nullptr;
    AY8910 *psg  = nullptr;

  public:
    PC();
    ~PC();

    void osint_render(void);
    void resize(int width, int height);
    void readevents();
    void load_overlay(const char *filename);
    void load_rom();
    void osint_emuloop();
    int  init(int argc, char *argv[]);
};
#endif
