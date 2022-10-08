#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "e8910.h"
#include "vecx.h"

#define SOUND_FREQ   22050
#define SOUND_SAMPLE 1024
#define MAX_OUTPUT   0x0fff

#define STEP3 1
#define STEP2 length
#define STEP  2

#define AY_AFINE    (0)
#define AY_ACOARSE  (1)
#define AY_BFINE    (2)
#define AY_BCOARSE  (3)
#define AY_CFINE    (4)
#define AY_CCOARSE  (5)
#define AY_NOISEPER (6)
#define AY_ENABLE   (7)
#define AY_AVOL     (8)
#define AY_BVOL     (9)
#define AY_CVOL     (10)
#define AY_EFINE    (11)
#define AY_ECOARSE  (12)
#define AY_ESHAPE   (13)
#define AY_PORTA    (14)
#define AY_PORTB    (15)



extern VECX   *vecx;
extern AY8910 *psg;



void AY8910::e8910_write(int r, int v)
{
    int old;
    if (Regs == NULL)
        return;
    Regs[r] = v;
    switch (r) {
        case AY_AFINE:
        case AY_ACOARSE:
            Regs[AY_ACOARSE] &= 0x0f;
            old     = PeriodA;
            PeriodA = (Regs[AY_AFINE] + 256 * Regs[AY_ACOARSE]) * STEP3;
            if (PeriodA == 0)
                PeriodA = STEP3;
            CountA += PeriodA - old;
            if (CountA <= 0)
                CountA = 1;
            break;
        case AY_BFINE:
        case AY_BCOARSE:
            Regs[AY_BCOARSE] &= 0x0f;
            old     = PeriodB;
            PeriodB = (Regs[AY_BFINE] + 256 * Regs[AY_BCOARSE]) * STEP3;
            if (PeriodB == 0)
                PeriodB = STEP3;
            CountB += PeriodB - old;
            if (CountB <= 0)
                CountB = 1;
            break;
        case AY_CFINE:
        case AY_CCOARSE:
            Regs[AY_CCOARSE] &= 0x0f;
            old     = PeriodC;
            PeriodC = (Regs[AY_CFINE] + 256 * Regs[AY_CCOARSE]) * STEP3;
            if (PeriodC == 0)
                PeriodC = STEP3;
            CountC += PeriodC - old;
            if (CountC <= 0)
                CountC = 1;
            break;
        case AY_NOISEPER:
            Regs[AY_NOISEPER] &= 0x1f;
            old     = PeriodN;
            PeriodN = Regs[AY_NOISEPER] * STEP3;
            if (PeriodN == 0)
                PeriodN = STEP3;
            CountN += PeriodN - old;
            if (CountN <= 0)
                CountN = 1;
            break;
        case AY_ENABLE:
            lastEnable = Regs[AY_ENABLE];
            break;
        case AY_AVOL:
            Regs[AY_AVOL] &= 0x1f;
            EnvelopeA = Regs[AY_AVOL] & 0x10;
            VolA      = EnvelopeA ? VolE : VolTable[Regs[AY_AVOL] ? Regs[AY_AVOL] * 2 + 1 : 0];
            break;
        case AY_BVOL:
            Regs[AY_BVOL] &= 0x1f;
            EnvelopeB = Regs[AY_BVOL] & 0x10;
            VolB      = EnvelopeB ? VolE : VolTable[Regs[AY_BVOL] ? Regs[AY_BVOL] * 2 + 1 : 0];
            break;
        case AY_CVOL:
            Regs[AY_CVOL] &= 0x1f;
            EnvelopeC = Regs[AY_CVOL] & 0x10;
            VolC      = EnvelopeC ? VolE : VolTable[Regs[AY_CVOL] ? Regs[AY_CVOL] * 2 + 1 : 0];
            break;
        case AY_EFINE:
        case AY_ECOARSE:
            old     = PeriodE;
            PeriodE = ((Regs[AY_EFINE] + 256 * Regs[AY_ECOARSE])) * STEP3;
            if (PeriodE == 0)
                PeriodE = STEP3 / 2;
            if (PeriodE == 0)
                PeriodE = STEP3;
            CountE += PeriodE - old;
            if (CountE <= 0)
                CountE = 1;
            break;
        case AY_ESHAPE:
            Regs[AY_ESHAPE] &= 0x0f;
            Attack = (Regs[AY_ESHAPE] & 0x04) ? 0x1f : 0x00;
            if ((Regs[AY_ESHAPE] & 0x08) == 0) {
                Hold      = 1;
                Alternate = Attack;
            } else {
                Hold      = Regs[AY_ESHAPE] & 0x01;
                Alternate = Regs[AY_ESHAPE] & 0x02;
            }
            CountE   = PeriodE;
            CountEnv = 0x1f;
            Holding  = 0;
            VolE     = VolTable[CountEnv ^ Attack];
            if (EnvelopeA)
                VolA = VolE;
            if (EnvelopeB)
                VolB = VolE;
            if (EnvelopeC)
                VolC = VolE;
            break;
        case AY_PORTA:
            break;
        case AY_PORTB:
            break;
    }
}
void AY8910::e8910_callback(void *userdata, uint8_t *stream, int length)
{
    (void)userdata;
    int      outn;
    uint8_t *buf1 = stream;
    if (!ready) {
        memset(stream, 0, length * sizeof(*stream));
        return;
    }
    length = length * 2;
    if (Regs[AY_ENABLE] & 0x01) {
        if (CountA <= STEP2)
            CountA += STEP2;
        OutputA = 1;
    } else if (Regs[AY_AVOL] == 0) {
        if (CountA <= STEP2)
            CountA += STEP2;
    }
    if (Regs[AY_ENABLE] & 0x02) {
        if (CountB <= STEP2)
            CountB += STEP2;
        OutputB = 1;
    } else if (Regs[AY_BVOL] == 0) {
        if (CountB <= STEP2)
            CountB += STEP2;
    }
    if (Regs[AY_ENABLE] & 0x04) {
        if (CountC <= STEP2)
            CountC += STEP2;
        OutputC = 1;
    } else if (Regs[AY_CVOL] == 0) {
        if (CountC <= STEP2)
            CountC += STEP2;
    }
    if ((Regs[AY_ENABLE] & 0x38) == 0x38)
        if (CountN <= STEP2)
            CountN += STEP2;
    outn = (OutputN | Regs[AY_ENABLE]);
    while (length > 0) {
        uint64_t vol;
        int      left = 2;
        int      vola, volb, volc;
        vola = volb = volc = 0;
        do {
            int nextevent;
            if (CountN < left)
                nextevent = CountN;
            else
                nextevent = left;
            if (outn & 0x08) {
                if (OutputA)
                    vola += CountA;
                CountA -= nextevent;
                while (CountA <= 0) {
                    CountA += PeriodA;
                    if (CountA > 0) {
                        OutputA ^= 1;
                        if (OutputA)
                            vola += PeriodA;
                        break;
                    }
                    CountA += PeriodA;
                    vola += PeriodA;
                }
                if (OutputA)
                    vola -= CountA;
            } else {
                CountA -= nextevent;
                while (CountA <= 0) {
                    CountA += PeriodA;
                    if (CountA > 0) {
                        OutputA ^= 1;
                        break;
                    }
                    CountA += PeriodA;
                }
            }
            if (outn & 0x10) {
                if (OutputB)
                    volb += CountB;
                CountB -= nextevent;
                while (CountB <= 0) {
                    CountB += PeriodB;
                    if (CountB > 0) {
                        OutputB ^= 1;
                        if (OutputB)
                            volb += PeriodB;
                        break;
                    }
                    CountB += PeriodB;
                    volb += PeriodB;
                }
                if (OutputB)
                    volb -= CountB;
            } else {
                CountB -= nextevent;
                while (CountB <= 0) {
                    CountB += PeriodB;
                    if (CountB > 0) {
                        OutputB ^= 1;
                        break;
                    }
                    CountB += PeriodB;
                }
            }
            if (outn & 0x20) {
                if (OutputC)
                    volc += CountC;
                CountC -= nextevent;
                while (CountC <= 0) {
                    CountC += PeriodC;
                    if (CountC > 0) {
                        OutputC ^= 1;
                        if (OutputC)
                            volc += PeriodC;
                        break;
                    }
                    CountC += PeriodC;
                    volc += PeriodC;
                }
                if (OutputC)
                    volc -= CountC;
            } else {
                CountC -= nextevent;
                while (CountC <= 0) {
                    CountC += PeriodC;
                    if (CountC > 0) {
                        OutputC ^= 1;
                        break;
                    }
                    CountC += PeriodC;
                }
            }
            CountN -= nextevent;
            if (CountN <= 0) {
                if ((RNG + 1) & 2) {
                    OutputN = ~OutputN;
                    outn    = (OutputN | Regs[AY_ENABLE]);
                }
                if (RNG & 1)
                    RNG ^= 0x24000;
                RNG >>= 1;
                CountN += PeriodN;
            }
            left -= nextevent;
        } while (left > 0);
        if (Holding == 0) {
            CountE -= STEP;
            if (CountE <= 0) {
                do {
                    CountEnv--;
                    CountE += PeriodE;
                } while (CountE <= 0);
                if (CountEnv < 0) {
                    if (Hold) {
                        if (Alternate)
                            Attack ^= 0x1f;
                        Holding  = 1;
                        CountEnv = 0;
                    } else {
                        if (Alternate && (CountEnv & 0x20))
                            Attack ^= 0x1f;
                        CountEnv &= 0x1f;
                    }
                }
                VolE = VolTable[CountEnv ^ Attack];
                if (EnvelopeA)
                    VolA = VolE;
                if (EnvelopeB)
                    VolB = VolE;
                if (EnvelopeC)
                    VolC = VolE;
            }
        }
        vol = (vola * VolA + volb * VolB + volc * VolC) / (3 * STEP);
        if (--length & 1)
            *(buf1++) = vol >> 8;
    }
}
void AY8910::e8910_build_mixer_table()
{
    int    i;
    double out;
    out = MAX_OUTPUT;
    for (i = 31; i > 0; i--) {
        VolTable[i] = (uint64_t)(out + 0.5);
        out /= 1.188502227;
    }
    VolTable[0] = 0;
}
void sound_cb(void *userdata, uint8_t *stream, int length)
{
    psg->e8910_callback(userdata, stream, length);
}
void AY8910::e8910_init_sound()
{
    SDL_AudioSpec reqSpec;
    SDL_AudioSpec givenSpec;
    Regs = vecx->snd_regs;

    RNG     = 1;
    OutputA = 0;
    OutputB = 0;
    OutputC = 0;
    OutputN = 0xff;
    e8910_build_mixer_table();
    ready = 1;

    reqSpec.freq     = SOUND_FREQ;      // Audio frequency in samples per second
    reqSpec.format   = AUDIO_U8;        // Audio data format
    reqSpec.channels = 1;               // Number of channels: 1 mono, 2 stereo
    reqSpec.samples  = SOUND_SAMPLE;    // Audio buffer size in samples
    reqSpec.callback = sound_cb;        // Callback function for filling the audio buffer
    reqSpec.userdata = NULL;
    if (SDL_OpenAudio(&reqSpec, &givenSpec) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_PauseAudio(0);
}
void AY8910::e8910_done_sound()
{
    SDL_CloseAudio();
}
