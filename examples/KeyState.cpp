#include <SDL3/SDL.h>

int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window;
    window = SDL_CreateWindow("poop", 320, 240, SDL_WINDOW_RESIZABLE);

    SDL_Event event;

    const bool* keyState = SDL_GetKeyboardState(nullptr);

    //to run indefinitely
    bool running = true;
    while(running) {
        //Event Handling loop
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("a key was pressed: %d", event.key.key);
            }
        }

        if(keyState[SDL_SCANCODE_L]==true && keyState[SDL_SCANCODE_K]==true) {
            SDL_Log("SDL_SCANCIDE_L was pressed");
        }
        //Application/Game Logic
        //...
    }

    SDL_Quit();

    return 0;
}