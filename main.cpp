#include "main.h"

using namespace std;
using namespace sf;

Color ray_color = Color(255, 255, 255, 80);
Clock cl;
gamestate state = mainmenu;
bool gameStateChanged = true;
bool windowsHasFocus = true;
float ammountRecharge = 90;
int fps = 60;
float cost_wall = 5;
float cost_shot = 1;
int max_level = 4;

unique_ptr<Level> lvl;
unique_ptr<vector<Enemy>> enemies;
unique_ptr<vector<Coin>> coins;
shared_ptr<Player> player;
String current_level;
PlayerProgress progress;
PlayerProgress save;
bool save_initialized = false;

void load_level(const std::string &name) {
    if (!save_initialized) {
        save = progress;
        save_initialized = true;
    } else {
        progress = save;
    }
    lvl = make_unique<Level>(Level(name));

    player = make_shared<Player>(Player(lvl->spawn));
    player->radius = 10.0f;

    //create enemies
    enemies = make_unique<vector<Enemy>>();
    for (Path &path: lvl->paths) {
        for (int i = 0; i < path.enemies; ++i) {
            Enemy tmp(path.nodes, *player, lvl->walls);
            Color color = Color::White;
            if (frand() < lvl->probability_red) {
                color = Color::Red;
            }
            if (frand() < lvl->probability_green) {
                color = Color::Green;
            }
            if (frand() < lvl->probability_blue) {
                color = Color::Blue;
            }
            tmp.color = color;
            enemies->emplace_back(tmp);
        }
    }
    coins = make_unique<vector<Coin>>();
    for (CoinArea &ca: lvl->coin_areas) {
        float x_max_delta = ca.downright.x - ca.upperleft.x;
        float y_max_delta = ca.downright.y - ca.upperleft.y;
        for (int i = 0; i < ca.count; ++i) {
            Vector2f position = ca.upperleft;
            position += frand() * x_max_delta * Vector2f(1, 0);
            position += frand() * y_max_delta * Vector2f(0, 1);
            coins->emplace_back(position, 8.0f);
        }
    }
    current_level = name;
}

void load_next_level() {
    progress.level++;
    save = progress;
    if (progress.level <= max_level) {
        load_level("map" + to_string(progress.level));
    } else {
        state = won;
        gameStateChanged = true;
    }
}

