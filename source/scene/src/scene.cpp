#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/entities/red_x.hpp"

using Gng2D::Scene;

Scene::Scene() { }

Scene::~Scene()
{
    reg.clear();
}

void Scene::onEnter()
{
    LOG::INFO("Entering", name);
    Repository::loadSprite("red_x");
    RedX spawner(reg);
    spawner.spawn();
    spawner.spawn();
    spawner.spawn();
}

void Scene::onExit()
{
    LOG::INFO("Exiting", name);
}

#ifdef GNG2D_IMGUI_ENABLED
struct IMGUI_EnableEdit
{
};

static void displayEntityInList(entt::registry& reg, entt::entity e)
{
    ImGui::BeginChild("EntityDisplay");
    ImGui::Text(" Entity %d", (uint32_t)e);
    if (ImGui::IsItemClicked())
    {
        Gng2D::LOG::INFO("CLICK");
        if (reg.all_of<IMGUI_EnableEdit>(e)) reg.remove<IMGUI_EnableEdit>(e);
        else reg.emplace<IMGUI_EnableEdit>(e);
    }
    if (reg.any_of<IMGUI_EnableEdit>(e))
    {
        std::string str{"Edit:" + std::to_string((uint32_t)e)};
        if (ImGui::BeginChild(str.c_str(), ImVec2{0, 0},
                              (ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY)))
        {
            ImGui::Text("Transform2d:");
            auto& transform = reg.get<Gng2D::Transform2d>(e);
            ImGui::InputFloat("x", &transform.x);
            ImGui::InputFloat("y", &transform.y);
            reg.patch<Gng2D::Transform2d>(e);
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

static void displayEntities(entt::registry& reg)
{
    ImGui::Begin("Scene Entities");
    auto view = reg.view<entt::entity>();
    ImGui::Text("Entity List[%zd]", view.size());
    if (ImGui::BeginChild("Entities"))
    {
        for (auto&& [e]: view.each())
            displayEntityInList(reg, e);
    }
    ImGui::EndChild();
    ImGui::End();
}
#endif

void Scene::update()
{
#ifdef GNG2D_IMGUI_ENABLED
    displayEntities(reg);
#endif
}

void Scene::render(SDL_Renderer* r)
{
    entityRenderer(r);
}

void Scene::onKeyPress(SDL_KeyboardEvent& e)
{
    inputHandler.handleKeyPress(e);
}

const std::string& Scene::getName() const
{
    return name;
}
