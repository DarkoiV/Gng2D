#include "Gng2D/core/application.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/core/settings.hpp"

Gng2D::Application::Application()
{
    if (instance) LOG::FATAL("Cannot create two isntances of Gng2D Application");
    instance = this;

    AssetRegistry::renderer = window.renderer();
    LOG::OK("Created Gng2D application");
}

Gng2D::Application::~Application()
{
    assetRegistry.freeAllSprites();
}

void Gng2D::Application::run()
{
    onCreate();
    LOG::INFO("Switching to first scene");
    switchScene();
    while (isRunning) mainLoop();
}

void Gng2D::Application::stopRunning()
{
    LOG::INFO("Application requested to stop running");
    instance->isRunning = false;
}

void Gng2D::Application::mainLoop()
{
    auto& scene         = *currentScene;
    uint64_t currentTS  = SDL_GetTicks64();
    uint32_t elapsed    = currentTS - previousTS;
    previousTS          = currentTS;
    logicLag           += elapsed;

    eventLoop(scene);
    while (logicLag >= LOGIC_TICK)
    {
        scene();
        logicLag -= LOGIC_TICK;
    }
    scene.render(window.renderer());
    window.renderFrame();
    window.displayFPS();

    if (scene.isCompleted()) switchScene(); 
}

void Gng2D::Application::eventLoop(Scene& scene)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT: 
                onQuit();
                break;
            case SDL_MOUSEMOTION:
                scene.onMouseMotion(event.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                scene.onMouseButton(event.button);
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat) break;
                scene.onKeyDown(event.key); 
                break;
            case SDL_KEYUP:
                scene.onKeyUp(event.key);
                break;
        }
    }
}

void Gng2D::Application::switchScene()
{
    if (not nextScene) 
    {
        LOG::ERROR("No next scene set, cannot switch");
        Application::stopRunning();
    }
    else
    {
        if (currentScene) currentScene->onExit();
        currentScene = std::move(nextScene);
        currentScene->onEnter();
    }

    if (not currentScene) LOG::FATAL("No new scene after switching!");
    previousTS  = SDL_GetTicks();
    logicLag    = LOGIC_TICK;
}

