#ifndef RAYZOR_HELPER_H
#define RAYZOR_HELPER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cfloat>

#define M_2PI 2*M_PI
static const int TX = 64;
static const char *const TILESHEET = "res/tex/tilesheet_complete.png";
using namespace sf;
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 640

enum walltype {
    plain, mirror, glass, none, invisible
};

extern Vector2f rotate(Vector2f v, float a);

extern float getLength(const Vector2f &vec);

extern Vector2f normalize(const Vector2f &vec);

extern int rand(int min, int max);

extern int rand(int max);

extern float frand();

extern float dotP(Vector2f a, Vector2f b);

Color exaggerate(Color c);
#endif //RAYZOR_HELPER_H
