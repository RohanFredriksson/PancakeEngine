#include "pancake/components/transition.hpp"

Transition::Transition(string type) : Component(type) {
    
}

void Transition::flag() {
    this->onFlag();
    this->kill();
}

void Transition::onFlag() {

}