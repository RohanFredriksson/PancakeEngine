#pragma once

#include "pancake/core/component.hpp"

namespace Pancake {

    class Transition : public Component {

        public:

            Transition(string type);

            void flag();
            virtual void onFlag();
            
    };

}