#ifndef RAYZOR_WALL_H
#define RAYZOR_WALL_H

#include "Line.h"
#include "Ray.h"
#include <optional>

class Wall : public Line {
public:

    Wall(Vector2f start, Vector2f end, walltype type = plain) : Line(start, end) {
        this->type = type;
        Vector2f direction = end - start;
        this->normal = normalize(Vector2f(-direction.y, direction.x));
    }

    Wall(Vector2f start, Vector2f end, Color color) : Wall(start, end, glass) {
        this->color = color;
    }

    Vector2f normal;
    walltype type;
    std::optional<Color> color;

    void collideRay(Ray &other) {
        if (this->collideLine(other.segments.back())) {
            other.w_type = this->type;
            other.normal = this->normal;
            other.color = this->color;
        }
    }

};
#endif //RAYZOR_WALL_H
