#ifndef SOUND_H_
#define SOUND_H_

typedef struct Sound Sound;

enum sound_num{
    snd_hit0,
    snd_hit1,
    snd_hit2,
    snd_hit3,
    snd_hit4,
    snd_hit5,
    snd_hit_last,
    snd_boom_0,
    snd_boom_last,
    snd_block0,
    snd_block1,
    snd_block2,
    snd_block3,
    snd_block_last,
    snd_pain0,
    snd_pain1,
    snd_pain2,
    snd_pain_last,
    snd_strike0,
    snd_strike1,
    snd_strike2,
    snd_strike3,
    snd_strike4,
    snd_strike5,
    snd_strike6,
    snd_strike7,
    snd_strike8,
    snd_strike9,
    snd_strike10,
    snd_strike_last,
    snd_box_last,
    snd_spell0,
    snd_spell1,
    snd_spell2,
    snd_spell3,
    snd_spell4,
    snd_spell5,
    snd_spell6,
    snd_spell_last,
    snd_push_last,
    snd_bless_last,
    snd_draw0,
    snd_draw1,
    snd_draw2,
    snd_draw3,
    snd_draw4,
    snd_draw5,
    snd_draw6,
    snd_draw7,
    snd_draw8,
    snd_draw_last,
    snd_dodge_last,
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
void PlaySoundRand(Sound_data *const, const Uint8, const Uint8);
void SoundDataDestruction(Sound_data *const);

#endif