int main(int argc, char *argv[]) {
    Sprite sprite_player;
    Sprite sprite_robot;
    Texture survivor;
    Texture survivor_reload;
    Texture robot;
    survivor.loadFromFile("res/tex/survivor/survivor1_gun.png");
    survivor_reload.loadFromFile("res/tex/survivor/survivor1_reload.png");
    robot.loadFromFile("res/tex/robot/robot1_stand.png");
    sprite_player.setTexture(survivor);
    Color color_player = sprite_player.getColor();
    sprite_robot.setTexture(robot);
    sprite_player.setOrigin(14, 21);
    sprite_robot.setOrigin(14, 21);

    srand((unsigned int) time(nullptr));
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    if (argc == 2 && string("-16") == argv[1])
        settings.antialiasingLevel = 16;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Rayzor", sf::Style::Default, settings);
    window.setFramerateLimit(fps);

    tgui::Gui gui(window);

    sf::RenderTexture rendertexture_light;
    rendertexture_light.create(WINDOW_WIDTH, WINDOW_HEIGHT, settings);
    sf::RenderTexture rendertexture_bg;
    rendertexture_bg.create(WINDOW_WIDTH, WINDOW_HEIGHT, settings);


    load_level("map1");

    VertexArray draw_lines(Lines);
    draw_lines.resize(lvl->walls.size() * 2);
    VertexArray draw_lines_light(Lines);
    draw_lines_light.resize(RAYCOUNT * 3);
    VertexArray draw_lines_light_shots(Lines);
    draw_lines_light_shots.resize(RAYCOUNT * 2);

    Vector2f mouse_position;

    Clock clock;
    Time updateTime = sf::milliseconds(1000 / fps);
    Time elapsed = clock.restart();

    while (window.isOpen()) {
        elapsed = elapsed + clock.restart();
        while (window.isOpen() && elapsed > updateTime) {

            //reset
            window.clear();
            draw_lines.clear();
            draw_lines_light.clear();
            draw_lines_light_shots.clear();
            rendertexture_light.clear();
            rendertexture_bg.clear();
            player->hasShot = false;

            sf::Event event{};
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Space) {
                            if (state == running && !player->isConstructing) {
                                player->isConstructing = true;
                                player->constructionStart = player->position;
                            }
                        } else if (event.key.code == sf::Keyboard::C) {
                            load_next_level();
                        } else if (event.key.code == sf::Keyboard::R) {
                            //reload level
                            load_level(current_level);
                        }
                        break;
                    case sf::Event::KeyReleased: {
                        if (event.key.code == sf::Keyboard::Escape) {
                            if (state == paused) {
                                state = running;
                                gameStateChanged = true;
                            } else if (state == running) {
                                state = paused;
                                gameStateChanged = true;
                            }
                        } else if (event.key.code == sf::Keyboard::Space) {
                            if (state == running && player->isConstructing) {
                                player->hasConstructed = true;
                                player->isConstructing = false;
                            }
                        }
                        break;
                    }
                    case sf::Event::GainedFocus:
                        windowsHasFocus = true;
                        break;
                    case sf::Event::LostFocus:
                        windowsHasFocus = false;
                        break;
                    default:
                        gui.handleEvent(event);
                        break;
                }
            }

            if (gameStateChanged) {
                gameStateChanged = false;
                gui.removeAllWidgets();
                switch (state) {
                    case mainmenu:
                        prepareMainMenuGUI(gui);
                        break;
                    case paused:
                        preparePauseGUI(gui);
                        break;
                    case running:
                        prepareGameGUI(gui);
                        prepareClearedGUI(gui);
                        break;
                    case won:
                        prepareWonGUI(gui);
                        break;
                }
            }

            mouse_position = Vector2f(Mouse::getPosition(window));

            // GAME LOGIC
            if (state == running) {
                if (player->alive) {
                    update_player(lvl->walls, *player);
                    float angle =
                            (float) M_PI +
                            atan2f(mouse_position.y - player->position.y, mouse_position.x - player->position.x);

                    //convert to other clock direction
                    angle = (float) M_PI - angle;
                    angle += (float) M_PI / 2.0;
                    player->angle = angle;
                }

                //https://stackoverflow.com/a/13102374
                auto it = coins->begin();
                while (it != coins->end()) {
                    if (it->collidesWith(*player)) {
                        progress.coins += it->value;
                        it = coins->erase(it);
                    } else {
                        ++it;
                    }
                }

                if (player->alive && coins->empty() && enemies->empty()) {
                    lvl->cleared = true;
                    save = progress;
                }

                if (player->alive && player->hasShot && !player->isRecharging && (player->ammo > 0)) {
                    //player shooting
                    player->ammo -= cost_shot;
                    float angle = player->angle;
                    angle -= progress.shootangle / 2.0f;
                    // we want the rays to come out of the gun and not out of the players body
                    Vector2f delta_weapon(-8, 28);
                    auto ray_start = player->position + rotate(delta_weapon, -player->angle);
                    // check if there is a wall between the gun and the players hitbox
                    Line checkline(player->position, ray_start);
                    for (auto &wall: lvl->walls) {
                        if (wall.type != invisible) {
                            if (wall.collideLine(checkline)) {
                                // there is at least one collision
                                break;
                            }
                        }
                    }
                    if (checkline.end == ray_start) {
                        // there is nothing between the player and the gun, so we can shoot
                        raycast(ray_start, lvl->walls, draw_lines_light_shots, progress.shootangle, angle, RAYCOUNT / 4,
                                true, progress.raylength);
                    }
                } else if (player->alive && player->hasConstructed) {
                    //player placed a wall
                    if (getLength(player->position - player->constructionStart) >= (2 * player->radius)) {
                        Vector2f end = player->position -
                                       (1.5f * player->radius) *
                                       normalize(player->position - player->constructionStart);

                        lvl->walls.emplace_back(player->constructionStart, end, progress.type);
                    }
                    player->hasConstructed = false;
                } else if (player->alive && player->isConstructing) {
                    //player is placing a wall;
                    if (!player->isRecharging && player->ammo > cost_wall) {
                        player->ammo -= cost_wall;
                    } else {
                        player->isConstructing = false;
                        player->hasConstructed = true;
                    }

                } else if (player->alive) {
                    if (player->ammo == 0.0f && !player->isRecharging) {
                        player->isRecharging = true;
                    }
                    if (player->isRecharging && player->ammo > ammountRecharge) {
                        player->isRecharging = false;
                    }
                    player->ammo = std::min(player->ammo + 1.0f, progress.ammo_max);
                }
            }
            if (state != paused) {
                //https://stackoverflow.com/a/13102374
                auto it = enemies->begin();
                while (it != enemies->end()) {
                    if (player->alive && it->collidesWith(*player)) {
                        player->health -= 5.0f;
                        player->gotHit = true;
                    }
                    // erase enemy if he is dead
                    if (it->health <= 0.f) {
                        it = enemies->erase(it);
                    } else {
                        it->update(state == running && player->alive);
                        it->applyMovement(lvl->walls);
                        ++it;
                    }
                }
                if (player->health <= 0) {
                    player->alive = false;
                }
            }
            if (state != mainmenu) {
                //ambient light around player
                if (player->alive) {
                    raycast(player->position, lvl->walls, draw_lines_light);
                }
            }

            // RENDER STUFF
            generate_walls(lvl->walls, draw_lines);
            for (auto light :lvl->lights) {
                raycast(light, lvl->walls, draw_lines_light);
            }

            for (const auto &map: lvl->maps) {
                rendertexture_bg.draw(map);
            }
            rendertexture_bg.display();

            rendertexture_light.draw(draw_lines_light_shots);
            rendertexture_light.display();
            sf::Texture texture_light = rendertexture_light.getTexture();
            Image image_light_shots;
            if (player->hasShot) {
                // this is *really* expensive :(
                // so we only do it if we have to, i.e. if the player shot
                image_light_shots = texture_light.copyToImage();
            }
            // TODO maybe move map light to own texture so we do not have to recalculate it every frame
            rendertexture_light.clear();
            rendertexture_light.draw(draw_lines_light);
            rendertexture_light.draw(draw_lines_light_shots);
            rendertexture_light.display();
            texture_light = rendertexture_light.getTexture();
            Sprite sprite_light(texture_light);

            //draw the background
            sf::Texture bg = rendertexture_bg.getTexture();
            sf::Sprite sprite_bg(bg);
            window.draw(sprite_bg);

            //draw enemies
            for (auto &e: *enemies) {
                sprite_robot.setPosition(e.position);
                sprite_robot.setRotation(e.angle * 180.0f / (float) M_PI);
                float scale = (2 - e.scale) * 0.8f;
                sprite_robot.setScale({scale, scale});
                /*sf::CircleShape tmp;
                tmp.setOrigin(e.radius, e.radius);
                tmp.setPosition(e.position);
                tmp.setRadius(e.radius);
                tmp.setFillColor(e.color);*/
                Color check;
                if (player->hasShot) {
                    check = e.getColor(image_light_shots);
                    check = exaggerate(check);
                    check = check * e.color;
                }
                if (player->hasShot && e.color == Color::White && e.getBrightness(image_light_shots) > 20) {
                    sprite_robot.setColor(Color::Magenta);
                    e.health -= 1.0f;
                } else if (player->hasShot && e.color != Color::White && (check.r + check.g + check.b) > 100) {
                    sprite_robot.setColor(Color::Magenta);
                    e.health -= 1.0f;
                } else {
                    sprite_robot.setColor(e.color);
                }
                window.draw(sprite_robot);
                //window.draw(tmp);
            }

            //draw coins
            draw_coins(window, *coins);

            //multiply the background with the light
            window.draw(sprite_light, BlendMultiply);

            //draw everything else on top
            window.draw(draw_lines);

            if (state != mainmenu && player->alive) {
                //draw player
                sprite_player.setPosition(player->position);
                sprite_player.setRotation((player->angle * -180.f / (float) M_PI) + 90.0f);
                sprite_player.setScale({0.8f, 0.8f});
                if (player->isRecharging) {
                    sprite_player.setTexture(survivor_reload, true);
                } else {
                    sprite_player.setTexture(survivor, true);
                }
                if (player->isConstructing) {
                    sprite_player.setColor(Color::Yellow);
                } else {
                    sprite_player.setColor(color_player);
                }
                window.draw(sprite_player);

                /*sf::CircleShape circle_player;
                circle_player.setOrigin(player->radius, player->radius);
                circle_player.setPosition(player->position);
                circle_player.setRadius(player->radius);
                circle_player.setOutlineColor(Color::Green);
                circle_player.setOutlineThickness(1);
                window.draw(circle_player);*/
            }

            if (player->gotHit) {
                player->gotHit = false;
                auto blood = RectangleShape({WINDOW_WIDTH, WINDOW_HEIGHT});
                blood.setFillColor(Color(255, 0, 0, 40));
                window.draw(blood);
            }

            if (state == running) {
                auto label_ammo = gui.get<tgui::Label>("ammo");
                stringstream pwr_out;
                string pwr = to_string(((int) floor(player->ammo / progress.ammo_max * 100)));
                pwr_out << std::setw(3) << std::setfill('0') << pwr;
                label_ammo->setText(pwr_out.str() + " % PWR");
                if (player->isRecharging) {
                    label_ammo->getRenderer()->setTextColor(Color::Cyan);
                } else {
                    label_ammo->getRenderer()->setTextColor(Color::White);
                }
                stringstream str_coins;
                str_coins << setw(3) << setfill('0') << to_string(progress.coins);
                gui.get<tgui::Label>("coins")->setText(str_coins.str() + " $");
                if (player->health <= 0.0f) {
                    gui.get<tgui::Label>("health")->setText("DEAD");
                    gui.get<tgui::Label>("pressR")->setVisible(true);
                } else {
                    stringstream str_health;
                    str_health << setw(3) << setfill('0') << to_string((int) player->health);
                    gui.get<tgui::Label>("health")->setText(str_health.str() + " HP");
                    gui.get<tgui::Label>("pressR")->setVisible(false);
                }
                gui.get<tgui::Label>("cleared")->setVisible(lvl->cleared);
                gui.get<tgui::Button>("next")->setVisible(lvl->cleared);
                gui.get<tgui::Label>("shop_hint")->setVisible(lvl->cleared);
            }

            if (state == paused) {
                Texture screen;
                screen.create(WINDOW_WIDTH, WINDOW_HEIGHT);
                screen.update(window);
                window.clear();
                Sprite scr(screen);
                scr.setColor({150, 150, 150, 255});
                window.draw(scr);
            }

            /*for(auto ca: lvl->coin_areas){
                CircleShape tmp;
                tmp.setFillColor(Color::Yellow);
                tmp.setPosition(ca.upperleft);
                tmp.setRadius(5);
                window.draw(tmp);
                tmp.setPosition(ca.downright);
                window.draw(tmp);
            }
            for(const Path& p: lvl->paths){
                CircleShape tmp;
                tmp.setFillColor(Color::Green);
                tmp.setRadius(3);
                for(auto node: p.nodes){
                    tmp.setPosition(node);
                    window.draw(tmp);
                }
            }*/

            //draw gui
            gui.draw();
            window.display();
            elapsed -= updateTime;
        }
    }

    return EXIT_SUCCESS;

}

