#include <string>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pancake/core/inspector.hpp"
#include "pancake/core/listener.hpp"
#include "pancake/core/window.hpp"
#include "pancake/graphics/debugdraw.hpp"
#include "pancake/physics/rigidbody.hpp"

namespace Pancake {

    namespace {

        bool active = false;
        int selected = -1; // Stored as an id so a deleted entity is never dereferenced.
        bool dragging = false;
        char filename[256] = "scene.json";

        int pick();

        Entity* selectedEntity() {
            Scene* scene = Window::getScene();
            if (scene == nullptr) {return nullptr;}
            Entity* e = scene->getEntity(selected);
            if (e == nullptr || e->isDead()) {return nullptr;}
            return e;
        }

        void toolbar() {

            if (ImGui::Button(Window::isPaused() ? "Play" : "Pause")) {Window::setPaused(!Window::isPaused());}
            ImGui::SameLine();
            if (ImGui::Button("Step")) {Window::step(1);}

            ImGui::SetNextItemWidth(-110.0f);
            ImGui::InputText("##File", filename, sizeof(filename));
            ImGui::SameLine();
            if (ImGui::Button("Save")) {Window::save(filename);}
            ImGui::SameLine();
            if (ImGui::Button("Load")) {Window::load(std::string(filename));}
            if (ImGui::IsItemHovered()) {ImGui::SetTooltip("Adds the file's entities to the current scene, like the load command.");}

        }

        // Select on click, then move the selection with the mouse while the button is held.
        void handleMouse() {
            if (MouseListener::isMouseBeginDown(GLFW_MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
                selected = pick();
                dragging = selectedEntity() != nullptr;
            }
            if (!MouseListener::isMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {dragging = false;}
            if (dragging && MouseListener::hasMouseMoved()) {
                Inspector::moveSelected(glm::vec2(MouseListener::getWorldDx(), MouseListener::getWorldDy()));
            }
        }

        void handleKeys() {
            if (ImGui::GetIO().WantCaptureKeyboard) {return;}
            bool control = KeyListener::isKeyDown(GLFW_KEY_LEFT_CONTROL) || KeyListener::isKeyDown(GLFW_KEY_RIGHT_CONTROL);
            if (KeyListener::isKeyBeginDown(GLFW_KEY_DELETE)) {Inspector::deleteSelected();}
            if (control && KeyListener::isKeyBeginDown(GLFW_KEY_D)) {Inspector::duplicateSelected();}
        }

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

        int duplicateSelected() {
            Entity* e = selectedEntity();
            if (e == nullptr) {return -1;}
            Entity* copy = e->clone();
            if (copy == nullptr) {return -1;}
            copy->addPosition(glm::vec2(0.5f, 0.5f)); // Offset, so the copy can be seen and grabbed.
            Window::getScene()->addEntity(copy);
            selected = copy->getId();
            return selected;
        }

        bool deleteSelected() {
            Entity* e = selectedEntity();
            if (e == nullptr) {return false;}
            e->kill();
            selected = -1;
            return true;
        }

        void moveSelected(glm::vec2 delta) {
            Entity* e = selectedEntity();
            if (e == nullptr) {return;}
            e->addPosition(delta);
            Rigidbody* r = e->getComponent<Rigidbody>();
            if (r != nullptr) {r->setVelocity(0.0f, 0.0f)->setAngularVelocity(0.0f);}
        }

        void render() {

            if (!active) {return;}
            Scene* scene = Window::getScene();
            if (scene == nullptr) {return;}

            handleMouse();
            handleKeys();

            // Open on the right hand side, clear of the console.
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 370.0f, 10.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(360, io.DisplaySize.y - 20.0f), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Inspector", &active)) {ImGui::End(); return;}

            toolbar();
            ImGui::Separator();

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
                    if (!e->getName().empty()) {label += " \"" + e->getName() + "\"";}
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
                ImGui::SameLine();
                if (ImGui::SmallButton("Duplicate")) {Inspector::duplicateSelected();}
                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {Inspector::deleteSelected();}
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
