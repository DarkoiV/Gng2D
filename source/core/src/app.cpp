#include "Gng2D/core/app.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/main_loop.hpp"
#include <SDL2/SDL_image.h>
#include <filesystem>

using Gng2D::LOG;
using namespace Gng2D::GLOBAL;
namespace Luna = Gng2D::Luna;
namespace fs   = std::filesystem;

static Luna::TableRef loadConfigToTable(const fs::path& path)
{
    auto stack = LUNA_STATE.getStack();
    stack.pushGlobal("Gng2D");
    GNG2D_ASSERT(stack.is(-1) == Luna::TYPE::TABLE, "");

    stack.pushString("config");
    stack.newTable();
    stack.setTableFieldFS();
    stack.pushTableField("config");
    GNG2D_ASSERT(stack.is(-1) == Luna::TYPE::TABLE, "");

    auto cfgTable = stack.read(-1).asTable();
    LUNA_STATE.doFile(path, cfgTable);
    return cfgTable;
}

static void loadAppSettings()
{
    APP_DIRECTORY  = SDL_GetBasePath();
    DATA_DIRECTORY = APP_DIRECTORY / "data/";
    LOG::INFO("Application directory path:", APP_DIRECTORY);
    LOG::INFO("Data directory path:", DATA_DIRECTORY);

    LUNA_STATE.createTable("Gng2D");
    LOG::INFO("Created global lua table Gng2D");

    fs::path configPath = DATA_DIRECTORY / "config.lua";
    if (fs::exists(configPath))
    {
        LOG::INFO("Loading config from", configPath);
        auto cfgTable = loadConfigToTable(configPath);
        cfgTable.get("TITLE").tryAssignTo(TITLE);
        cfgTable.get("RENDER_WIDTH").tryAssignTo(RENDER_WIDTH);
        cfgTable.get("RENDER_HEIGHT").tryAssignTo(RENDER_HEIGHT);
        cfgTable.get("RENDER_SCALE").tryAssignTo(RENDER_SCALE);
        cfgTable.get("LOGIC_TICK").tryAssignTo(LOGIC_TICK);
    }
    else
    {
        LOG::INFO("No config, using default settings");
    }
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
    Repository::registerDefaultComponents();
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
