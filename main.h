#ifndef RAYZOR_MAIN_H
#define RAYZOR_MAIN_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <iomanip>
#include <TGUI/TGUI.hpp>
#include "enemy.h"
#include "Level.h"
#include <cfloat>
#include <sstream>
#include <fstream>
#include "Wall.h"
#include "helper.h"
#include "entitiy.h"
#include "TileMap.h"

#define RAYCOUNT 1500

static const int MAX_JUMPS = 5;
using namespace sf;
using namespace std;

void generate_walls(const vector<Wall> &walls, VertexArray &draw_lines);

void raycast(Vector2f origin, std::vector<Wall> &walls, VertexArray &draw_lines, float window = M_2PI, float angle = 0,
             int raycount = RAYCOUNT, bool shot = false, float ray_length = 1500);

void update_player(const std::vector<Wall> &walls, Player &player);

std::vector<TileMap> load_map(const std::string &name);

float delta_speed = 17;

void draw_coins(RenderWindow &window, const vector<Coin> &coins);

void prepareGameGUI(tgui::Gui &gui);

void prepareClearedGUI(tgui::Gui &gui);

void preparePauseGUI(tgui::Gui &gui);

void prepareMainMenuGUI(tgui::Gui &gui);

void prepareWonGUI(tgui::Gui &gui);

void startGame(tgui::Gui &gui, bool restart = false);

bool isKeyPressed(Keyboard::Key key);

enum gamestate {
    running, paused, mainmenu, won
};
#endif //RAYZOR_MAIN_H
