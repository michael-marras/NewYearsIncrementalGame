#include <SDL3/SDL.h>
#include "Application.h"

int main() {
    Application app("poopie");

    if (!app.Initialize()) {
        SDL_Log("Failed to initialize application!");
        return 1;
    }

    app.MainLoop();
    return 0;
}
