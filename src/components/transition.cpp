#include "pancake/components/transition.hpp"

namespace Pancake {

    Transition::Transition(string type) : Component(type) {
        
    }

    void Transition::flag() {
        this->onFlag();
        this->kill();
    }

    void Transition::onFlag() {

    }

}