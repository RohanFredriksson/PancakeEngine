#include "pancake/components/transition.hpp"

Transition::Transition(string type) : Component(type) {
    
}

void Transition::end() {
    this->clean();
    this->onEnd();
    this->kill();
}

void Transition::clean() {

}

void Transition::onEnd() {

}