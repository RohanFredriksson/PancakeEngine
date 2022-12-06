#pragma once

#include "core/component.hpp"

class Rigidbody;

class Collider : public Component {

    private:

        Rigidbody* rigidbody;

    public:

        Collider();

        Rigidbody* getRigidbody();
        void setRigidbody(Rigidbody* rigidbody);

};

#include "physics/primitives/rigidbody.hpp"