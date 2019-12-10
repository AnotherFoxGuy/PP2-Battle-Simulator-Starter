#include "explosion.h"

bool PP2::Explosion::done() const {
    return current_frame > 17;
}

void PP2::Explosion::Tick() {
    if (current_frame < 18) current_frame++;
}

void PP2::Explosion::Draw(Surface *screen) {
    explosion_sprite->SetFrame(current_frame / 2);
    explosion_sprite->Draw(screen, (int) position.x, (int) position.y);
}