void prepareGameGUI(tgui::Gui &gui) {
    auto ammoLabel = tgui::Label::create();
    ammoLabel->setPosition(1150, 600);
    ammoLabel->getSharedRenderer()->setTextOutlineColor(Color::Cyan);
    ammoLabel->getSharedRenderer()->setTextColor(Color::White);
    ammoLabel->getSharedRenderer()->setTextOutlineThickness(.2f);
    ammoLabel->setTextSize(20);
    gui.add(ammoLabel, "ammo");

    auto coinLabel = tgui::Label::create();
    coinLabel->setPosition(1150, 580);
    coinLabel->getRenderer()->setTextColor(Color::Yellow);
    coinLabel->setTextSize(20);
    gui.add(coinLabel, "coins");

    auto healthLabel = tgui::Label::create();
    healthLabel->setPosition(1150, 560);
    healthLabel->getRenderer()->setTextColor(Color::Red);
    healthLabel->setTextSize(20);
    gui.add(healthLabel, "health");

    auto label_pressR = tgui::Label::create("Press R to respawn");
    label_pressR->setPosition("(parent.width - width) / 2", "80%");
    label_pressR->setVisible(false);
    gui.add(label_pressR, "pressR");
}

void prepareClearedGUI(tgui::Gui &gui) {
    auto cleared = tgui::Label::create("Level finished");
    cleared->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    cleared->setTextSize(30);
    cleared->getRenderer()->setTextColor(Color(211, 211, 211));
    cleared->getRenderer()->setTextOutlineThickness(1);
    cleared->getRenderer()->setTextOutlineColor(Color::Blue);
    cleared->setPosition("(parent.width - width) / 2", "20%");
    gui.add(cleared, "cleared");

    auto label_shop_note = tgui::Label::create("Press ESC to shop for upgrades!");
    label_shop_note->setTextSize(15);
    label_shop_note->setPosition("(parent.width - width) / 2", "80%");
    gui.add(label_shop_note, "shop_hint");

    auto next = tgui::Button::create("Next Level");
    next->setTextSize(20);
    next->setPosition("(parent.width - width) / 2", "40%");
    gui.add(next, "next");
    next->connect("pressed", load_next_level);
}

