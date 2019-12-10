using namespace std;

#include <string>
#include <cstdio>
#include <iostream>
#include "surface.h"
#include "template.h"

using namespace PP2;

#include "prerequisites.h"
#include "ThreadPool.h"

#include "tank.h"
#include "rocket.h"
#include "smoke.h"
#include "explosion.h"
#include "particle_beam.h"

#include "game.h"

#define NUM_TANKS_BLUE 1279
#define NUM_TANKS_RED 1279

#define TANK_MAX_HEALTH 1000
#define ROCKET_HIT_VALUE 60
#define PARTICLE_BEAM_HIT_VALUE 50

#define TANK_MAX_SPEED 1.5

#define HEALTH_BARS_OFFSET_X 0
#define HEALTH_BAR_HEIGHT 70
#define HEALTH_BAR_WIDTH 1
#define HEALTH_BAR_SPACING 0

#define MAX_FRAMES 2000


//Global performance timer
#define REF_PERFORMANCE 65732 //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface *background_img = new Surface("assets/Background_Grass.png");
static Surface *tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface *tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface *rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface *rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface *particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface *smoke_img = new Surface("assets/Smoke.png");
static Surface *explosion_img = new Surface("assets/Explosion.png");

static Sprite background(background_img, 1);
static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(14, 18);
const static vec2 rocket_size(25, 24);

