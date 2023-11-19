#ifdef GNG2D_IMGUI_ENABLED
#include "imgui.h"                   // IWYU pragma: export
#include "imgui_impl_sdl2.h"         // IWYU pragma: export
#include "imgui_impl_sdlrenderer2.h" // IWYU pragma: export
#endif

//// ENABLED //////////////////////////////////////////////////////////////////////////////////////
#ifdef GNG2D_IMGUI_ENABLED
#define IMGUI_PROCESS_EVENT(EVENT)                                                                 \
    do                                                                                             \
    {                                                                                              \
        if (EVENT.type == SDL_MOUSEMOTION)                                                         \
        {                                                                                          \
            /* Fixes scaling issue with ImGui mouse movement */                                    \
            auto copy      = EVENT;                                                                \
            copy.motion.x *= (Sint32)GLOBAL::RENDER_SCALE;                                         \
            copy.motion.y *= (Sint32)GLOBAL::RENDER_SCALE;                                         \
            ImGui_ImplSDL2_ProcessEvent(&copy);                                                    \
        }                                                                                          \
        else ImGui_ImplSDL2_ProcessEvent(&EVENT);                                                  \
    } while (0)

#define IMGUI_NEW_FRAME()                                                                          \
    do                                                                                             \
    {                                                                                              \
        ImGui_ImplSDLRenderer2_NewFrame();                                                         \
        ImGui_ImplSDL2_NewFrame();                                                                 \
        ImGui::NewFrame();                                                                         \
    } while (0)

#define IMGUI_END_FRAME() ImGui::EndFrame()

#define IMGUI_RENDER_FRAME()                                                                       \
    do                                                                                             \
    {                                                                                              \
        /* Render ImGui at full resolution */                                                      \
        SDL_RenderSetScale(GLOBAL::RENDERER, ImGui::GetIO().DisplayFramebufferScale.x,             \
                           ImGui::GetIO().DisplayFramebufferScale.y);                              \
        ImGui::Render();                                                                           \
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());                               \
        SDL_RenderSetLogicalSize(GLOBAL::RENDERER, GLOBAL::RENDER_WIDTH, GLOBAL::RENDER_HEIGHT);   \
    } while (0)

//// DISABLED //////////////////////////////////////////////////////////////////////////////////////
#else
#define IMGUI_PROCESS_EVENT(EVENT) ((void)0)
#define IMGUI_NEW_FRAME()          ((void)0)
#define IMGUI_END_FRAME()          ((void)0)
#define IMGUI_RENDER_FRAME()       ((void)0)
#define IMGUI_WINDOW(TITLE, BODY)

#endif
