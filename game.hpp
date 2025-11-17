#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <iostream>


class Character {
public:
    virtual ~Character() {}

    // All characters must implement these:
    virtual void update(float dt , sf::FloatRect platformBounds[]) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

protected:
    sf::Texture texture;
    sf::Sprite sprite;
};


class Player : public Character {
public:
    Player();

    void update(float dt , sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;

private:
    float timer;
    bool onGround;
};

class Platform {
public:
    Platform() {};
    Platform(const std::string& filename, float x, float y);

    virtual ~Platform() = default;

    virtual void draw(sf::RenderWindow& window);

    virtual sf::FloatRect getBounds() const;

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

    void setTexture(const sf::Texture& tex);

protected:
    sf::Texture texture;
    sf::Sprite sprite;
};

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock clock;

    Player player;
    Platform platform1;
    Platform platform2;
    Platform platform3;
    Platform platform4;
    Platform platform5;
    Platform platform6;
    Platform platform7;

    void processEvents();
    void update(float dt);
    void render();
};

#endif
