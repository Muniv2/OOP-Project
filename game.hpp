#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

// ========================
// UI ELEMENT BASE CLASS
// ========================
class UIElement {
protected:
    sf::Text text;
    sf::Font font;

public:
    UIElement();
    virtual ~UIElement() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update() = 0;
};

// ========================
// HEALTH BAR CLASS
// ========================
class HealthBar : public UIElement {
private:
    int* playerHealth; // Pointer to player's health
    int maxHealth;

public:
    HealthBar(int* health, int maxHP);
    void draw(sf::RenderWindow& window) override;
    void update() override;
    void takeDamage(int damage);
};

// ========================
// SOUL BAR CLASS
// ========================
class SoulBar : public UIElement {
private:
    int* playerSoul; // Pointer to player's soul

public:
    SoulBar(int* soul);
    void draw(sf::RenderWindow& window) override;
    void update() override;
};

class Background {
public:
    Background(const std::string& filename);
    void draw(sf::RenderWindow& window);

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

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

    // Make health public for UI access
    int health;
    int maxHealth;

protected:
    // Common attributes for all characters
    sf::Texture texture;
    sf::Sprite sprite;
    
    // Physics and movement
    float vx, vy;
    float gravity;
    
    // Character properties
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

    // Make soul public for UI access
    int soul;
    int maxSoul;

private:
    // Player-specific attributes
    float moveSpeed;
    float jumpForce;
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

    Background background;  // Simple background
    Player player;
    Platform platform1;
    Platform platform2;
    Platform platform3;
    Platform platform4;
    Platform platform5;
    Platform platform6;
    Platform platform7;

    // Add UI elements
    HealthBar healthBar;
    SoulBar soulBar;

    void processEvents();
    void update(float dt);
    void render();
};

#endif