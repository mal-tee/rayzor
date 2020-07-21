#include "Level.h"


std::vector<TileMap> Level::load_map(const std::string &name) {
    std::vector<TileMap> tmp_maps;
    for (int c = 1; c < 4; c++) {
        int lvl[10 * 20];
        int i = 0;
        std::string path = LVL_FOLDER + name + "_" + std::to_string(c) + ".csv";
        std::ifstream fin(path.c_str());
        std::string buffer;
        while (getline(fin, buffer)) {
            std::string number;
            std::stringstream stream(buffer);
            while (stream.good()) {
                getline(stream, number, ',');
                lvl[i++] = std::stoi(number);
            }
        }
        TileMap map;
        map.load(TILESHEET, TX, lvl, 20, 10);
        tmp_maps.push_back(map);
    }
    return tmp_maps;
}

std::vector<sf::Vector2f> Level::load_lights(const std::string &name) {
    std::vector<Vector2f> tmp_lights;

    std::string path = LVL_FOLDER + name + ".lights";
    std::ifstream fin(path.c_str());
    std::string buffer;
    while (getline(fin, buffer)) {
        sf::Vector2f tmp;
        std::string number;
        std::stringstream stream(buffer);

        getline(stream, number, ',');
        tmp.x = stof(number) * TX;
        getline(stream, number, ',');
        tmp.y = stof(number) * TX;
        tmp_lights.push_back(tmp);
    }
    return tmp_lights;
}

std::vector<Wall> Level::load_walls(const std::string &name) {
    std::vector<Wall> tmp_walls;
    std::string path = LVL_FOLDER + name + ".walls";
    std::ifstream fin(path.c_str());
    std::string buffer;
    // walls around the screen
    tmp_walls.emplace_back(Vector2f(0,0),Vector2f(WINDOW_WIDTH,0), invisible);
    tmp_walls.emplace_back(Vector2f(WINDOW_WIDTH,WINDOW_HEIGHT),Vector2f(WINDOW_WIDTH,0), invisible);
    tmp_walls.emplace_back(Vector2f(WINDOW_WIDTH,WINDOW_HEIGHT),Vector2f(0,WINDOW_HEIGHT), invisible);
    tmp_walls.emplace_back(Vector2f(0,0),Vector2f(0,WINDOW_HEIGHT), invisible);
    while (getline(fin, buffer)) {
        std::string number;
        std::stringstream stream(buffer);
        Vector2f start;
        Vector2f end;
        walltype type = plain;
        Color color = Color::Magenta;
        getline(stream, number, ',');
        start.x = std::stof(number) * TX;
        getline(stream, number, ',');
        start.y = std::stof(number) * TX;
        getline(stream, number, ',');
        end.x = std::stof(number) * TX;
        getline(stream, number, ',');
        end.y = std::stof(number) * TX;
        if (stream.good()) {
            getline(stream, number, ',');
            if (number == "mirror") {
                type = mirror;
            } else if (number == "invisible") {
                type = invisible;
            } else if (number == "blue") {
                type = glass;
                color = Color::Blue;
            } else if (number == "green") {
                type = glass;
                color = Color::Green;
            } else if (number == "red") {
                type = glass;
                color = Color::Red;
            }
        }
        Wall wall(start, end, type);
        wall.color = color;
        tmp_walls.emplace_back(wall);
    }

    return tmp_walls;
}

void Level::load_meta(const std::string &name) {
    std::string filepath = LVL_FOLDER + name + ".meta";
    std::ifstream fin(filepath.c_str());
    std::string buffer;
    while (getline(fin, buffer)) {
        std::string str;
        std::stringstream stream(buffer);
        getline(stream, str, ',');
        if (str == "p") {
            std::vector<Vector2f> tmp_path;
            getline(stream, str, ',');
            int count = std::stoi(str);
            while (stream.good()) {
                Vector2f node;
                getline(stream, str, ',');
                node.x = std::stof(str) * TX;
                getline(stream, str, ',');
                node.y = std::stof(str) * TX;
                tmp_path.push_back(node);
            }
            Path tmp;
            tmp.enemies = count;
            tmp.nodes = tmp_path;
            this->paths.push_back(tmp);
        } else if (str == "r") {
            getline(stream, str, ',');
            this->probability_red = std::stof(str);
        } else if (str == "g") {
            getline(stream, str, ',');
            this->probability_green = std::stof(str);
        } else if (str == "b") {
            getline(stream, str, ',');
            this->probability_blue = std::stof(str);
        } else if (str == "s") {
            Vector2f tmp_spawn;
            getline(stream, str, ',');
            tmp_spawn.x = std::stof(str) * TX;
            getline(stream, str, ',');
            tmp_spawn.y = std::stof(str) * TX;
            this->spawn = tmp_spawn;
        } else if (str == "c") {
            CoinArea rect;
            getline(stream, str, ',');
            rect.count = std::stoi(str);

            Vector2f node;
            getline(stream, str, ',');
            node.x = std::stof(str) * TX;
            getline(stream, str, ',');
            node.y = std::stof(str) * TX;
            rect.upperleft = node;

            getline(stream, str, ',');
            node.x = std::stof(str) * TX;
            getline(stream, str, ',');
            node.y = std::stof(str) * TX;
            rect.downright = node;

            this->coin_areas.push_back(rect);
        }
    }
}




