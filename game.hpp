#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>  // For std::sqrt

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

// ========================
// MELEE ATTACK CLASS
// ========================
class MeleeAttack {
public:
    MeleeAttack();
    
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void activate(const sf::Vector2f& position, bool facingRight);
    bool isActive() const { return active; }
    sf::FloatRect getHitbox() const;
    int getDamage() const { return damage; }
    void setDamage(int dmg) { damage = dmg; }

    // Make sprite public so we can update its position
    sf::Sprite sprite;

private:
    sf::Texture texture;
    bool active;
    float activeTime;
    float maxActiveTime;
    int damage;
};

class Character {
public:
    Character();
    virtual ~Character() {}

    // Pure virtual functions
    virtual void update(float dt, sf::FloatRect platformBounds[]) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    
    // Common functions
    virtual void takeDamage(int damage);
    virtual bool isDead() const;
    
    // Getters
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void resetColor();
    bool isFacingRight() const;
    
    // Position setters
    void setPosition(float x, float y) { sprite.setPosition(x, y); }
    
    // Damage flash system
    void triggerDamageFlash();
    void updateDamageFlash(float dt);

    // Make health public for UI access
    int health;
    int maxHealth;

protected:
    // Common attributes
    sf::Texture texture;
    sf::Sprite sprite;
    
    // Physics and movement
    float vx, vy;
    float gravity;
    
    // Character properties
    bool facingRight;
    
    // Collision and state
    bool onGround;
    
    // Damage flash system
    float damageFlashTimer;
    sf::Color originalColor;
};
class Player : public Character {
public:
    Player();
    void update(float dt, sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;

    // Player-specific functions
    void jump();
    void attack();  // This will set the flag
    void gainSoul(int amount);
    void heal();

    // Make soul public for UI access
    int soul;
    int maxSoul;

    // Get current attack for collision detection
    MeleeAttack* getCurrentAttack() { return &currentAttack; }

    // Add a method to check if attack is allowed
    bool canAttack() const { return !isAttackOnCooldown; }

private:
    // Player-specific attributes
    float moveSpeed;
    float jumpForce;
    
    // Attack system - simplified with flag
    MeleeAttack currentAttack;
    bool isAttackOnCooldown;
    float attackCooldownTimer;
};

// ========================
// ENEMY CLASS
// ========================
class Enemy:public Character {
public:
    Enemy(float startX, float startY, float leftBound, float rightBound, float patrolSpeed, float detectionRadius);
    void update(float dt, sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;
    
    // Enemy-specific functions
    bool isPlayerInRange(const sf::Vector2f& playerPosition) const;
    void performMeleeAttack();
    bool isAttacking() const { return isAttacking_; }
    int getAttackDamage() const { return attackDamage_; }
    
    // Getters
    bool isPatrolling() const { return isPatrolling_; }
    bool isPlayerDetected() const { return playerDetected_; }
    float getDetectionRadius() const { return detectionRadius_; }
    
    // Get current attack
    MeleeAttack* getCurrentAttack() { return &currentAttack; }

    // Set player position for detection
    void setPlayerPosition(const sf::Vector2f& position) { playerPosition_ = position; }

private:
    void patrolMovement(float dt);
    void updateAttackCooldown(float dt);
    
    // Patrol boundaries and movement
    float leftBoundary_;
    float rightBoundary_;
    float patrolSpeed_;
    
    // Detection system
    float detectionRadius_;
    bool playerDetected_;
    
    // Attack system
    bool isAttacking_;
    float attackCooldown_;
    int attackDamage_;
    // Attack system - simplified with flag
    bool isAttackOnCooldown_;
    float attackCooldownTimer_;

    // State flags
    bool isPatrolling_;
    bool movingRight_;
    
    // Melee attack
    MeleeAttack currentAttack;
    
    // Store player position for detection
    sf::Vector2f playerPosition_;
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
    Enemy enemy; 
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
    void checkAttackCollisions();
};

#endif