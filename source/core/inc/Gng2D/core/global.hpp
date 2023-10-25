#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace Gng2D::GLOBAL
{
    inline std::string      TITLE           = "Gng2D";
    inline int              RENDER_WIDTH    = 100;
    inline int              RENDER_HEIGHT   = 100;
    inline int              RENDER_SCALE    = 2;
    inline uint32_t         LOGIC_TICK      = 10;
    inline SDL_Window*      WINDOW;
    inline SDL_Renderer*    RENDERER;
    inline std::string      APP_DIRECTORY;

    void initApp();
}

