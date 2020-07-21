#ifndef RAYZOR_RAY_H
#define RAYZOR_RAY_H

#include <SFML/Graphics.hpp>
#include <cassert>
#include "helper.h"
#include <optional>

class Ray {
public:
    Ray(const Vector2f &root, const Vector2f &direction, float length) {
        this->root = root;
        this->direction = normalize(direction);
        this->length = length;
        this->segments.emplace_back(Line(root, getEnd()));
        this->jumps = 0;
    }

    Vector2f root;
    Vector2f direction;
    float length;
    std::vector<Line> segments;
    //Line line = Line(sf::Vector2f(), sf::Vector2f());
    Vector2f normal;
    std::optional<Color> color;
    walltype w_type;
    int jumps = 0;
    bool shot = false;


    [[nodiscard]] Vector2f getEnd() const {
        return this->root + this->direction * this->length;
    }


};


#endif //RAYZOR_RAY_H