const static float tank_radius = 12.f;
const static float rocket_radius = 10.f;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init() {
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);

    uint rows = (uint) sqrt(NUM_TANKS_BLUE + NUM_TANKS_RED);
    uint max_rows = 12;

    float start_blue_x = tank_size.x + 10.0f;
    float start_blue_y = tank_size.y + 80.0f;

    float start_red_x = 980.0f;
    float start_red_y = 100.0f;

    float spacing = 15.0f;

    //Spawn blue tanks
    for (int i = 0; i < NUM_TANKS_BLUE; i++) {
        tanks.push_back(Tank(start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing), BLUE,
                             &tank_blue, &smoke, 1200, 600, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }
    //Spawn red tanks
    for (int i = 0; i < NUM_TANKS_RED; i++) {
        tanks.push_back(
                Tank(start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing), RED, &tank_red,
                     &smoke, 80, 80, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }

    particle_beams.push_back(Particle_beam(vec2(SCRWIDTH / 2, SCRHEIGHT / 2), vec2(100, 50), &particle_beam_sprite,
                                           PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(
            Particle_beam(vec2(80, 80), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(
            Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown() {
}

// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// -----------------------------------------------------------
Tank &Game::FindClosestEnemy(Tank &current_tank) {
    float closest_distance = numeric_limits<float>::infinity();
    int closest_index = 0;

    for (int i = 0; i < tanks.size(); i++) {
        if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active) {
            float sqrDist = fabsf((tanks.at(i).Get_Position() - current_tank.Get_Position()).sqrLength());
            if (sqrDist < closest_distance) {
                closest_distance = sqrDist;
                closest_index = i;
            }
        }
    }

    return tanks.at(closest_index);
}

// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::Update(float deltaTime) {
    //Update tanks
    for (Tank &tank : tanks) {
        if (tank.active) {
            //Check tank collision and nudge tanks away from each other
            for (Tank &oTank : tanks) {
                if (&tank == &oTank) continue;

                vec2 dir = tank.Get_Position() - oTank.Get_Position();
                float dirSquaredLen = dir.sqrLength();

                float colSquaredLen = (tank.Get_collision_radius() * tank.Get_collision_radius()) +
                                      (oTank.Get_collision_radius() * oTank.Get_collision_radius());

                if (dirSquaredLen < colSquaredLen) {
                    tank.Push(dir.normalized(), 1.f);
                }
            }

            //Move tanks according to speed and nudges (see above) also reload
            tank.Tick();

            //Shoot at closest target if reloaded
            if (tank.Rocket_Reloaded()) {
                Tank &target = FindClosestEnemy(tank);

                rockets.push_back(
                        Rocket(tank.position, (target.Get_Position() - tank.position).normalized() * 3, rocket_radius,
                               tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

                tank.Reload_Rocket();
            }
        }
    }

    //Update smoke plumes
    for (Smoke &smoke : smokes) {
        smoke.Tick();
    }

    //Update rockets
    for (Rocket &rocket : rockets) {
        rocket.Tick();

        //Check if rocket collides with enemy tank, spawn explosion and if tank is destroyed spawn a smoke plume
        for (Tank &tank : tanks) {
            if (tank.active && (tank.allignment != rocket.allignment) &&
                rocket.Intersects(tank.position, tank.collision_radius)) {
                explosions.push_back(Explosion(&explosion, tank.position));

                if (tank.hit(ROCKET_HIT_VALUE)) {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                }

                rocket.active = false;
                break;
            }
        }
    }

    //Remove exploded rockets with remove erase idiom
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket &rocket) { return !rocket.active; }),
                  rockets.end());

    //Update particle beams
    for (Particle_beam &particle_beam : particle_beams) {
        particle_beam.tick(tanks);

        //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
        for (Tank &tank : tanks) {
            if (tank.active &&
                particle_beam.rectangle.intersectsCircle(tank.Get_Position(), tank.Get_collision_radius())) {
                if (tank.hit(particle_beam.damage)) {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                }
            }
        }
    }

    //Update explosion sprites and remove when done with remove erase idiom
    for (Explosion &explosion : explosions) {
        explosion.Tick();
    }

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
                                    [](const Explosion &explosion) { return explosion.done(); }), explosions.end());
}

void Game::Draw() {
    // clear the graphics window
    screen->Clear(0);

    //Draw background
    background.Draw(screen, 0, 0);

    //Draw sprites
    for (int i = 0; i < NUM_TANKS_BLUE + NUM_TANKS_RED; i++) {
        tanks.at(i).Draw(screen);

        vec2 tPos = tanks.at(i).Get_Position();
        // tread marks
        if ((tPos.x >= 0) && (tPos.x < SCRWIDTH) && (tPos.y >= 0) && (tPos.y < SCRHEIGHT))
            background.GetBuffer()[(int) tPos.x + (int) tPos.y * SCRWIDTH] = SubBlend(
                    background.GetBuffer()[(int) tPos.x + (int) tPos.y * SCRWIDTH], 0x808080);
    }

    for (Rocket &rocket : rockets) {
        rocket.Draw(screen);
    }

    for (Smoke &smoke : smokes) {
        smoke.Draw(screen);
    }

    for (Particle_beam &particle_beam : particle_beams) {
        particle_beam.Draw(screen);
    }

    for (Explosion &explosion : explosions) {
        explosion.Draw(screen);
    }

    //Draw sorted health bars
    for (int t = 0; t < 2; t++) {
        const UINT16 NUM_TANKS = ((t < 1) ? NUM_TANKS_BLUE : NUM_TANKS_RED);

        const UINT16 begin = ((t < 1) ? 0 : NUM_TANKS_BLUE);
        std::vector<const Tank *> sorted_tanks;
        insertion_sort_tanks_health(tanks, sorted_tanks, begin, begin + NUM_TANKS);

        for (int i = 0; i < NUM_TANKS; i++) {
            int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
            int health_bar_start_y = (t < 1) ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
            int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
            int health_bar_end_y = (t < 1) ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

            screen->Bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
            screen->Bar(health_bar_start_x, health_bar_start_y + (int) ((double) HEALTH_BAR_HEIGHT * (1 -
                                                                                                      ((double) sorted_tanks.at(
                                                                                                              i)->health /
                                                                                                       (double) TANK_MAX_HEALTH))),
                        health_bar_end_x, health_bar_end_y, GREENMASK);
        }
    }
}

// -----------------------------------------------------------
// Sort tanks by health value using insertion sort
// -----------------------------------------------------------
void
PP2::Game::insertion_sort_tanks_health(const std::vector<Tank> &original, std::vector<const Tank *> &sorted_tanks,
                                       UINT16 begin, UINT16 end) {
    const UINT16 NUM_TANKS = end - begin;
    sorted_tanks.reserve(NUM_TANKS);
    sorted_tanks.emplace_back(&original.at(begin));

    for (int i = begin + 1; i < (begin + NUM_TANKS); i++) {
        const Tank &current_tank = original.at(i);

        for (int s = (int) sorted_tanks.size() - 1; s >= 0; s--) {
            const Tank *current_checking_tank = sorted_tanks.at(s);

            if ((current_checking_tank->CompareHealth(current_tank) <= 0)) {
                sorted_tanks.insert(1 + sorted_tanks.begin() + s, &current_tank);
                break;
            }

            if (s == 0) {
                sorted_tanks.insert(sorted_tanks.begin(), &current_tank);
                break;
            }
        }
    }
}

// -----------------------------------------------------------
// When we reach MAX_FRAMES print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void PP2::Game::MeasurePerformance() {
    char buffer[128];
    if (frame_count >= MAX_FRAMES) {
        if (!lock_update) {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;
        }

        frame_count--;
    }

    if (lock_update) {
        screen->Bar(420, 170, 870, 430, 0x030000);
        int ms = (int) duration % 1000, sec = ((int) duration / 1000) % 60, min = ((int) duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->Centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->Centre(screen, buffer, 340);
    }
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick(float deltaTime) {
    if (!lock_update) {
        Update(deltaTime);
    }
    Draw();

    MeasurePerformance();

    // print something in the graphics window
    //screen->Print("hello world", 2, 2, 0xffffff);

    // print something to the text window
    //cout << "This goes to the console window." << std::endl;

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->Print(screen, frame_count_string.c_str(), 350, 580);
}
