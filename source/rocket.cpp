#include "rocket.h"

namespace PP2 {
    Rocket::Rocket(vec2 position, vec2 direction, float collision_radius, allignments allignment, Sprite *rocket_sprite)
            : position(position), speed(direction), collision_radius(collision_radius), allignment(allignment),
              current_frame(0), rocket_sprite(rocket_sprite), active(true) {
    }

    Rocket::~Rocket() {
    }

    void Rocket::Tick() {
        position += speed;
        if (++current_frame > 8) current_frame = 0;
    }

//Draw the sprite with the facing based on this rockets movement direction
    void Rocket::Draw(Surface *screen) {
        rocket_sprite->SetFrame(((abs(speed.x) > abs(speed.y)) ? ((speed.x < 0) ? 3 : 0) : ((speed.y < 0) ? 9 : 6)) +
                                (current_frame / 3));
        rocket_sprite->Draw(screen, (int) position.x - 12, (int) position.y - 12);
    }

//Does the given circle collide with this rockets collision circle?
    bool Rocket::Intersects(vec2 position_other, float radius_other) const {
        //Uses squared lengths to remove expensive square roots
        float distance_sqr = (position_other - position).sqrLength();

        if (distance_sqr <= ((collision_radius * collision_radius) + (radius_other * radius_other))) {
            return true;
        } else {
            return false;
        }
    }

} // namespace PP2
