#pragma once

#include "surface.h"
#include "template.h"

namespace PP2 {

    class Explosion {
    public:
        Explosion(Sprite *explosion_sprite, vec2 position) : current_frame(0), explosion_sprite(explosion_sprite),
                                                             position(position) {}

        bool done() const;

        void Tick();

        void Draw(Surface *screen);

        vec2 position;

        int current_frame;
        Sprite *explosion_sprite;
    };

}
