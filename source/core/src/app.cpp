#include "Gng2D/core/app.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/core/luna.hpp"

using Gng2D::LOG;
using Gng2D::Luna;
using namespace Gng2D::GLOBAL;

static void loadAppSettings()
{
    APP_DIRECTORY = SDL_GetBasePath();
    LOG::INFO("Application directory path:", APP_DIRECTORY);

    LOG::INFO("Loading config");
    Luna configReader;
    configReader.doFile(APP_DIRECTORY + "config.lua");
    auto setIntValue = [&](const std::string& name, auto& target) 
    {
        if (auto value = configReader.readGlobalInt(name); value)
        {
            target = *value;
            LOG::INFO(name, "set to", *value);
        }
    };

    setIntValue("WINDOW_WIDTH", WINDOW_WIDTH);
    setIntValue("WINDOW_HEIGHT", WINDOW_HEIGHT);
    setIntValue("RENDER_SCALE", RENDER_SCALE);
    setIntValue("LOGIC_TICK", LOGIC_TICK);
    if (auto value = configReader.readGlobalString("WINDOW_TITLE"); value)
    {
        WINDOW_TITLE = std::move(*value);
    }

    LOG::OK("Config loaded");
}

static void createSdlWindow()
{
    LOG::INFO("Creating Gng2D Window");

    LOG::INFO("Initializing SDL2 Video");
    auto err = SDL_Init(SDL_INIT_VIDEO);
    if (err) LOG::FATAL("Could not initialize SDL2 Video", SDL_GetError());

    LOG::INFO("Creating SDL2 Window");
    WINDOW = SDL_CreateWindow(WINDOW_TITLE.c_str(), 
                                 SDL_WINDOWPOS_CENTERED, 
                                 SDL_WINDOWPOS_CENTERED, 
                                 WINDOW_WIDTH * RENDER_SCALE, 
                                 WINDOW_HEIGHT * RENDER_SCALE, 
                                 0);
    if (not WINDOW) LOG::FATAL("Could not create SDL2 Window", SDL_GetError());
    LOG::INFO("Window size set to", WINDOW_WIDTH * RENDER_SCALE, "x", WINDOW_HEIGHT * RENDER_SCALE);

    LOG::INFO("Creating SDL2 Renderer for window");
    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (not RENDERER) LOG::FATAL("Could not create SDL2 Renderer", SDL_GetError());

    SDL_RenderSetLogicalSize(RENDERER, WINDOW_WIDTH, WINDOW_HEIGHT);
    LOG::INFO("Renderer logical size set to", WINDOW_WIDTH, "x", WINDOW_HEIGHT);

    LOG::OK("Created Gng2D Window");
}

static void destroySdlWindow()
{
    LOG::INFO("Destroying Gng2D Window");
    SDL_DestroyRenderer(RENDERER);
    SDL_DestroyWindow(WINDOW);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Gng2D::initApp()
{
    loadAppSettings();
    createSdlWindow();
}

void Gng2D::mainLoop()
{
    SDL_Delay(1000);
}

void Gng2D::closeApp()
{
    destroySdlWindow();
}

