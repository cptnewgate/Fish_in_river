#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
struct Rock
{
    sf::RectangleShape shape;
    int lane;
    bool goingLeft;
};
int main()
{
    std::cout << "SFML app running!" << std::endl;
    bool paused = false;
    bool game_over = false;
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Random Wall Chaser");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));
    sf::Clock game_clock;
    const float base_speed = 1.f;
    float speed = base_speed;
    const float speed_increment = 0.05f;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "failed to load font" << std::endl;
    }
    sf::Text timer_text;
    timer_text.setFont(font);
    timer_text.setCharacterSize(20);
    timer_text.setFillColor(sf::Color::White);
    timer_text.setPosition(10.f, 50.f);
    int score = 0;
    sf::Text score_text;
    score_text.setFont(font);
    score_text.setCharacterSize(20);
    score_text.setFillColor(sf::Color::White);
    score_text.setPosition(10.f, 30.f);
    score_text.setString("Score: 0");
    sf::Text hp_text;
    hp_text.setFont(font);
    hp_text.setCharacterSize(20);
    hp_text.setFillColor(sf::Color::White);
    hp_text.setPosition(10.f, 70.f);
    hp_text.setString("HP: 5");
    int high_score = 0;
    sf::Text high_score_text;
    high_score_text.setFont(font);
    high_score_text.setCharacterSize(20);
    high_score_text.setFillColor(sf::Color::White);
    high_score_text.setPosition(10.f, 90.f);
    high_score_text.setString("High Score: 0");
    const int num_lanes = 4;
    sf::RectangleShape lanes[num_lanes];
    const float line_thickness = 20.f;
    const float lane_spacing = 600.f / 5.f;
    for (int i = 0; i < num_lanes; ++i)
    {
        lanes[i].setSize(sf::Vector2f(800.f, line_thickness));
        lanes[i].setFillColor(sf::Color::Blue);
        lanes[i].setPosition(0.f, (i + 1) * lane_spacing);
    }
    sf::CircleShape player(10.f);
    player.setFillColor(sf::Color::Red);
    int current_lane = 1;
    player.setPosition(400.f, (current_lane + 1) * lane_spacing - player.getRadius());
    const float lane_y_positions[num_lanes] = {
        lane_spacing * 1 - 10.f,
        lane_spacing * 2 - 10.f,
        lane_spacing * 3 - 10.f,
        lane_spacing * 4 - 10.f};
    const float lateral_speed = 5.f;
    int player_hp = 5;
    sf::Color grey(100, 100, 100);
    sf::Color brown(165, 42, 42);
    std::vector<Rock> rocks;
    sf::Clock spawn_clock;
    float spawn_interval = 1.5f;
    std::vector<Rock> food;
    sf::Clock food_spawn_clock;
    float food_spawn_interval = 3.f;
    std::vector<Rock> medicine;
    sf::Clock medicine_spawn_clock;
    float medicine_spawn_interval = 7.f;
    sf::RectangleShape pause_overlay(sf::Vector2f(800.f, 600.f));
    pause_overlay.setFillColor(sf::Color(0, 0, 0, 150));
    sf::Text status_text;
    status_text.setFont(font);
    status_text.setCharacterSize(40);
    status_text.setFillColor(sf::Color::White);
    status_text.setString("PAUSED");
    sf::FloatRect textRect = status_text.getLocalBounds();
    status_text.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
    status_text.setPosition(400.f, 300.f);
    sf::Text exit_text;
    exit_text.setFont(font);
    exit_text.setCharacterSize(30);
    exit_text.setFillColor(sf::Color::White);
    exit_text.setString("Exit Game (Press Escape)");
    sf::FloatRect exitRect = exit_text.getLocalBounds();
    exit_text.setOrigin(exitRect.width / 2.f, exitRect.height / 2.f);
    exit_text.setPosition(400.f, 360.f);
    sf::Text restart_text;
    restart_text.setFont(font);
    restart_text.setCharacterSize(30);
    restart_text.setFillColor(sf::Color::White);
    restart_text.setString("Restart Game (Press Enter)");
    sf::FloatRect restart_Rect = restart_text.getLocalBounds();
    restart_text.setOrigin(restart_Rect.width / 2.f, restart_Rect.height / 2.f);
    restart_text.setPosition(400.f, 380.f);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && !game_over)
            {
                paused = !paused;
            }
            if (paused && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
            if (paused && event.key.code == sf::Keyboard::Enter)
            {
                std::stringstream hs;
                hs << "High Score: " << high_score;
                high_score_text.setString(hs.str());
                current_lane = 1;
                player.setPosition(400.f, lane_y_positions[current_lane]);
                player_hp = 5;
                score = 0;
                rocks.clear();
                food.clear();
                medicine.clear();
                game_clock.restart();
                spawn_clock.restart();
                food_spawn_clock.restart();
                medicine_spawn_clock.restart();
                score_text.setString("Score: 0");
                hp_text.setString("HP: 5");
                timer_text.setString("Time: 0s");
                paused = false;
                game_over = false;
            }
        }
        if (!paused)
        {
            if (player_hp <= 0)
            {
                game_over = true;
                paused = true;
            }
            float time = game_clock.getElapsedTime().asSeconds();
            std::stringstream ss;
            ss << "Time: " << static_cast<int>(time) << "s";
            timer_text.setString(ss.str());
            std::stringstream hp_ss;
            hp_ss << "HP: " << player_hp;
            hp_text.setString(hp_ss.str());
            if (spawn_clock.getElapsedTime().asSeconds() > spawn_interval)
            {
                Rock new_rock;
                new_rock.lane = rand() % num_lanes;
                new_rock.goingLeft = rand() % 2;
                new_rock.shape.setSize(sf::Vector2f(20.f, 20.f));
                new_rock.shape.setFillColor(sf::Color(grey));
                float y = lane_y_positions[new_rock.lane];
                float x = new_rock.goingLeft ? 800.f : -20.f;
                new_rock.shape.setPosition(x, y);
                rocks.push_back(new_rock);
                spawn_clock.restart();
            }
            if (food_spawn_clock.getElapsedTime().asSeconds() > food_spawn_interval)
            {
                Rock new_food;
                new_food.lane = rand() % num_lanes;
                new_food.goingLeft = rand() % 2;
                new_food.shape.setSize(sf::Vector2f(25.f, 25.f));
                new_food.shape.setFillColor(sf::Color(brown));
                float y = lane_y_positions[new_food.lane];
                float x = new_food.goingLeft ? 800.f : -30.f;
                new_food.shape.setPosition(x, y);
                food.push_back(new_food);
                food_spawn_clock.restart();
            }
            if (medicine_spawn_clock.getElapsedTime().asSeconds() > medicine_spawn_interval)
            {
                Rock new_med;
                new_med.lane = rand() % num_lanes;
                new_med.goingLeft = rand() % 2;
                new_med.shape.setSize(sf::Vector2f(25.f, 25.f));
                new_med.shape.setFillColor(sf::Color::Yellow);
                float y = lane_y_positions[new_med.lane];
                float x = new_med.goingLeft ? 800.f : -30.f;
                new_med.shape.setPosition(x, y);
                medicine.push_back(new_med);
                medicine_spawn_clock.restart();
            }
            speed = base_speed + (time * speed_increment);
            for (auto it = rocks.begin(); it != rocks.end();)
            {
                float moveX = speed * (it->goingLeft ? -1.f : 1.f);
                it->shape.move(moveX, 0.f);
                if (it->shape.getGlobalBounds().intersects(player.getGlobalBounds()))
                {
                    if (player_hp > 0)
                        player_hp--;
                    it = rocks.erase(it);
                    continue;
                }
                sf::Vector2f pos = it->shape.getPosition();
                if (pos.x < -40.f || pos.x > 840.f)
                {
                    it = rocks.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            for (auto it = food.begin(); it != food.end();)
            {
                float moveX = speed * (it->goingLeft ? -1.f : 1.f);
                it->shape.move(moveX, 0.f);
                if (it->shape.getGlobalBounds().intersects(player.getGlobalBounds()))
                {
                    score += 1;
                    std::stringstream ss;
                    ss << "Score: " << score;
                    score_text.setString(ss.str());
                    it = food.erase(it);
                    if (score > high_score)
                    {
                        high_score = score;
                        std::stringstream hs;
                        hs << "High Score: " << high_score;
                        high_score_text.setString(hs.str());
                    }
                    continue;
                }
                sf::Vector2f pos = it->shape.getPosition();
                if (pos.x < -40.f || pos.x > 840.f)
                {
                    it = food.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            for (auto it = medicine.begin(); it != medicine.end();)
            {
                float moveX = speed * (it->goingLeft ? -1.f : 1.f);
                it->shape.move(moveX, 0.f);
                if (it->shape.getGlobalBounds().intersects(player.getGlobalBounds()))
                {
                    if (player_hp < 5)
                        player_hp++;
                    it = medicine.erase(it);
                    continue;
                }
                sf::Vector2f pos = it->shape.getPosition();
                if (pos.x < -40.f || pos.x > 840.f)
                {
                    it = medicine.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                player.move(-lateral_speed, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                player.move(lateral_speed, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                current_lane = (current_lane - 1 + num_lanes) % num_lanes;
                player.setPosition(player.getPosition().x, lane_y_positions[current_lane]);
                sf::sleep(sf::milliseconds(100));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                current_lane = (current_lane + 1) % num_lanes;
                player.setPosition(player.getPosition().x, lane_y_positions[current_lane]);
                sf::sleep(sf::milliseconds(100));
            }
            sf::Vector2f pos = player.getPosition();
            float diameter = player.getRadius() * 2;
            if (pos.x + diameter < 0.f)
                pos.x = 800.f;
            else if (pos.x > 800.f)
                pos.x = -diameter;
            player.setPosition(pos.x, pos.y);
        }
        window.clear(sf::Color::Green);
        for (const auto &lane : lanes)
            window.draw(lane);
        window.draw(timer_text);
        window.draw(score_text);
        window.draw(player);
        window.draw(hp_text);
        for (const auto &rock : rocks)
            window.draw(rock.shape);
        for (const auto &f : food)
            window.draw(f.shape);
        for (const auto &m : medicine)
            window.draw(m.shape);
        if (paused)
        {
            status_text.setString(game_over ? "GAME OVER" : "PAUSED");
            sf::FloatRect textRect = status_text.getLocalBounds();
            status_text.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
            status_text.setPosition(400.f, 300.f);
            window.draw(pause_overlay);
            window.draw(status_text);
            window.draw(exit_text);
            window.draw(high_score_text);
            window.draw(restart_text);
        }
        window.display();
    }
}