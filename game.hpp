#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

class Character {
public:
    Character();
    virtual ~Character() {}

    // Pure virtual functions - must be implemented by derived classes
    virtual void update(float dt, sf::FloatRect platformBounds[]) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    
    // Common functions that can be used by all characters
    virtual void takeDamage(int damage);
    virtual bool isDead() const;
    
    // Getters for collision and positioning
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    bool isFacingRight() const;

protected:
    // Common attributes for all characters
    sf::Texture texture;
    sf::Sprite sprite;
    
    // Physics and movement
    float vx, vy;
    float gravity;
    
    // Character properties
    int health;
    int maxHealth;
    bool facingRight;
    
    // Collision and state
    bool onGround;
};

class Player : public Character {
public:
    Player();
    void update(float dt, sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;

    // Player-specific functions
    void jump();
    void meleeAttack();
    void gainSoul(int amount);
    void heal();

private:
    // Player-specific attributes
    float moveSpeed;
    float jumpForce;
    int soul;
    int maxSoul;
    bool isAttacking;
    float attackCooldown;
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
