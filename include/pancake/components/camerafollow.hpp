#pragma once

#include <glm/glm.hpp>

#include "pancake/core/component.hpp"
#include "pancake/core/camera.hpp"

namespace Pancake {

    // Moves the scene camera to follow an entity: its own entity by default, or another by id.
    // Pausing the scene stops smoothed movement, as it advances with the scene's delta time.
    class CameraFollow : public Component {

        private:

            int target;          // The entity to follow, or -1 for this component's entity.
            glm::vec2 offset;    // Added to the target's position.
            float smoothing;     // Seconds for the camera to close most of the gap (about 63%). Zero snaps.
            glm::vec2 deadZone;  // Half size of a box around the camera centre the target can move in freely.
            bool bounded;
            glm::vec2 boundsMin; // The view is kept inside these bounds, or centred on them if it is larger.
            glm::vec2 boundsMax;
            bool snapNext;       // Ignore smoothing on the next update, so the camera starts on the target.

        public:

            CameraFollow();

            void start() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;
            void imgui() override;

            // Move the camera towards the target. Called by update with the scene camera and the resolved target.
            void apply(Camera* camera, Entity* target, float dt);

            int getTarget();
            glm::vec2 getOffset();
            float getSmoothing();
            glm::vec2 getDeadZone();
            bool hasBounds();
            glm::vec2 getBoundsMin();
            glm::vec2 getBoundsMax();

            CameraFollow* setTarget(Entity* entity); // nullptr follows this component's entity.
            CameraFollow* setTarget(int entityId);
            CameraFollow* setOffset(glm::vec2 offset);
            CameraFollow* setSmoothing(float seconds);
            CameraFollow* setDeadZone(glm::vec2 halfSize);
            CameraFollow* setBounds(glm::vec2 min, glm::vec2 max);
            CameraFollow* clearBounds();
            CameraFollow* snap(); // Jump straight to the target on the next update.

    };

    REGISTER(Component, CameraFollow);

}
