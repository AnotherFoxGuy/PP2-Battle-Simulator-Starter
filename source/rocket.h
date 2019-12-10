#pragma once

#include "template.h"
#include "tank.h"
#include "surface.h"

namespace PP2 {

    class Rocket {
    public:
        Rocket(vec2 position, vec2 direction, float collision_radius, allignments allignment, Sprite *rocket_sprite);

        ~Rocket();

        void Tick();

        void Draw(Surface *screen);

        bool Intersects(vec2 position_other, float radius_other) const;

        vec2 position;
        vec2 speed;

        float collision_radius;

        bool active;

        allignments allignment;

        int current_frame;
        Sprite *rocket_sprite;
    };

} // namespace PP2