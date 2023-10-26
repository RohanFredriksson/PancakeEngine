#include "pancake/physics/force.hpp"
#include <deque>

namespace Pancake {

    ForceGenerator::ForceGenerator(std::string type) {
        this->init(type);
    }

    void ForceGenerator::init(std::string type) {
        this->type = type;
    }

    void ForceGenerator::updateForce(Rigidbody* rigidbody, float dt) {

    }

    nlohmann::json ForceGenerator::serialise() {
        nlohmann::json j;
        j.emplace("type", this->type);
        return j;
    }

    bool ForceGenerator::load(nlohmann::json j) {
        if (!j.contains("type") || !j["type"].is_string()) {return false;}
        this->init(j["type"]);
        return true;
    }

    std::string ForceGenerator::getType() {
        return this->type;
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
        this->gravity = glm::vec2(0.0f, 0.0f);
    }

    nlohmann::json Gravity::serialise() {
        nlohmann::json j = this->ForceGenerator::serialise();
        j.emplace("gravity", nlohmann::json::array());
        j["gravity"].push_back(this->gravity.x);
        j["gravity"].push_back(this->gravity.y);
        return j;
    }

    bool Gravity::load(nlohmann::json j) {
        
        if (!this->ForceGenerator::load(j)) {return false;}

        if (!j.contains("gravity") || !j["gravity"].is_array()) {return false;}
        if (j["gravity"].size() != 2) {return false;}
        if (!j["gravity"][0].is_number()) {return false;}
        if (!j["gravity"][1].is_number()) {return false;}

        this->setGravity(glm::vec2(j["gravity"][0], j["gravity"][1]));
        
        return true;

    }

    void Gravity::updateForce(Rigidbody* rigidbody, float dt)  {
        glm::vec2 acceleration = this->gravity * rigidbody->getMass();
        rigidbody->addForce(acceleration);
    }

    glm::vec2 Gravity::getGravity() {
        return this->gravity;
    }

    void Gravity::setGravity(glm::vec2 gravity)  {
        this->gravity = gravity;
    }

}