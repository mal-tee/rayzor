#include "Line.h"
#include <cfloat>

bool Line::collideLine(Line &other) const {
    // calc x where the lines collide
    float denominator = this->a - other.a;
    if (denominator == 0) {
        //parallel
        return false;
    }
    if (std::isnan(denominator)) {
        return false;
    }

    if (std::isinf(other.a) || std::isinf(this->a)) {
        //one of the lines is parallel to the y-axis
        //calculate the collision under the assumption that both lines are inf long
        Vector2f inf_end;
        if (std::isinf(other.a)) {
            //the other line is parallel to the y-axis
            float x = other.start.x;
            inf_end = {x, this->a * x + this->b};
        } else {
            float x = this->start.x;
            inf_end = {x, other.a * x + other.b};
        }
        if (inf_end.x < std::min({this->start.x, this->end.x}) ||
            inf_end.x > std::max({this->start.x, this->end.x})) {
            return false;
        }
        if (inf_end.x < std::min({other.start.x, other.end.x}) ||
            inf_end.x > std::max({other.start.x, other.end.x})) {
            return false;
        }

        if (inf_end.y < std::min({this->start.y, this->end.y}) ||
            inf_end.y > std::max({this->start.y, this->end.y})) {
            return false;
        }
        if (inf_end.y < std::min({other.start.y, other.end.y}) ||
            inf_end.y > std::max({other.start.y, other.end.y})) {
            return false;
        }

        other.end = inf_end;
        return true;
    }


    float x = (other.b - this->b) / denominator;
    float y = this->a * x + this->b;

    if (x < std::min({this->start.x, this->end.x}) || x > std::max({this->start.x, this->end.x})) {
        return false;
    }

    if (x < std::min({other.start.x, other.end.x}) || x > std::max({other.start.x, other.end.x})) {
        return false;
    }

    other.end.x = x;
    other.end.y = y;
    return true;
}
