#include <common.h>
#include <stdlib.h>
#include <sound.h>

#define sound(num)	        (sd->sounds + num)
#define SOUND_FILES_NAMES   {\
                                "snd0_0",\
                                "snd0_1",\
                                "snd0_2",\
                                "snd0_3",\
                                "snd0_4",\
                                "snd0_5",\
                                "snd0_6",\
                                "snd1_0",\
                                "snd1_1",\
                                "snd2_0",\
                                "snd2_1",\
                                "snd2_2",\
                                "snd2_3",\
                                "snd2_4",\
                                "snd3_0",\
                                "snd3_1",\
                                "snd3_2",\
                                "snd3_3",\
                                "snd4_0",\
                                "snd4_1",\
                                "snd4_2",\
                                "snd4_3",\
                                "snd4_4",\
                                "snd4_5",\
                                "snd4_6",\
                                "snd4_7",\
                                "snd4_8",\
                                "snd4_9",\
                                "snd4_A",\
                                "snd4_B",\
                                "snd4_C",\
                                "snd4_D",\
                                "snd4_E",\
                                "snd4_F",\
                                "snd4_10",\
                                "snd4_11",\
                                "snd4_12",\
                                "snd5_0",\
                                "snd6_0",\
                                "snd6_1",\
                                "snd6_2",\
                                "snd6_3",\
                                "snd6_4",\
                                "snd6_5",\
                                "snd6_6",\
                                "snd6_7",\
                                "snd7_0",\
                                "snd8_0",\
                                "snd9_0",\
                                "snd9_1",\
                                "snd9_2",\
                                "snd9_3",\
                                "snd9_4",\
                                "snd9_5",\
                                "snd9_6",\
                                "snd9_7",\
                                "snd9_8",\
                                "snd9_9",\
                                "sndA_0",\
                                "sndB_0",\
                                "sndC_0",\
                                "sndD_0"\
                            }
#define SOUND_PATH          "%sdata/sound/%s.wav"

static inline void ToNextIndex(Sound_data *const sd){
    sd->indx = (sd->indx + 1U) % MAX_AUDIO_STREAMS;
}

static inline void ToPrevIndex(Sound_data *const sd){
    sd->indx = (sd->indx + MAX_AUDIO_STREAMS - 1U) % MAX_AUDIO_STREAMS;
}

void SoundInitiation(Sound_data *const sd){
    sd->device = 0U;
    sd->indx = 7U;
    if(!SDL_InitSubSystem(SDL_INIT_AUDIO)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL audio: %s", SDL_GetError());
        exit(-1);
    }
    sd->device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if(!sd->device) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio device: %s", SDL_GetError());
        exit(-1);
    }
    char* wav_path = NULL;
    const char* const sound_files[] = SOUND_FILES_NAMES;
    if(snds_num != SDL_arraysize(sound_files)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Bad number of sound files");
        exit(-1);
    }
    for(unsigned int i = 0U; i < SDL_arraysize(sound_files); ++i){
        SDL_asprintf(&wav_path, SOUND_PATH, SDL_GetBasePath(), *(sound_files + i));
        if(!SDL_LoadWAV(wav_path, &sound(i)->spec, &sound(i)->wav_data, &sound(i)->len)){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load .wav file: %s", SDL_GetError());
            exit(-1);
        }
        SDL_free(wav_path);
    }
    for(unsigned int i = 0U; i < SDL_arraysize(sd->streams); ++i){
        *(sd->streams + i) = SDL_CreateAudioStream(NULL, NULL);
        if(!(*(sd->streams + i))){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create audio stream: %s", SDL_GetError());
            exit(-1);
        }
    }
    if(!SDL_BindAudioStreams(sd->device, sd->streams, MAX_AUDIO_STREAMS)){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to bind stream to device: %s (%u)", SDL_GetError(), sd->device);
        exit(-1);
    }
}

void PlayGameSound(Sound_data *const sd, const Uint8 sound_num){
    const Uint8 indx = sd->indx;
    do{
        ToNextIndex(sd);
    }while(SDL_GetAudioStreamQueued(*(sd->streams + sd->indx)) > 0 && sd->indx != indx);
    SDL_SetAudioStreamFormat(*(sd->streams + sd->indx), &sound(sound_num)->spec, NULL);
    SDL_PutAudioStreamDataNoCopy(*(sd->streams + sd->indx), sound(sound_num)->wav_data, (int)sound(sound_num)->len, NULL, NULL);
    SDL_FlushAudioStream(*(sd->streams + sd->indx));
    if(indx == sd->indx){
        ToPrevIndex(sd);
    }
}

extern inline void PlaySoundRand(Sound_data *const sd, const Uint8 sound_num_from, const Uint8 sound_num_to){
    PlayGameSound(sd, SDL_rand(sound_num_to - sound_num_from + 1) + sound_num_from);
}

void SoundDataDestruction(Sound_data *const sd){
    SDL_CloseAudioDevice(sd->device);
    for(unsigned int i = 0U; i < SDL_arraysize(sd->streams); ++i){
        if(*(sd->streams + i)){
            SDL_DestroyAudioStream(*(sd->streams + i));
        }
    }
    for(unsigned int i = 0U; i < snds_num; ++i){
        SDL_free(sound(i)->wav_data);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
