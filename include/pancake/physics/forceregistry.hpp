#pragma once

#include <vector>
#include "pancake/physics/forcegenerator.hpp"
#include "pancake/physics/rigidbody.hpp"

using std::vector;

class ForceRegistry;

class ForceRegistration {

    public:

        ForceGenerator* generator;
        Rigidbody* rigidbody;

        ForceRegistration(ForceGenerator* generator, Rigidbody* rigidbody);

};

class ForceRegistry {

    private:
        vector<ForceRegistration> registry;

    public:

        void add(ForceGenerator* generator, Rigidbody* rigidbody);
        void remove(ForceGenerator* generator, Rigidbody* rigidbody);
        void clear();
        void updateForces(float dt);
        void zeroForces();

};