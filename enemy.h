#ifndef RAYZOR_ENEMY_H
#define RAYZOR_ENEMY_H

#include <utility>
#include "entitiy.h"

class Enemy : public Entity {
public:
    Enemy(std::vector<Vector2f> path, Entity &target, std::vector<Wall> &walls) : Entity(Vector2f(0, 0), 5.0f),
                                                                                  target(target), walls(walls) {
        this->path = std::move(path);
        // set the position to some random part of the path
        int max_index = this->path.size() - 1;
        int end_index = rand(0, max_index);
        int start_index = end_index > 0 ? end_index - 1 : max_index;
        this->currentpathtarget = end_index;
        float i = frand();
        // do not spawn next to the nodes
        i = std::min(i, 0.9f);
        i = std::max(i, 0.1f);
        this->position = (1 - i) * this->path[start_index] + i * this->path[end_index];
        this->scale = 1 + (float) (25 - rand(50)) / 100;
        this->radius *= (2 - scale);
        this->walk_step *= scale;
        this->health *= (2 - scale);
    }

    /**
     * We have to declare the assignment explicitly because we have reference members
     * @param e
     * @return
     */
    Enemy &operator=(const Enemy &e) {
        angle = e.angle;
        health = e.health;
        walls = e.walls;
        path = e.path;
        target = e.target;
        color = e.color;
        scale = e.scale;
        currentpathtarget = e.currentpathtarget;
        walk_step = e.walk_step;
        position = e.position;
        velocity = e.velocity;
        radius = e.radius;
        return *this;
    }

    Entity &target;
    std::vector<Wall> &walls;
    std::vector<Vector2f> path;
    float angle;
    float health = 50.f;

    Color color = Color::Magenta;

    void update(bool player_alive = true);

    float scale = 0;
private:
    int currentpathtarget;
    float walk_step = 7.0f;
    int delay = 0;

    void move_on_path();

    bool canSeePos(Vector2f pos, bool ignoreGlass = false);
};

#endif //RAYZOR_ENEMY_H