void upgradeGunWidth(tgui::Gui &gui) {
    if (progress.coins >= 5) {
        progress.shootangle += 0.2f;
        float twopi = (float) M_PI * 2.f;
        progress.shootangle = min(twopi, progress.shootangle);
        progress.coins -= 5;
        gui.get<tgui::Label>("p_coins")->setText("Coins: " + to_string(progress.coins));
    }
}

void upgradeGunLength(tgui::Gui &gui) {
    if (progress.coins >= 5) {
        progress.raylength += 150.0f;
        progress.coins -= 5;
        gui.get<tgui::Label>("p_coins")->setText("Coins: " + to_string(progress.coins));
    }
}

void upgradeWalltype(walltype type, tgui::Gui &gui) {
    if (progress.coins >= 5) {
        progress.type = type;
        progress.coins -= 5;
        gui.get<tgui::Label>("p_coins")->setText("Coins: " + to_string(progress.coins));
        if (type == mirror) {
            gui.get<tgui::Button>("upgrade_mirror")->setEnabled(false);
        }
    }
}

void biggerBattery(tgui::Gui &gui) {
    if (progress.coins >= 5) {
        progress.ammo_max += 60;
        progress.coins -= 5;
        gui.get<tgui::Label>("p_coins")->setText("Coins: " + to_string(progress.coins));
    }
}

