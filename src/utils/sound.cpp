#include "sound.h"

Sound::Sound(std::string filepath) 
    : m_device(0), m_waveStart(nullptr), m_waveLength(0) {
    
    // Load the WAV file
    if (!SDL_LoadWAV(filepath.c_str(), &m_audioSpec, &m_waveStart, &m_waveLength)) {
        std::cerr << "Failed to load WAV file: " << filepath << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    std::cout << "Successfully loaded: " << filepath << std::endl;
    std::cout << "Audio format: " << SDL_AUDIO_BITSIZE(m_audioSpec.format) << " bit" << std::endl;
    std::cout << "Channels: " << (int)m_audioSpec.channels << std::endl;
    std::cout << "Sample rate: " << m_audioSpec.freq << " Hz" << std::endl;
}

Sound::Sound() {
}

Sound::~Sound() {
    // Stop playback if active
    StopSound();
    
    // Free the audio buffer
    if (m_waveStart != nullptr) {
        SDL_free(m_waveStart);
        m_waveStart = nullptr;
    }
    
    std::cout << "Sound resources cleaned up" << std::endl;
}

void Sound::SetupDevice() {
    if (m_device != 0) {
        std::cout << "Audio device already set up" << std::endl;
        return;
    }
    
    if (m_waveStart == nullptr) {
        std::cerr << "No audio data loaded" << std::endl;
        return;
    }
    
    // Open the audio device with the desired spec
    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec);
    
    if (m_device == 0) {
        std::cerr << "Failed to open audio device" << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    std::cout << "Audio device opened successfully" << std::endl;
}

void Sound::PlaySound() {
    if (m_device == 0) {
        std::cerr << "Audio device not set up. Call SetupDevice() first" << std::endl;
        return;
    }
    
    if (m_waveStart == nullptr) {
        std::cerr << "No audio data to play" << std::endl;
        return;
    }
    
    // Create an audio stream
    SDL_AudioStream* stream = SDL_CreateAudioStream(&m_audioSpec, &m_audioSpec);
    if (stream == nullptr) {
        std::cerr << "Failed to create audio stream" << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Put the audio data into the stream
    if (!SDL_PutAudioStreamData(stream, m_waveStart, m_waveLength)) {
        std::cerr << "Failed to put audio data in stream" << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        return;
    }
    
    // Bind the stream to the device
    if (!SDL_BindAudioStream(m_device, stream)) {
        std::cerr << "Failed to bind audio stream to device" << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        return;
    }
    
    // Resume the audio device to start playback
    if (!SDL_ResumeAudioDevice(m_device)) {
        std::cerr << "Failed to resume audio device" << std::endl;
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    std::cout << "Playing sound..." << std::endl;
}

void Sound::StopSound() {
    if (m_device == 0) {
        return;
    }
    
    // Pause the audio device
    SDL_PauseAudioDevice(m_device);
    
    // Close the audio device (this will unbind and destroy all streams)
    SDL_CloseAudioDevice(m_device);
    m_device = 0;
    
    std::cout << "Sound stopped and device closed" << std::endl;
}

std::unordered_map<std::string, Sound*> Sound::GetSounds() {
    std::unordered_map<std::string, Sound*> sounds;
    Sound* sound1 = new Sound("audio/success_02-68338.wav");
    sound1->SetupDevice();
    sounds["Object break"] = sound1;
    Sound* sound2 = new Sound("audio/retro-explode-2-236688.wav");
    sound2->SetupDevice();
    sounds["Big Bang"] = sound2;

    return sounds;
}