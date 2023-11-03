#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include "Gng2D/commons/scene_interface.hpp"

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

using ScenePtr = std::unique_ptr<SceneInterface>;
inline ScenePtr         CURRENT_SCENE   = nullptr;
inline ScenePtr         NEXT_SCENE      = nullptr;
}