void preparePauseGUI(tgui::Gui &gui) {
    auto name = tgui::Label::create("Paused");
    name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    name->setTextSize(30);
    name->getRenderer()->setTextColor(Color(211, 211, 211));
    name->getRenderer()->setTextOutlineThickness(1);
    name->getRenderer()->setTextOutlineColor(Color::White);
    name->setPosition("(parent.width - width) / 2", "20%");
    gui.add(name);

    auto label_coins = tgui::Label::create("Coins: " + to_string(progress.coins));
    label_coins->setPosition("(parent.width - width) / 2", "40%");
    label_coins->getRenderer()->setTextColor(Color::Yellow);
    label_coins->setTextSize(20);
    gui.add(label_coins, "p_coins");

    auto button_upgrade_gun_width = tgui::Button::create("Wider Shots - 5$");
    button_upgrade_gun_width->setPosition("3*(parent.width - width) / 4", "60%");
    button_upgrade_gun_width->setTextSize(20);
    gui.add(button_upgrade_gun_width);
    button_upgrade_gun_width->connect("pressed", upgradeGunWidth, ref(gui));

    auto button_upgrade_gun_length = tgui::Button::create("Longer Shots - 5$");
    button_upgrade_gun_length->setPosition("(parent.width - width) / 4", "60%");
    button_upgrade_gun_length->setTextSize(20);
    gui.add(button_upgrade_gun_length);
    button_upgrade_gun_length->connect("pressed", upgradeGunLength, ref(gui));

    auto button_upgrade_to_mirror = tgui::Button::create("Place Mirrors instead of Walls - 5$");
    button_upgrade_to_mirror->setPosition("(parent.width - width) / 4", "80%");
    button_upgrade_to_mirror->setTextSize(20);
    if (progress.type == mirror) {
        button_upgrade_to_mirror->setEnabled(false);
    }
    gui.add(button_upgrade_to_mirror, "upgrade_mirror");
    button_upgrade_to_mirror->connect("pressed", upgradeWalltype, mirror, ref(gui));

    auto button_upgrade_battery = tgui::Button::create("Increase Battery-Size - 5$");
    button_upgrade_battery->setPosition("3*(parent.width - width) / 4", "80%");
    button_upgrade_battery->setTextSize(20);
    gui.add(button_upgrade_battery);
    button_upgrade_battery->connect("pressed", biggerBattery, ref(gui));

}

