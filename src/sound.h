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
    snd_strikeA,
    snd_strikeB,
    snd_strikeC,
    snd_strikeD,
    snd_strikeE,
    snd_strikeF,
    snd_strike10,
    snd_strike11,
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
    snd_start_last,
    snd_menu_move_last,
    snd_menu_select_last,
    snd_burn0,
    snd_burn1,
    snd_burn2,
    snd_burn_last,
    snd_flame0,
    snd_flame1,
    snd_flame2,
    snd_flame_last,
    snd_thunder_last,
    snd_electic0,
    snd_electic1,
    snd_electic2,
    snd_electic_last,
    snd_hit_b0,
    snd_hit_b1,
    snd_hit_b2,
    snd_hit_b3,
    snd_hit_b4,
    snd_hit_b_last,
    snd_blacksmith_last,
    snd_paper0,
    snd_paper1,
    snd_paper2,
    snd_paper_last,
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
void PlayGameSound(Sound_data *const, const Uint8);
void PlaySoundRand(Sound_data *const, const Uint8, const Uint8);
void SoundDataDestruction(Sound_data *const);

#endif
