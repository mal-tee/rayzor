#ifndef RAYZOR_PLAYER_H
#define RAYZOR_PLAYER_H

#include <SFML/Graphics.hpp>
#include "Wall.h"
#include "helper.h"

using namespace sf;

struct UnclampedColor {
    int r;
    int g;
    int b;
    int a;
};

struct PlayerProgress{
    int level = 1;
    int coins = 0;
    float shootangle = 0.4f;
    float raylength = 300;
    walltype type = plain;
    float ammo_max = 60 * 4;
};

class Entity {
public:

    explicit Entity(Vector2f position, float radius = 3.0f) {
        this->position = position;
        this->radius = radius;
    }

    Entity(Vector2f position, Vector2f velocity) : Entity(position) {
        this->velocity = velocity;

    }

    [[nodiscard]] Color getColor(const Image &image_light) const {

        UnclampedColor color{};
        int i = 0;

        addColorsave(image_light, color,
                     (unsigned int) floor(this->position.x),
                     (unsigned int) floor(this->position.y), &i);
        addColorsave(image_light, color,
                     (unsigned int) ceil(this->position.x - radius),
                     (unsigned int) ceil(this->position.y - radius), &i);
        addColorsave(image_light, color,
                     (unsigned int) floor(this->position.x + radius),
                     (unsigned int) floor(this->position.y + radius), &i);
        if (i == 0) {
            return Color(0, 0, 0, 0);
        }
        Color rtrn;
        rtrn.r = color.r / i;
        rtrn.g = color.g / i;
        rtrn.b = color.b / i;
        rtrn.a = color.a / i;
        return rtrn;
    }


    [[nodiscard]] double getBrightness(const Image &image_light) const {
        Color color = getColor(image_light);
        return (color.r + color.g + color.b) / 3.0;
    }

    bool collidesWith(Entity &other) const {
        Vector2f difference = other.position - this->position;
        return getLength(difference) < (this->radius + other.radius);
    }

    void applyMovement(const std::vector<Wall> &walls) {
        //Collision with walls
        if (this->velocity.x != 0.0 || this->velocity.y != 0.0) {
            Vector2f new_position = this->position + this->velocity;
            Line travel(this->position, new_position);
            // we already implemented collision checks between lines for the raycasting, so we just use that here too :D
            for (auto wall: walls) {
                wall.collideLine(travel);
            }

            if (getLength(travel.end - travel.start) > this->radius) {
                this->position = travel.end - this->radius * normalize(new_position - this->position);
            } else {
                this->position = travel.start;
            }

        }
    }

    Vector2f position;
    Vector2f velocity;
    float radius;
private:
    static void addColorsave(const Image &image, UnclampedColor &color, unsigned int x, unsigned int y, int *i) {
        if (x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT || x < 0 || y < 0) {
            return;
        }
        Color tmp = image.getPixel(x, y);
        color.a += tmp.a;
        color.r += tmp.r;
        color.g += tmp.g;
        color.b += tmp.b;
        (*i)++;
    }
};

class Player : public Entity {
public:
    explicit Player(Vector2f position) : Entity(position) {}

    bool hasShot = false;
    bool hasConstructed = false;
    bool isConstructing = false;
    Vector2f constructionStart;
    bool gotHit = false;
    float health = 100.0f;
    float ammo = 60 * 3;
    bool isRecharging = false;
    bool alive = true;
    float angle = 0;
};

class Coin : public Entity {
public:
    Coin(Vector2f position, float radius, int value = 1) : Entity(position, radius) {
        this->value = value;
    }

    int value;
};


#endif //RAYZOR_PLAYER_H
