#define _USE_MATH_DEFINES

#include <cmath>
#include "enemy.h"

void Enemy::update(bool player_alive) {
    // Movement
    this->velocity = {0, 0};
    if (player_alive && canSeePos(target.position, true)) {
        this->delay = 50 + rand(10);
        // we can see the Player
        auto direction = (target.position) - this->position;
        float len = std::min(getLength(direction), 7.0f);
        this->velocity = normalize(direction) * len;
    } else {
        if (this->delay > 0) {
            this->delay--;
            return;
        }
        // move on the path
        auto target_position = this->path[currentpathtarget];
        if (getLength(position - target_position) <= (walk_step + 1.0f)) {
            if (currentpathtarget == (int) this->path.size() - 1) {
                //reached the end of the target
                this->currentpathtarget = 0;
            } else {
                this->currentpathtarget++;
            }
        }
        target_position = this->path[currentpathtarget];
        if (!canSeePos(target_position)) {
            // we can not see the target we want to move towards, so we check if we can see the other targets
            for (unsigned int i = 0; i < this->path.size(); ++i) {
                if (canSeePos(this->path[i])) {
                    this->currentpathtarget = (int) i;
                    break;
                }
            }
        }
        move_on_path();
    }
    this->angle = atan2f(this->velocity.y, this->velocity.x);
}

void Enemy::move_on_path() {
    Vector2f direction;
    direction = this->path[currentpathtarget] - this->position;
    float len = std::min(getLength(direction), walk_step);
    direction = normalize(direction) * len;
    this->velocity = direction;
}

bool Enemy::canSeePos(Vector2f pos, bool ignoreGlass) {
    Line line(this->position, pos);
    auto old_end = line.end;
    if (line.start == line.end)
        return true;
    for (auto wall: walls) {
        if (ignoreGlass && (wall.type == glass || wall.type == invisible))
            continue;
        if (wall.collideLine(line))
            return false;
    }
    return old_end == line.end;
}
