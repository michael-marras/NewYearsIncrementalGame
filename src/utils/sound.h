#ifndef SOUND_H
#define SOUND_H
#include <SDL3/SDL.h>
#include <string>
#include <iostream>
#include <unordered_map>

class Sound {
    public:
        /**
         * Constructor
         */
        Sound(std::string filepath);

        /**
         * Constructor
         */
        Sound();

        /**
         * Destructor
         */ 
        ~Sound();

        /**
         * plays audio
         */
        void PlaySound();

        /**
         * stops audio
         */
        void StopSound();

        /**
         * Setup device
         */
        void SetupDevice();

        /**
         * Get Initialized sounds
         */
        std::unordered_map<std::string, Sound*> GetSounds();

    private:
        SDL_AudioDeviceID m_device;
        

        // file properties
        SDL_AudioSpec m_audioSpec;
        Uint8*        m_waveStart;
        Uint32        m_waveLength;

};
#endif