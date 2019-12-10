#pragma once

#include <vector>
#include "template.h"
#include "surface.h"
#include "tank.h"

namespace PP2 {
    class Particle_beam {
    public:
        Particle_beam();

        Particle_beam(vec2 min, vec2 max, Sprite *particle_beam_sprite, int damage);

        void tick(std::vector <Tank> &tanks);

        void Draw(Surface *screen);

        vec2 min_position;
        vec2 max_position;

        Rectangle2D rectangle;

        int sprite_frame;

        int damage;

        Sprite *particle_beam_sprite;
    };
} // namespace PP2