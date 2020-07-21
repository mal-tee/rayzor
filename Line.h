#ifndef RAYZOR_LINE_H
#define RAYZOR_LINE_H

#include "SFML/Graphics.hpp"
#include <optional>
#include <cmath>

using namespace sf;

class Line {

public:
    Line(Vector2f start, Vector2f end) {
        this->start = start;
        this->end = end;
        this->a = (end.y - start.y) / (end.x - start.x);
        this->b = start.y - this->a * start.x;
    }

    /**
     * Collide an*other* Line with this one. The other one will be shortened, if they collide.
     * @param other
     */
    bool collideLine(Line &other) const;

    Vector2f start;
    Vector2f end;
    //f(x) = a*x+b
    float a;
    float b;
    std::optional<Color> color;

};


#endif //RAYZOR_LINE_H