void startGame(tgui::Gui &gui, bool restart) {
    if (restart) {
        save_initialized = false;
        PlayerProgress tmp;
        tmp.level = 0;
        progress = tmp;
        load_next_level();
    }
    state = running;
    gameStateChanged = true;
}

void prepareMainMenuGUI(tgui::Gui &gui) {
    tgui::Label::Ptr name = tgui::Label::create("Rayzor");
    name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    name->setTextSize(50);
    name->getRenderer()->setTextColor(Color::Magenta);
    name->getRenderer()->setTextOutlineThickness(2);
    name->getRenderer()->setTextOutlineColor(Color::White);
    name->setPosition("(parent.width - width) / 2", "40%");
    gui.add(name);

    tgui::Button::Ptr button_start = tgui::Button::create("Start Game");
    button_start->setTextSize(20);
    button_start->setPosition("(parent.width - width) / 2", "60%");
    gui.add(button_start, "start");
    button_start->connect("pressed", startGame, ref(gui), (bool) false);

    auto label_instructions = tgui::Label::create("WASD to move, LMB to shoot, ESC to pause and upgrade\n"
                                                  "Hold space, move around and release it to place walls\n"
                                                  "R to reload a level\n"
                                                  "\n"
                                                  "Hint: Blue enemies can only be damaged by blue light");
    label_instructions->setTextSize(15);
    label_instructions->setPosition("(parent.width - width) / 2", "80%");
    gui.add(label_instructions);
}

void prepareWonGUI(tgui::Gui &gui) {
    tgui::Label::Ptr name = tgui::Label::create("Won!");
    name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    name->setTextSize(50);
    name->getRenderer()->setTextColor(Color::Magenta);
    name->getRenderer()->setTextOutlineThickness(2);
    name->getRenderer()->setTextOutlineColor(Color::White);
    name->setPosition("(parent.width - width) / 2", "40%");
    gui.add(name);

    tgui::Button::Ptr button_start = tgui::Button::create("Restart Game");
    button_start->setTextSize(20);
    button_start->setPosition("(parent.width - width) / 2", "60%");
    gui.add(button_start, "start");
    button_start->connect("pressed", startGame, ref(gui), true);
}

void draw_coins(RenderWindow &window, const vector<Coin> &coins) {
    for (auto c: coins) {
        CircleShape tmp;
        tmp.setOrigin(c.radius, c.radius);
        tmp.setPosition(c.position);
        tmp.setRadius(c.radius);
        tmp.setFillColor(Color::Yellow);
        tmp.setOutlineThickness(2);
        tmp.setOutlineColor(Color(255, 69, 0)); // dark orange
        window.draw(tmp);
    }
}

void update_player(const std::vector<Wall> &walls, Player &player) {
    player.velocity = {0, 0};
    //player
    if (isKeyPressed(sf::Keyboard::W)) {
        player.velocity.y -= delta_speed;
    }
    if (isKeyPressed(sf::Keyboard::S)) {
        player.velocity.y += delta_speed;
    }
    if (isKeyPressed(sf::Keyboard::A)) {
        player.velocity.x -= delta_speed;
    }
    if (isKeyPressed(sf::Keyboard::D)) {
        player.velocity.x += delta_speed;
    }
    if (player.velocity.x != 0 || player.velocity.y != 0) {
        player.velocity = normalize(player.velocity);
        player.velocity *= delta_speed;
        player.applyMovement(walls);
    }

    if (windowsHasFocus && Mouse::isButtonPressed(sf::Mouse::Left)) {
        player.hasShot = true;
    }
}

