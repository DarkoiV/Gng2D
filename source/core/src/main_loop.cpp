#include "Gng2D/core/main_loop.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"

using Gng2D::MainLoop;
using namespace Gng2D::GLOBAL;

void MainLoop::operator()()
{
    while (APP_RUNNING)
        loop();
    if (CURRENT_SCENE) CURRENT_SCENE->onExit();
}

void MainLoop::loop()
{
    if (NEXT_SCENE) switchScene();
    if (not CURRENT_SCENE) LOG::FATAL("No scene set");

    uint64_t currentTS = SDL_GetTicks64();
    uint32_t elapsed   = currentTS - previousTS;
    previousTS         = currentTS;
    logicLag           += elapsed;

    eventsProcessing();
    while (logicLag >= LOGIC_TICK)
    {
        logicProcessing();
        logicLag -= LOGIC_TICK;
    }
    rendering();
}

void MainLoop::eventsProcessing()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
        switch (event.type)
        {
        case SDL_QUIT:
            APP_RUNNING = false;
            break;
        case SDL_KEYDOWN:
            CURRENT_SCENE->onKeyPress(event.key);
            break;
        }
}

void MainLoop::logicProcessing()
{
    CURRENT_SCENE->update();
}

void MainLoop::rendering()
{
    CURRENT_SCENE->render(RENDERER);
    SDL_RenderPresent(RENDERER);
    SDL_RenderClear(RENDERER);
}

void MainLoop::switchScene()
{
    LOG::INFO("Switching scene");
    if (CURRENT_SCENE)
    {
        CURRENT_SCENE->onExit();
    }

    CURRENT_SCENE = std::move(NEXT_SCENE);
    CURRENT_SCENE->onEnter();
    previousTS = SDL_GetTicks64();
}

