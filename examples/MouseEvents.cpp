#include <SDL3/SDL.h>

struct SDLApplication {
    SDL_Window* window;
    //to run indefinitely
    bool running = true;

    //constructor
    SDLApplication(const char* title) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(title, 320, 240, SDL_WINDOW_RESIZABLE);
    }

    //destructor
    ~SDLApplication() {
        SDL_Quit();
    }

    //Advances out loop one iteration
    void Tick() {
        SDL_Event event;

        // const bool* keyState = SDL_GetKeyboardState(nullptr);

        //Event Handling loop
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if(event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("a key was pressed: %d", event.key.key);
            }
            else if(event.type == SDL_EVENT_MOUSE_MOTION) {
                SDL_Log("x,y: %f %f", event.motion.x, event.motion.y);
                SDL_Log("xrel,yrel: %f %f", event.motion.xrel, event.motion.yrel);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    SDL_Log("left button clicked %d", event.button.button);
                }
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                    SDL_Log("middle button clicked %d", event.button.button);
                }
                if(event.button.button == SDL_BUTTON_RIGHT) {
                    SDL_Log("right button clicked %d", event.button.button);
                }
                SDL_Log("Clicks: %d", event.button.clicks);
            }
        }

        // float x,y;
        //Get the 'local' within current window mouse position
        // SDL_MouseButtonFlags mouse = SDL_GetMouseState(&x,&y);
        //Get Mouse position outside window, across multiple monitors
        // SDL_MouseButtonFlags mouse = SDL_GetMouseState(&x, &y);
        // SDL_Log("x.y: %f %f", x,y);

        //Application/Game Logic
        //...
    }
    //Main Application Loop
    void MainLoop() {
        int w,h;
        SDL_GetWindowSize(window, &w, &h);
        SDL_WarpMouseInWindow(window, w/2 , h/2);

        while(running) {
            Tick();
        }
    }          
};

// Entry Point
int main(int argc, char* argv[]) {
    SDLApplication app("poo");
    app.MainLoop();
    return 0;
}