void generate_walls(const vector<Wall> &walls, VertexArray &draw_lines) {
    Color wall_color = Color(150, 150, 150, 255);
    Color mirror_color = Color::Blue;
    for (auto wall: walls) {
        Color color;
        switch (wall.type) {
            case plain:
                color = wall_color;
                break;
            case mirror:
                color = mirror_color;
                break;
            case glass:
                color = wall.color.value();
                break;
            case invisible:
                continue;
            default:
                color = Color::Magenta;
                break;
        }
        Vertex start = wall.start;
        start.color = color;
        draw_lines.append(start);
        Vertex end = wall.end;
        end.color = color;
        draw_lines.append(end);
    }
}

void trace_ray(std::vector<Wall> &walls, Ray &ray) {
    for (auto wall: walls) {
        if (wall.type == invisible)
            continue;
        wall.collideRay(ray);
    }
    if (ray.w_type == none) {
        return;
    }
    switch (ray.w_type) {
        case plain:
            break;
        case mirror: {
            if (ray.shot && ray.jumps < MAX_JUMPS) {
                auto diff = ray.segments.back().end - ray.segments.back().start;
                Vector2f direction = normalize(diff);
                float cos = dotP(direction, ray.normal);
                float flip_factor = 1;
                if (cos > 0) {
                    //the ray comes from the other side of the mirror
                    flip_factor = -1.0f;
                }
                auto start = ray.segments.back().end + ray.normal * flip_factor;
                Vector2f new_direction =
                        direction - 2 * dotP(direction, ray.normal * flip_factor) * ray.normal * flip_factor;
                ray.length -= getLength(diff);
                Line new_line(start, start + new_direction * ray.length);
                new_line.color = ray.segments.back().color; // preserve color
                ray.segments.push_back(new_line);
                ray.jumps++;
                ray.w_type = none; //reset ray
                trace_ray(walls, ray);
            }
            break;
        }
        case glass: {
            auto rend = ray.segments.back().end;
            auto rstart = ray.segments.back().start;
            auto diff = rend - rstart;
            ray.length -= getLength(diff) - 0.5f;
            Vector2f direction = normalize(diff);

            auto start = ray.segments.back().end + direction;
            Line new_line(start, start + direction * ray.length);
            assert(ray.color.has_value());
            new_line.color = ray_color * ray.color.value();
            if (abs(direction.x) < FLT_EPSILON) {
                //TODO fix
                break;
            }
            ray.segments.push_back(new_line);
            ray.w_type = none; //reset ray
            trace_ray(walls, ray);
            break;
        }
        default:
            break;
    }
}

void
raycast(Vector2f origin, std::vector<Wall> &walls, VertexArray &draw_lines, float window, float angle, int raycount,
        bool shot, float ray_length) {

    std::vector<Ray> rays;

    float step = window / (float) raycount;
    int steps = static_cast <int> (floor(window / step));
    for (int i = 0; i < steps; i++) {
        angle += step;
        auto direction = Vector2f(sin(angle), cos(angle));
        Ray ray(origin, direction, ray_length);
        ray.shot = shot;
        rays.push_back(ray);
    }
    for (auto ray: rays) {
        trace_ray(walls, ray);
        //convert ray segments to vertices for the draw call
        Color seg_color;
        Vertex root;
        Vertex end;
        for (Line line: ray.segments) {
            seg_color = ray_color;
            if (line.color.has_value()) {
                seg_color = line.color.value();
            }
            root = line.start;
            root.color = seg_color;
            draw_lines.append(root);

            end = line.end;
            end.color = seg_color;
            draw_lines.append(end);
        }
    }
}

bool isKeyPressed(Keyboard::Key key) {
    return windowsHasFocus && Keyboard::isKeyPressed(key);
}