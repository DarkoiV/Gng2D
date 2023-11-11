#include "Gng2D/core/app.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/main_loop.hpp"
#include "Gng2D/core/repository.hpp"
#include <SDL2/SDL_image.h>

using Gng2D::LOG;
using namespace Gng2D::GLOBAL;

static void loadAppSettings()
{
    APP_DIRECTORY  = SDL_GetBasePath();
    DATA_DIRECTORY = APP_DIRECTORY + "data/";
    LOG::INFO("Application directory path:", APP_DIRECTORY);
    LOG::INFO("Data directory path:", APP_DIRECTORY);

    LOG::INFO("Loading config");
    auto config = LUNA_STATE.readFileAsTable(APP_DIRECTORY + "config.lua");
    if (auto title = config["TITLE"]; title.isString())
    {
        TITLE = title.toString();
        LOG::INFO("Title:", TITLE);
    }
    if (auto rwidth = config["RENDER_WIDTH"]; rwidth.isInteger())
    {
        RENDER_WIDTH = rwidth.toInteger();
        LOG::INFO("Render width:", RENDER_WIDTH);
    }
    if (auto rheight = config["RENDER_HEIGHT"]; rheight.isInteger())
    {
        RENDER_HEIGHT = rheight.toInteger();
        LOG::INFO("Render height:", RENDER_HEIGHT);
    }
    if (auto rscale = config["RENDER_SCALE"]; rscale.isInteger())
    {
        RENDER_SCALE = rscale.toInteger();
        LOG::INFO("Render scale:", RENDER_SCALE);
    }
    if (auto ltick = config["LOGIC_TICK"]; ltick.isInteger())
    {
        LOGIC_TICK = ltick.toInteger();
        LOG::INFO("Logic tick:", LOGIC_TICK);
    }

    LOG::OK("Config loaded");
}

static void createSdlWindow()
{
    LOG::INFO("Initializing SDL2 Video");
    auto err = SDL_Init(SDL_INIT_VIDEO);
    if (err) LOG::FATAL("Could not initialize SDL2 Video", SDL_GetError());

    LOG::INFO("Creating SDL2 Window");
    WINDOW = SDL_CreateWindow(TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              RENDER_WIDTH * RENDER_SCALE, RENDER_HEIGHT * RENDER_SCALE, 0);
    if (not WINDOW) LOG::FATAL("Could not create SDL2 Window", SDL_GetError());
    LOG::INFO("Window size set to", RENDER_WIDTH * RENDER_SCALE, "x", RENDER_HEIGHT * RENDER_SCALE);

    LOG::INFO("Creating SDL2 Renderer for window");
    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (not RENDERER) LOG::FATAL("Could not create SDL2 Renderer", SDL_GetError());

    SDL_RenderSetLogicalSize(RENDERER, RENDER_WIDTH, RENDER_HEIGHT);
    LOG::INFO("Renderer logical size set to", RENDER_WIDTH, "x", RENDER_HEIGHT);

    auto flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (IMG_Init(flags) != flags) LOG::FATAL("Failed to initialize SDL2_image:", IMG_GetError());

    LOG::OK("Created Window");
}

static void destroySdlWindow()
{
    LOG::INFO("Destroying Gng2D Window");
    IMG_Quit();
    SDL_DestroyRenderer(RENDERER);
    SDL_DestroyWindow(WINDOW);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Gng2D::initApp()
{
    LOG::INFO("App init");
    loadAppSettings();
    createSdlWindow();
}

void Gng2D::runApp()
{
    LOG::INFO("Entering main loop");
    try
    {
        MainLoop()();
    }
    catch (std::exception& e)
    {
        LOG::INFO("Exception caught:", e.what());
        if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "EXCEPTION CAUGHT", e.what(),
                                     GLOBAL::WINDOW) != 0)
        {
            LOG::ERROR("Can't show message box:", SDL_GetError());
        }
    }
}

void Gng2D::closeApp()
{
    LOG::INFO("Closing app");
    Repository::freeResources();
    destroySdlWindow();
}
