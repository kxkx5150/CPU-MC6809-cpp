#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>


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

typedef int      INT32;
typedef uint64_t UINT32;
typedef char     INT8;
typedef uint8_t  UINT8;

extern uint64_t snd_regs[16];


struct AY8910
{
    int       index;
    int       ready;
    uint64_t *Regs;
    INT32     lastEnable;
    INT32     PeriodA, PeriodB, PeriodC, PeriodN, PeriodE;
    INT32     CountA, CountB, CountC, CountN, CountE;
    UINT32    VolA, VolB, VolC, VolE;
    UINT8     EnvelopeA, EnvelopeB, EnvelopeC;
    UINT8     OutputA, OutputB, OutputC, OutputN;
    INT8      CountEnv;
    UINT8     Hold, Alternate, Attack, Holding;
    INT32     RNG;
    uint64_t  VolTable[32];
} PSG;



void e8910_write(int r, int v)
{
    int old;
    if (PSG.Regs == NULL)
        return;
    PSG.Regs[r] = v;
    switch (r) {
        case AY_AFINE:
        case AY_ACOARSE:
            PSG.Regs[AY_ACOARSE] &= 0x0f;
            old         = PSG.PeriodA;
            PSG.PeriodA = (PSG.Regs[AY_AFINE] + 256 * PSG.Regs[AY_ACOARSE]) * STEP3;
            if (PSG.PeriodA == 0)
                PSG.PeriodA = STEP3;
            PSG.CountA += PSG.PeriodA - old;
            if (PSG.CountA <= 0)
                PSG.CountA = 1;
            break;
        case AY_BFINE:
        case AY_BCOARSE:
            PSG.Regs[AY_BCOARSE] &= 0x0f;
            old         = PSG.PeriodB;
            PSG.PeriodB = (PSG.Regs[AY_BFINE] + 256 * PSG.Regs[AY_BCOARSE]) * STEP3;
            if (PSG.PeriodB == 0)
                PSG.PeriodB = STEP3;
            PSG.CountB += PSG.PeriodB - old;
            if (PSG.CountB <= 0)
                PSG.CountB = 1;
            break;
        case AY_CFINE:
        case AY_CCOARSE:
            PSG.Regs[AY_CCOARSE] &= 0x0f;
            old         = PSG.PeriodC;
            PSG.PeriodC = (PSG.Regs[AY_CFINE] + 256 * PSG.Regs[AY_CCOARSE]) * STEP3;
            if (PSG.PeriodC == 0)
                PSG.PeriodC = STEP3;
            PSG.CountC += PSG.PeriodC - old;
            if (PSG.CountC <= 0)
                PSG.CountC = 1;
            break;
        case AY_NOISEPER:
            PSG.Regs[AY_NOISEPER] &= 0x1f;
            old         = PSG.PeriodN;
            PSG.PeriodN = PSG.Regs[AY_NOISEPER] * STEP3;
            if (PSG.PeriodN == 0)
                PSG.PeriodN = STEP3;
            PSG.CountN += PSG.PeriodN - old;
            if (PSG.CountN <= 0)
                PSG.CountN = 1;
            break;
        case AY_ENABLE:
            PSG.lastEnable = PSG.Regs[AY_ENABLE];
            break;
        case AY_AVOL:
            PSG.Regs[AY_AVOL] &= 0x1f;
            PSG.EnvelopeA = PSG.Regs[AY_AVOL] & 0x10;
            PSG.VolA      = PSG.EnvelopeA ? PSG.VolE : PSG.VolTable[PSG.Regs[AY_AVOL] ? PSG.Regs[AY_AVOL] * 2 + 1 : 0];
            break;
        case AY_BVOL:
            PSG.Regs[AY_BVOL] &= 0x1f;
            PSG.EnvelopeB = PSG.Regs[AY_BVOL] & 0x10;
            PSG.VolB      = PSG.EnvelopeB ? PSG.VolE : PSG.VolTable[PSG.Regs[AY_BVOL] ? PSG.Regs[AY_BVOL] * 2 + 1 : 0];
            break;
        case AY_CVOL:
            PSG.Regs[AY_CVOL] &= 0x1f;
            PSG.EnvelopeC = PSG.Regs[AY_CVOL] & 0x10;
            PSG.VolC      = PSG.EnvelopeC ? PSG.VolE : PSG.VolTable[PSG.Regs[AY_CVOL] ? PSG.Regs[AY_CVOL] * 2 + 1 : 0];
            break;
        case AY_EFINE:
        case AY_ECOARSE:
            old         = PSG.PeriodE;
            PSG.PeriodE = ((PSG.Regs[AY_EFINE] + 256 * PSG.Regs[AY_ECOARSE])) * STEP3;
            if (PSG.PeriodE == 0)
                PSG.PeriodE = STEP3 / 2;
            if (PSG.PeriodE == 0)
                PSG.PeriodE = STEP3;
            PSG.CountE += PSG.PeriodE - old;
            if (PSG.CountE <= 0)
                PSG.CountE = 1;
            break;
        case AY_ESHAPE:
            PSG.Regs[AY_ESHAPE] &= 0x0f;
            PSG.Attack = (PSG.Regs[AY_ESHAPE] & 0x04) ? 0x1f : 0x00;
            if ((PSG.Regs[AY_ESHAPE] & 0x08) == 0) {
                PSG.Hold      = 1;
                PSG.Alternate = PSG.Attack;
            } else {
                PSG.Hold      = PSG.Regs[AY_ESHAPE] & 0x01;
                PSG.Alternate = PSG.Regs[AY_ESHAPE] & 0x02;
            }
            PSG.CountE   = PSG.PeriodE;
            PSG.CountEnv = 0x1f;
            PSG.Holding  = 0;
            PSG.VolE     = PSG.VolTable[PSG.CountEnv ^ PSG.Attack];
            if (PSG.EnvelopeA)
                PSG.VolA = PSG.VolE;
            if (PSG.EnvelopeB)
                PSG.VolB = PSG.VolE;
            if (PSG.EnvelopeC)
                PSG.VolC = PSG.VolE;
            break;
        case AY_PORTA:
            break;
        case AY_PORTB:
            break;
    }
}
static void e8910_callback(void *userdata, uint8_t *stream, int length)
{
    (void)userdata;
    int      outn;
    uint8_t *buf1 = stream;
    if (!PSG.ready) {
        memset(stream, 0, length * sizeof(*stream));
        return;
    }
    length = length * 2;
    if (PSG.Regs[AY_ENABLE] & 0x01) {
        if (PSG.CountA <= STEP2)
            PSG.CountA += STEP2;
        PSG.OutputA = 1;
    } else if (PSG.Regs[AY_AVOL] == 0) {
        if (PSG.CountA <= STEP2)
            PSG.CountA += STEP2;
    }
    if (PSG.Regs[AY_ENABLE] & 0x02) {
        if (PSG.CountB <= STEP2)
            PSG.CountB += STEP2;
        PSG.OutputB = 1;
    } else if (PSG.Regs[AY_BVOL] == 0) {
        if (PSG.CountB <= STEP2)
            PSG.CountB += STEP2;
    }
    if (PSG.Regs[AY_ENABLE] & 0x04) {
        if (PSG.CountC <= STEP2)
            PSG.CountC += STEP2;
        PSG.OutputC = 1;
    } else if (PSG.Regs[AY_CVOL] == 0) {
        if (PSG.CountC <= STEP2)
            PSG.CountC += STEP2;
    }
    if ((PSG.Regs[AY_ENABLE] & 0x38) == 0x38)
        if (PSG.CountN <= STEP2)
            PSG.CountN += STEP2;
    outn = (PSG.OutputN | PSG.Regs[AY_ENABLE]);
    while (length > 0) {
        uint64_t vol;
        int      left = 2;
        int      vola, volb, volc;
        vola = volb = volc = 0;
        do {
            int nextevent;
            if (PSG.CountN < left)
                nextevent = PSG.CountN;
            else
                nextevent = left;
            if (outn & 0x08) {
                if (PSG.OutputA)
                    vola += PSG.CountA;
                PSG.CountA -= nextevent;
                while (PSG.CountA <= 0) {
                    PSG.CountA += PSG.PeriodA;
                    if (PSG.CountA > 0) {
                        PSG.OutputA ^= 1;
                        if (PSG.OutputA)
                            vola += PSG.PeriodA;
                        break;
                    }
                    PSG.CountA += PSG.PeriodA;
                    vola += PSG.PeriodA;
                }
                if (PSG.OutputA)
                    vola -= PSG.CountA;
            } else {
                PSG.CountA -= nextevent;
                while (PSG.CountA <= 0) {
                    PSG.CountA += PSG.PeriodA;
                    if (PSG.CountA > 0) {
                        PSG.OutputA ^= 1;
                        break;
                    }
                    PSG.CountA += PSG.PeriodA;
                }
            }
            if (outn & 0x10) {
                if (PSG.OutputB)
                    volb += PSG.CountB;
                PSG.CountB -= nextevent;
                while (PSG.CountB <= 0) {
                    PSG.CountB += PSG.PeriodB;
                    if (PSG.CountB > 0) {
                        PSG.OutputB ^= 1;
                        if (PSG.OutputB)
                            volb += PSG.PeriodB;
                        break;
                    }
                    PSG.CountB += PSG.PeriodB;
                    volb += PSG.PeriodB;
                }
                if (PSG.OutputB)
                    volb -= PSG.CountB;
            } else {
                PSG.CountB -= nextevent;
                while (PSG.CountB <= 0) {
                    PSG.CountB += PSG.PeriodB;
                    if (PSG.CountB > 0) {
                        PSG.OutputB ^= 1;
                        break;
                    }
                    PSG.CountB += PSG.PeriodB;
                }
            }
            if (outn & 0x20) {
                if (PSG.OutputC)
                    volc += PSG.CountC;
                PSG.CountC -= nextevent;
                while (PSG.CountC <= 0) {
                    PSG.CountC += PSG.PeriodC;
                    if (PSG.CountC > 0) {
                        PSG.OutputC ^= 1;
                        if (PSG.OutputC)
                            volc += PSG.PeriodC;
                        break;
                    }
                    PSG.CountC += PSG.PeriodC;
                    volc += PSG.PeriodC;
                }
                if (PSG.OutputC)
                    volc -= PSG.CountC;
            } else {
                PSG.CountC -= nextevent;
                while (PSG.CountC <= 0) {
                    PSG.CountC += PSG.PeriodC;
                    if (PSG.CountC > 0) {
                        PSG.OutputC ^= 1;
                        break;
                    }
                    PSG.CountC += PSG.PeriodC;
                }
            }
            PSG.CountN -= nextevent;
            if (PSG.CountN <= 0) {
                if ((PSG.RNG + 1) & 2) {
                    PSG.OutputN = ~PSG.OutputN;
                    outn        = (PSG.OutputN | PSG.Regs[AY_ENABLE]);
                }
                if (PSG.RNG & 1)
                    PSG.RNG ^= 0x24000;
                PSG.RNG >>= 1;
                PSG.CountN += PSG.PeriodN;
            }
            left -= nextevent;
        } while (left > 0);
        if (PSG.Holding == 0) {
            PSG.CountE -= STEP;
            if (PSG.CountE <= 0) {
                do {
                    PSG.CountEnv--;
                    PSG.CountE += PSG.PeriodE;
                } while (PSG.CountE <= 0);
                if (PSG.CountEnv < 0) {
                    if (PSG.Hold) {
                        if (PSG.Alternate)
                            PSG.Attack ^= 0x1f;
                        PSG.Holding  = 1;
                        PSG.CountEnv = 0;
                    } else {
                        if (PSG.Alternate && (PSG.CountEnv & 0x20))
                            PSG.Attack ^= 0x1f;
                        PSG.CountEnv &= 0x1f;
                    }
                }
                PSG.VolE = PSG.VolTable[PSG.CountEnv ^ PSG.Attack];
                if (PSG.EnvelopeA)
                    PSG.VolA = PSG.VolE;
                if (PSG.EnvelopeB)
                    PSG.VolB = PSG.VolE;
                if (PSG.EnvelopeC)
                    PSG.VolC = PSG.VolE;
            }
        }
        vol = (vola * PSG.VolA + volb * PSG.VolB + volc * PSG.VolC) / (3 * STEP);
        if (--length & 1)
            *(buf1++) = vol >> 8;
    }
}
static void e8910_build_mixer_table()
{
    int    i;
    double out;
    out = MAX_OUTPUT;
    for (i = 31; i > 0; i--) {
        PSG.VolTable[i] = (uint64_t)(out + 0.5);
        out /= 1.188502227;
    }
    PSG.VolTable[0] = 0;
}
void e8910_init_sound()
{
    //     // SDL audio stuff
    //     SDL_AudioSpec reqSpec;
    //     SDL_AudioSpec givenSpec;
    //     PSG.Regs    = snd_regs;
    //     PSG.RNG     = 1;
    //     PSG.OutputA = 0;
    //     PSG.OutputB = 0;
    //     PSG.OutputC = 0;
    //     PSG.OutputN = 0xff;
    //     e8910_build_mixer_table();
    //     PSG.ready = 1;
    //     // set up audio buffering
    //     reqSpec.freq     = SOUND_FREQ;        // Audio frequency in samples per second
    //     reqSpec.format   = AUDIO_U8;          // Audio data format
    //     reqSpec.channels = 1;                 // Number of channels: 1 mono, 2 stereo
    //     reqSpec.samples  = SOUND_SAMPLE;      // Audio buffer size in samples
    //     reqSpec.callback = e8910_callback;    // Callback function for filling the audio buffer
    //     reqSpec.userdata = NULL;
    //     if (SDL_OpenAudio(&reqSpec, &givenSpec) < 0) {
    //         fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    //         exit(-1);
    //     }
    // #if 0
    // 	fprintf(stdout, "samples:%d format=%x freq=%d\n", givenSpec.samples, givenSpec.format, givenSpec.freq);
    // #endif
    //     // Start playing audio
    //     SDL_PauseAudio(0);
}
void e8910_done_sound()
{
    // SDL_CloseAudio();
}
