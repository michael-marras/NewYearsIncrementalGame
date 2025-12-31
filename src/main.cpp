#include <SDL3/SDL.h>
#include <string>
#include "textures.h"

const Uint64 FPS = 100;
const Uint64 TARGETFRAMETIME = 1000 / FPS;

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TextureManager* textureManager;
    //to run indefinitely
    bool running = true;

    //constructor
    SDLApplication(const char* title) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(title, 1920, 1080, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, NULL);  // NULL = use default renderer
        textureManager = new TextureManager(renderer);
    }

    //destructor
    ~SDLApplication() {
        delete textureManager;
        
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }

    // Initialize and load all resources (textures, sounds, etc.)
    bool Initialize() {
        // Load all textures here
        if (!textureManager->LoadImageFromRes("test_img", "test_img.png")) {
            SDL_Log("Failed to load test_img!");
            return false;
        }
        
        // You can load more images here:
        // if (!textureManager->LoadImageFromRes("player", "player.png")) {
        //     SDL_Log("Failed to load player texture!");
        //     return false;
        // }
        // textureManager->LoadImageFromRes("enemy", "enemy.png");
        // textureManager->LoadImageFromRes("background", "bg.png");
        
        return true;
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
        // Clear the screen with black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        
        // Example: Render a texture by name
        TextureInfo* tex = textureManager->GetTexture("test_img");
        if (tex) {
            SDL_FRect dstRect;
            dstRect.x = 100.0f;  // X position on screen
            dstRect.y = 100.0f;  // Y position on screen
            dstRect.w = (float)tex->width;   // Width (can scale by multiplying)
            dstRect.h = (float)tex->height;   // Height (can scale by multiplying)
            
            // Render the texture at the specified position
            SDL_RenderTexture(renderer, tex->texture, NULL, &dstRect);
        }
        
        // Present the rendered frame to the screen
        SDL_RenderPresent(renderer);
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
            SDL_Delay(TARGETFRAMETIME - deltaTime - 1);
            while (SDL_GetTicks() - *currentTick < TARGETFRAMETIME) {
                // Tight loop for precision (usually < 1ms)
            }
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
    SDLApplication app("poop");

    // Initialize all resources (textures, sounds, etc.)
    if (!app.Initialize()) {
        SDL_Log("Failed to initialize application!");
        return 1;
    }

    app.MainLoop();
    return 0;
}