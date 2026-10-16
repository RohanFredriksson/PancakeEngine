#include <string>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pancake/core/inspector.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/core/window.hpp"
#include "pancake/graphics/debugdraw.hpp"

namespace Pancake {

    namespace {

        bool active = false;
        int selected = -1; // Stored as an id so a deleted entity is never dereferenced.

        // Find the entity under the mouse, first by the picking texture, then by entity bounds.
        int pick() {

            Scene* scene = Window::getScene();
            if (scene == nullptr) {return -1;}

            int x = (int) MouseListener::getX();
            int y = (int) MouseListener::getY();
            if (x >= 0 && y >= 0 && x < Window::getWidth() && y < Window::getHeight()) {
                int id = Window::readPixel(x, y);
                if (scene->getEntity(id) != nullptr) {return id;}
            }

            glm::vec2 world = glm::vec2(MouseListener::getWorldX(), MouseListener::getWorldY());
            std::vector<Entity*> entities = scene->getEntitiesAt(world);
            if (entities.empty()) {return -1;}

            // Prefer the smallest entity under the mouse, which is usually the one meant.
            Entity* best = entities[0];
            float bestArea = -1.0f;
            for (Entity* e : entities) {
                std::pair<glm::vec2, glm::vec2> b = scene->getBounds(e);
                float area = (b.second.x - b.first.x) * (b.second.y - b.first.y);
                if (bestArea < 0.0f || area < bestArea) {best = e; bestArea = area;}
            }
            return best->getId();

        }

    }

    namespace Inspector {

        void open() {
            active = true;
        }

        void close() {
            active = false;
        }

        void set(bool state) {
            active = state;
        }

        void toggle() {
            active = !active;
        }

        bool opened() {
            return active;
        }

        int getSelected() {
            return selected;
        }

        void select(int entityId) {
            selected = entityId;
        }

        void render() {

            if (!active) {return;}
            Scene* scene = Window::getScene();
            if (scene == nullptr) {return;}

            // Select the entity under the mouse when clicking on the game, not on a window.
            if (MouseListener::isMouseBeginDown(GLFW_MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
                selected = pick();
            }

            // Open on the right hand side, clear of the console.
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 370.0f, 10.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(360, io.DisplaySize.y - 20.0f), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Inspector", &active)) {ImGui::End(); return;}

            // Scene settings.
            if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::Text("Name:     %s", scene->getName().c_str());
                ImGui::Text("Entities: %d", (int) scene->getEntities().size());

                glm::vec2 gravity = scene->getPhysics()->getGravity();
                if (ImGui::DragFloat2("Gravity", &gravity[0], 0.1f)) {scene->getPhysics()->setGravity(gravity);}

                bool debug = scene->getPhysics()->hasDebugDraw();
                if (ImGui::Checkbox("Physics debug", &debug)) {scene->getPhysics()->setDebugDraw(debug);}

            }

            // Entity list, so entities without a visible shape can still be selected.
            if (ImGui::CollapsingHeader("Entities")) {
                for (Entity* e : scene->getEntities()) {
                    std::string label = std::to_string(e->getId());
                    for (Component* c : e->getComponents()) {label += " " + c->getType();}
                    if (ImGui::Selectable(label.c_str(), e->getId() == selected)) {selected = e->getId();}
                }
            }

            // The selected entity's editor.
            Entity* entity = scene->getEntity(selected);
            if (entity == nullptr || entity->isDead()) {
                ImGui::Spacing();
                ImGui::TextDisabled("Click an entity to inspect it.");
            } else {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Entity %d", entity->getId());
                ImGui::PushID(entity->getId());
                entity->imgui();
                ImGui::PopID();
                std::pair<glm::vec2, glm::vec2> b = scene->getBounds(entity);
                DebugDraw::drawAABB(b.first, b.second, vec3(1.0f, 1.0f, 0.0f), 1);
            }

            ImGui::End();

        }

    }

}
