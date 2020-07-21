#include "helper.h"

using namespace sf;

Vector2f rotate(Vector2f v, float a) {
    return {v.x * (float) cos(a) - v.y * (float) sin(a), v.x * (float) sin(a) + v.y * (float) cos(a)};
}

float getLength(const Vector2f &vec) {
    if (abs(vec.x) <= FLT_EPSILON && abs(vec.y) <= FLT_EPSILON) {
        return 0.0f;
    }
    return (sqrt(pow(vec.x, 2.0f) + pow(vec.y, 2.0f)));
}

Vector2f normalize(const Vector2f &vec) {
    double factor = 1.0 / (sqrt(pow(vec.x, 2.0) + pow(vec.y, 2.0)));
    auto x = (float) (vec.x * factor);
    auto y = (float) (vec.y * factor);
    return {x, y};
}

int rand(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int rand(int max) {
    return rand(0, max);
}

float frand() {
    return (float) rand(100) / 100.f;
}

float dotP(Vector2f a, Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

Color exaggerate(Color c) {
    if (c.r == c.g && c.r == c.b) {
        return Color::Black;
    }
    if (c.r > c.g && c.r > c.b) {
        return Color(c.r, 0, 0, c.a);
    }
    if (c.g > c.r && c.g > c.b) {
        return Color(0, c.g, 0, c.a);
    }
    if (c.b > c.r && c.b > c.g) {
        return Color(0, 0, c.b, c.a);
    }
    if (c.r == c.b) {
        return Color(c.r, c.b, 0, c.a);
    }
    if (c.r == c.g) {
        return Color(c.r, 0, c.g, c.a);
    }
    if (c.b == c.g) {
        return Color(0, c.b, c.g, c.a);
    }
    return c;
}
