#ifdef GNG2D_IMGUI_ENABLED
#include "imgui.h"                   // IWYU pragma: export
#include "imgui_impl_sdl2.h"         // IWYU pragma: export
#include "imgui_impl_sdlrenderer2.h" // IWYU pragma: export
#endif

//// ENABLED //////////////////////////////////////////////////////////////////////////////////////
#ifdef GNG2D_IMGUI_ENABLED
#define IMGUI_PROCESS_EVENT(EVENT) ImGui_ImplSDL2_ProcessEvent(&EVENT);

#define IMGUI_NEW_FRAME()                                                                          \
    do                                                                                             \
    {                                                                                              \
        ImGui_ImplSDLRenderer2_NewFrame();                                                         \
        ImGui_ImplSDL2_NewFrame();                                                                 \
        ImGui::NewFrame();                                                                         \
    } while (0)

#define IMGUI_END_FRAME() ImGui::EndFrame()

#define IMGUI_LEFT_SIDEBAR 350

#define IMGUI_RENDER_FRAME()                                                                       \
    do                                                                                             \
    {                                                                                              \
                                                                                                   \
        /* Draw red rectangle around game area */                                                  \
        SDL_SetRenderDrawColor(RENDERER, 155, 55, 25, 255);                                        \
        SDL_RenderDrawRect(RENDERER, NULL);                                                        \
        SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);                                            \
        /* Render ImGui at full resolution */                                                      \
        SDL_RenderSetViewport(RENDERER, NULL);                                                     \
        SDL_RenderSetScale(GLOBAL::RENDERER, ImGui::GetIO().DisplayFramebufferScale.x,             \
                           ImGui::GetIO().DisplayFramebufferScale.y);                              \
        ImGui::Render();                                                                           \
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());                               \
        SDL_RenderSetScale(RENDERER, RENDER_SCALE, RENDER_SCALE);                                  \
        const SDL_Rect viewport{.x = IMGUI_LEFT_SIDEBAR / RENDER_SCALE,                            \
                                .y = 0,                                                            \
                                .w = RENDER_WIDTH,                                                 \
                                .h = RENDER_HEIGHT};                                               \
        SDL_RenderSetViewport(RENDERER, &viewport);                                                \
    } while (0)

//// DISABLED //////////////////////////////////////////////////////////////////////////////////////
#else
#define IMGUI_PROCESS_EVENT(EVENT) ((void)0)
#define IMGUI_NEW_FRAME()          ((void)0)
#define IMGUI_END_FRAME()          ((void)0)
#define IMGUI_LEFT_SIDEBAR         0
#define IMGUI_RENDER_FRAME()       ((void)0)

#endif
