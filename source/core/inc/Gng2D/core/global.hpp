#pragma once
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/scene_interface.hpp"
#include <SDL2/SDL.h>
#include <filesystem>
#include <string>

namespace Gng2D::GLOBAL {
// CONFIFG
inline std::string TITLE         = "Gng2D";
inline int         RENDER_WIDTH  = 320;
inline int         RENDER_HEIGHT = 200;
inline int         RENDER_SCALE  = 4;
inline uint32_t    LOGIC_TICK    = 10;

// APP STATE
inline SDL_Window*           WINDOW;
inline SDL_Renderer*         RENDERER;
inline std::filesystem::path APP_DIRECTORY;
inline std::filesystem::path DATA_DIRECTORY;
inline bool                  APP_RUNNING = true;
inline Luna::State           LUNA_STATE;

inline ScenePtr CURRENT_SCENE = nullptr;
inline ScenePtr NEXT_SCENE    = nullptr;
} // namespace Gng2D::GLOBAL
