#include "Gng2D/core/app.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/main_loop.hpp"
#include "Gng2D/scene/scene.hpp"
#include <SDL2/SDL_image.h>
#include <filesystem>

using Gng2D::LOG;
using namespace Gng2D;
using namespace Gng2D::GLOBAL;
namespace fs = std::filesystem;

static std::string FIRST_SCENE;

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
    GNG2D_ASSERT(LUNA_STATE.read("Gng2D").isTable());
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

        if (not cfgTable.get("FIRST_SCENE").tryAssignTo(FIRST_SCENE))
            LOG::FATAL("First scene has to be set in config!");
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

    int windowScreenWidth  = RENDER_WIDTH * RENDER_SCALE + IMGUI_LEFT_SIDEBAR;
    int windowScreenHeight = RENDER_HEIGHT * RENDER_SCALE;

    LOG::INFO("Creating SDL2 Window");
    WINDOW = SDL_CreateWindow(TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowScreenWidth, windowScreenHeight, 0);
    if (not WINDOW) LOG::FATAL("Could not create SDL2 Window", SDL_GetError());
    LOG::INFO("Window size set to", windowScreenWidth, "x", windowScreenHeight);

    LOG::INFO("Creating SDL2 Renderer for window");
    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (not RENDERER) LOG::FATAL("Could not create SDL2 Renderer", SDL_GetError());

    SDL_RenderSetIntegerScale(RENDERER, SDL_TRUE);
    SDL_RenderSetScale(RENDERER, RENDER_SCALE, RENDER_SCALE);
    LOG::INFO("Game render size set to", RENDER_WIDTH, "x", RENDER_HEIGHT);

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

static void initImGui()
{
#ifdef GNG2D_IMGUI_ENABLED
    LOG::INFO("ImGui enabled");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(Gng2D::GLOBAL::WINDOW, Gng2D::GLOBAL::RENDERER);
    ImGui_ImplSDLRenderer2_Init(Gng2D::GLOBAL::RENDERER);
    auto& IO = ImGui::GetIO();
#endif
}

static void closeImGui()
{
#ifdef GNG2D_IMGUI_ENABLED
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
}

static void setFirstScene()
{
    if (auto fsp = Repository::getScenePath(FIRST_SCENE))
    {
        GLOBAL::NEXT_SCENE = std::make_unique<Scene>(FIRST_SCENE, *fsp);
    }
}

void Gng2D::initApp()
{
    LOG::INFO("App init");
    loadAppSettings();
    createSdlWindow();
    Repository::loadDefaultResources();
    initImGui();
    setFirstScene();
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
    closeImGui();
    destroySdlWindow();
}
