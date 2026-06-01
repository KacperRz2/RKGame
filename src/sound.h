#ifndef SOUND_H_
#define SOUND_H_

typedef struct Sound Sound;

enum sound_num{
    snd_hit,
    snd_boom,
    snd_block0,
    snd_block1,
    snd_block2,
    snd_block3,
    snd_block4,
    snd_pain0,
    snd_pain1,
    snd_pain2,
    snd_pain3,
    snds_num
};

struct Sound{
    SDL_AudioSpec spec;
    Uint8 *wav_data;
    Uint32 len;
};

struct Sound_data{
    SDL_AudioStream *streams[MAX_AUDIO_STREAMS];
    Uint8 indx;
    Sound sounds[snds_num];
    SDL_AudioDeviceID device;
};

void SoundInitiation(Sound_data *const);
void PlaySound(Sound_data *const, const Uint8);
void SoundDataDestruction(Sound_data *const);

#endif
