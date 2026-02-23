#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL_main.h> 
// ----------------------------------------------------------

#include <SDL3/SDL.h>
#include "../include/core/Game.h"
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_APP_FAILURE;
    }

    Game* game = new Game();
    *appstate = game;

    if (game->init("Magic Arena", 1344, 712)) {
        return SDL_APP_CONTINUE;
    }
    
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    Game* game = (Game*)appstate;

    static Uint64 lastTime = SDL_GetTicks();
    Uint64 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    
    if (deltaTime > 0.05f) deltaTime = 0.05f;

    game->update(deltaTime);
    game->render();

    return SDL_APP_CONTINUE; 
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    Game* game = (Game*)appstate;
    
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    
    game->handleEvents(event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    if (appstate) {
        Game* game = (Game*)appstate;
        delete game;
    }
}