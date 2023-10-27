#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include "Gng2D/core/scene_interface.hpp"

namespace Gng2D::GLOBAL
{
// CONFIFG
inline std::string      TITLE           = "Gng2D";
inline int              RENDER_WIDTH    = 100;
inline int              RENDER_HEIGHT   = 100;
inline int              RENDER_SCALE    = 2;
inline uint32_t         LOGIC_TICK      = 10;

// APP STATE
inline SDL_Window*      WINDOW;
inline SDL_Renderer*    RENDERER;
inline std::string      APP_DIRECTORY;
inline bool             APP_RUNNING     = true;
inline SceneInterface*  CURRENT_SCENE   = nullptr;
inline SceneInterface*  NEXT_SCENE      = nullptr;
}

