#include <SDL3/SDL.h>
#include <string>

const Uint64 TARGETFRAMETIME = 16;

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

    //Handle input events from I/O or networking devices
    void Tick(){
        Input();
        Update();
        Render();
    }

    //Advances out loop one iteration
    void Input() {
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

    void Update() {
    }

    void Render() {
    }

    void FPSCount(Uint64* currentTick, Uint64* lastTime, Uint64* fps) {
        if (*currentTick > *lastTime + 1000) {
            *lastTime = *currentTick;
            std::string title;
            title += "Mike's SDL3 FPS: " + std::to_string(*fps);
            SDL_SetWindowTitle(window, title.c_str());
            *fps = 0;
        }
    }
    
    void TimePerFrame(Uint64* currentTick) {
        Uint64 deltaTime = SDL_GetTicks() - *currentTick;
        if (deltaTime < TARGETFRAMETIME) {
            SDL_Delay(TARGETFRAMETIME - deltaTime);
        }
    }

    //Main Application Loop
    void MainLoop() {
        Uint64 fps = 0; //number of frames per second
        Uint64 lastTime = 0;

        //inf loop
        while(running) {
            Uint64 currentTick = SDL_GetTicks();
            Tick();
            fps++;
            
            //Per frame calculation of elapsed time
            TimePerFrame(&currentTick);

            //FPS Calculation
            FPSCount(&currentTick, &lastTime, &fps);
        }
    }          
};

// Entry Point
int main(int argc, char* argv[]) {
    SDLApplication app("poo");
    app.MainLoop();
    return 0;
}