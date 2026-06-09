#include <common.h>
#include <stdlib.h>
#include <sound.h>

#define sound(num)	        (sd->sounds + num)
#define SOUNDS_PATH         "%ssound"

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
    char *sounds_path = NULL;
    SDL_asprintf(&sounds_path, SOUNDS_PATH, SDL_GetBasePath());
    SDL_IOStream *const stream = SDL_IOFromFile(sounds_path, "rb");
    SDL_free(sounds_path);
    if(!stream){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create stream from file: %s", SDL_GetError()); exit(-1);
    }
    Uint64 sound_files_num = 0U;
    SDL_ReadIO(stream, &sound_files_num, sizeof(sound_files_num));
    if(snds_num != sound_files_num){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Bad number of sound files"); exit(-1);
    }
    Uint64 positions[snds_num] = {};
    SDL_ReadIO(stream, positions, sizeof(positions));
    for(unsigned int i = 0U; i < snds_num; ++i){
        if(*(positions + i) != SDL_TellIO(stream)){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Sounds file bad position"); exit(-1);
        }
        SDL_ReadIO(stream, &sound(i)->spec, sizeof(sound(i)->spec));
        SDL_ReadIO(stream, &sound(i)->len, sizeof(sound(i)->len));
        const size_t memsize = sizeof(Uint8) * sound(i)->len;
        sound(i)->wav_data = (Uint8*)SDL_malloc(memsize);
        SDL_ReadIO(stream, sound(i)->wav_data, memsize);
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
    while(1){
        ToNextIndex(sd);
        if(0 >= SDL_GetAudioStreamQueued(*(sd->streams + sd->indx))){
            SDL_SetAudioStreamFormat(*(sd->streams + sd->indx), &sound(sound_num)->spec, NULL);
            SDL_PutAudioStreamDataNoCopy(*(sd->streams + sd->indx), sound(sound_num)->wav_data, (int)sound(sound_num)->len, NULL, NULL);
            SDL_FlushAudioStream(*(sd->streams + sd->indx));
            return;
        }else if(indx == sd->indx){
            ToPrevIndex(sd);
            return;
        }
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
