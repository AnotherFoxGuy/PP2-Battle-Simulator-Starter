#include "particle_beam.h"

namespace PP2 {

    Particle_beam::Particle_beam()
            : min_position(), max_position(), particle_beam_sprite(nullptr), sprite_frame(0), rectangle(), damage(1) {
    }

    Particle_beam::Particle_beam(vec2 min, vec2 max, Sprite *particle_beam_sprite, int damage) : particle_beam_sprite(
            particle_beam_sprite), sprite_frame(0), damage(damage) {
        min_position = min;
        max_position = min + max;

        rectangle = Rectangle2D(min_position, max_position);
    }

    void Particle_beam::tick(std::vector<Tank> &tanks) {

        if (++sprite_frame == 30) {
            sprite_frame = 0;
        }
    }

    void Particle_beam::Draw(Surface *screen) {
        vec2 position = rectangle.min;

        const int offsetX = 23;
        const int offsetY = 137;

        particle_beam_sprite->SetFrame(sprite_frame / 10);
        particle_beam_sprite->Draw(screen, (int) (position.x - offsetX), (int) (position.y - offsetY));
    }

} // namespace PP2