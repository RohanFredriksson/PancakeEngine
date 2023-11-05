#include "pancake/physics/force.hpp"
#include "pancake/core/window.hpp"
#include <deque>

namespace Pancake {

    ForceGenerator::ForceGenerator(std::string type) : Component(type) {

    }

    void ForceGenerator::updateForce(Rigidbody* rigidbody, float dt) {

    }

    void ForceGenerator::start() {
         Window::getScene()->getPhysics()->addForceGenerator(this);
    }

    void ForceGenerator::end() {
        Window::getScene()->getPhysics()->removeForceGenerator(this);
    }   

    ForceRegistration::ForceRegistration(ForceGenerator* generator, Rigidbody* rigidbody) {
        this->generator = generator;
        this->rigidbody = rigidbody;
    }

    void ForceRegistry::add(ForceGenerator* generator, Rigidbody* rigidbody) {
        ForceRegistration registration = ForceRegistration(generator, rigidbody);
        this->registry.push_back(registration);
    }

    void ForceRegistry::remove(ForceGenerator* generator, Rigidbody* rigidbody) {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            if (registration.generator == generator && registration.rigidbody == rigidbody) {
                this->registry.erase(this->registry.begin() + i);
                return;
            }
        }
    }

    void ForceRegistry::remove(ForceGenerator* generator) {
        
        std::deque<int> indices;
        for (int i = 0; i < this->registry.size(); i++) {
            if (this->registry[i].generator == generator) {indices.push_front(i);}
        }

        for (int i : indices) {
            this->registry.erase(this->registry.begin() + i);
        }

    }

    void ForceRegistry::clear() {
        this->registry.clear();
    }

    void ForceRegistry::updateForces(float dt) {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            registration.generator->updateForce(registration.rigidbody, dt);
        }
    }

    void ForceRegistry::zeroForces() {
        int n = this->registry.size();
        for (int i = 0; i < n; i++) {
            ForceRegistration registration = this->registry[i];
            registration.rigidbody->zeroForces();
        }
    }

    Gravity::Gravity() : ForceGenerator("Gravity") {
        this->acceleration = glm::vec2(0.0f, 0.0f);
    }

    nlohmann::json Gravity::serialise() {

        nlohmann::json j = this->Component::serialise();

        j.emplace("acceleration", nlohmann::json::array());
        j["acceleration"].push_back(this->acceleration.x);
        j["acceleration"].push_back(this->acceleration.y);

        return j;

    }

    bool Gravity::load(nlohmann::json j) {
        
        if (!this->Component::load(j)) {return false;}

        if (!j.contains("acceleration") || !j["acceleration"].is_array()) {return false;}
        if (j["acceleration"].size() != 2) {return false;}
        if (!j["acceleration"][0].is_number()) {return false;}
        if (!j["acceleration"][1].is_number()) {return false;}

        this->setAcceleration(glm::vec2(j["acceleration"][0], j["acceleration"][1]));
        
        return true;

    }

    void Gravity::updateForce(Rigidbody* rigidbody, float dt)  {
        glm::vec2 force = this->acceleration * rigidbody->getMass();
        rigidbody->addForce(force);
    }

    glm::vec2 Gravity::getAcceleration() {
        return this->acceleration;
    }

    void Gravity::setAcceleration(glm::vec2 acceleration)  {
        this->acceleration = acceleration;
    }

    void Gravity::setAcceleration(float x, float y) {
        this->setAcceleration(glm::vec2(x, y));
    }

}