#include <common.h>
#include <stdlib.h>
#include <sound.h>

#define sound(num)	        (sd->sounds + num)
#define SOUND_FILES_NAMES   {\
                                "snd0",\
                                "snd1",\
                            }
#define SOUND_PATH          "%sdata/sound/%s.wav"

static inline void NextIndex(Sound_data *const sd){
    sd->indx = (sd->indx + 1U) % MAX_AUDIO_STREAMS;
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

void PlaySound(Sound_data *const sd, const Uint8 sound_num){
    const Uint8 indx = sd->indx;
    do{
        NextIndex(sd);
    }while(SDL_GetAudioStreamQueued(*(sd->streams + sd->indx)) > 0 && sd->indx != indx);
    SDL_SetAudioStreamFormat(*(sd->streams + sd->indx), &sound(sound_num)->spec, NULL);
    SDL_PutAudioStreamDataNoCopy(*(sd->streams + sd->indx), sound(sound_num)->wav_data, (int)sound(sound_num)->len, NULL, NULL);
    SDL_FlushAudioStream(*(sd->streams + sd->indx));
}

void SoundDataDestruction(Sound_data *const sd){
    SDL_CloseAudioDevice(sd->device);
    for (unsigned int i = 0U; i < SDL_arraysize(sd->streams); ++i) {
        if(*(sd->streams + i)){
            SDL_DestroyAudioStream(*(sd->streams + i));
        }
    }
    for (unsigned int i = 0U; i < snds_num; ++i) {
        SDL_free(sound(i)->wav_data);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
