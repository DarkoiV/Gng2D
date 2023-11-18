#include "Gng2D/editor/editor.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/scene/scene.hpp"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"

using Gng2D::Editor;

static void startImGui()
{
    using namespace Gng2D;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(GLOBAL::WINDOW, GLOBAL::RENDERER);
    ImGui_ImplSDLRenderer2_Init(GLOBAL::RENDERER);
}

static void newImGuiFrame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

static void setRenderScaleForScene()
{
    using namespace Gng2D;
    SDL_RenderSetLogicalSize(GLOBAL::RENDERER, GLOBAL::RENDER_WIDTH, GLOBAL::RENDER_HEIGHT);
}

static void setRenderScaleForImGui()
{
    using namespace Gng2D;
    SDL_RenderSetLogicalSize(GLOBAL::RENDERER, GLOBAL::RENDER_WIDTH * GLOBAL::RENDER_SCALE,
                             GLOBAL::RENDER_HEIGHT * GLOBAL::RENDER_SCALE);
}

static void renderImGuiFrame()
{
    using namespace Gng2D;
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

static void closeImGui()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Editor::onEnter()
{
    LOG::INFO("Entering editor");
    startImGui();
    newImGuiFrame();

    editedScene = std::make_unique<Gng2D::Scene>();
    editedScene->onEnter();

    Gng2D::Scene* scene = (Gng2D::Scene*)editedScene.get();
    sceneReg            = &(scene->reg);
}

void Editor::onExit()
{
    editedScene->onExit();
    closeImGui();
}

void Editor::update()
{
    newImGuiFrame();

    ImGui::Begin("Entities");
    auto entityView = sceneReg->view<entt::entity, Gng2D::detail::Position>();
    for (auto&& [e, pos]: entityView.each())
    {
        ImGui::Text("Entity: %d at pos: %.3gx%.3g", (uint32_t)e, pos.x, pos.y);
    }
    ImGui::End();

    editedScene->update();
    ImGui::EndFrame();
}

void Editor::render(SDL_Renderer* r)
{
    setRenderScaleForScene();
    editedScene->render(r);
    setRenderScaleForImGui();
    renderImGuiFrame();
}

void Editor::onKeyPress(SDL_KeyboardEvent& e)
{
    editedScene->onKeyPress(e);
}

void Editor::onMouseMotion(SDL_MouseMotionEvent& e)
{
    ImGui_ImplSDL2_ProcessEvent((SDL_Event*)&e);
}

void Editor::onMouseButton(SDL_MouseButtonEvent& e)
{
    ImGui_ImplSDL2_ProcessEvent((SDL_Event*)&e);
}

const std::string& Editor::getName() const
{
    return name;
}
