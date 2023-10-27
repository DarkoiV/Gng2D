#include "Gng2D/core/main_loop.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL.h>

using Gng2D::MainLoop;
using namespace Gng2D::GLOBAL;

void MainLoop::operator()()
{
    uint64_t currentTS  = SDL_GetTicks64();
    uint32_t elapsed    = currentTS - previousTS;
    logicLag           += elapsed; 

    eventsProcessing();
    while(logicLag >= LOGIC_TICK)
    {
        logicProcessing();
        logicLag -= LOGIC_TICK;
    }
    rendering();
}

void MainLoop::eventsProcessing()
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) switch (event.type)
    {
        case SDL_QUIT:
            APP_RUNNING = false;
            break;
    }
}

void MainLoop::logicProcessing()
{
}

void MainLoop::rendering()
{
    SDL_RenderPresent(RENDERER);
    SDL_RenderClear(RENDERER);
}

