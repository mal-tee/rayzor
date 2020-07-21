#ifndef RAYZOR_LEVEL_H
#define RAYZOR_LEVEL_H

static const char *const LVL_FOLDER = "lvls/";

#include <string>
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cfloat>
#include <sstream>
#include <fstream>
#include "TileMap.h"
#include "helper.h"
#include "Wall.h"

struct Path {
    std::vector<Vector2f> nodes;
    int enemies;
};
struct CoinArea {
    // describes a axis aligned box
    Vector2f upperleft;
    Vector2f downright;
    int count = 0;
};

class Level {
public:
    explicit Level(const std::string &name) {
        // load basic map infos
        this->maps = load_map(name);
        this->lights = load_lights(name);
        this->walls = load_walls(name);
        // now load the rest
        this->load_meta(name);
    }

    std::vector<TileMap> maps;
    std::vector<Vector2f> lights;
    std::vector<Wall> walls;
    std::vector<Path> paths;
    std::vector<CoinArea> coin_areas;
    Vector2f spawn;
    bool cleared = false;
    float probability_red = 0.0f;
    float probability_green = 0.0f;
    float probability_blue = 0.0f;

private:
    static std::vector<TileMap> load_map(const std::string &name);

    static std::vector<sf::Vector2f> load_lights(const std::string &name);

    static std::vector<Wall> load_walls(const std::string &name);

    void load_meta(const std::string &name);

};


#endif //RAYZOR_LEVEL_H
