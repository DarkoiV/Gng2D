#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace Gng2D
{
struct Window
{
    Window();
    ~Window();
    void renderFrame();
    void displayFPS();
    
    SDL_Renderer*    renderer();

private:
    SDL_Window*             sdlWindow;
    SDL_Renderer*           sdlRenderer;
};
}
