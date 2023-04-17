#pragma once

#include "pancake/core/component.hpp"

class Transition : public Component {

    public:

        Transition(string type);

        void flag();
        virtual void onFlag();
        
};