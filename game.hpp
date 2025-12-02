#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath> 
#include <string>

using namespace std;

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


class HealthBar : public UIElement {
private:
    int* playerHealth;
    int maxHealth;
    sf::Texture fullHealthTexture;
    sf::Texture lowHealthTexture;
    sf::Sprite healthSprites[10];


public:
    HealthBar(int* health, int maxHP);
    void draw(sf::RenderWindow& window) override;
    void update() override;
    void takeDamage(int damage);
};


class SoulBar : public UIElement {
private:
    int* playerSoul;
    sf::Texture texture;
    sf::Sprite sprites[4];

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
    friend class Game;
};


class Character {
public:
    Character();
    virtual ~Character() {}

    virtual void update(float dt, sf::FloatRect platformBounds[]) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    bool isFacingRight() const;
    
    virtual void setColor(const sf::Color& color) = 0;

    int health;
    int damage;
protected:
    sf::Texture texture;
    sf::Sprite sprite;
    
    float vx, vy;
    float gravity;
    
    bool facingRight;
    bool onGround;
    
};

class Player : public Character {
public:
    Player();

    void update(float dt, sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getAttackHitbox() const;
    void meleeAttack();

    void jump();
    void gainSoul(int amount);
    void heal();
    void setColor(const sf::Color& color);
    void respawn();
    
private:
    int damage;
    float moveSpeed;
    float jumpForce;
    int maxHealth;
    int soul;
    int maxSoul;
    bool isAttacking;
    bool attacked;
    float attackDuration;
    float attackCooldown;
    
    sf::Texture attackTexture;
    sf::Sprite attackSprite;
    friend class Game;
};

class Enemy : public Character {
public:
    Enemy(float startX, float startY, float leftBound, float rightBound, string filename, float scale, int healthAmount, int damageAmount,
    float atr);

    void update(float dt, sf::FloatRect platformBounds[]) override;
    void draw(sf::RenderWindow& window) override;
    float distanceToPlayer(Player& player);
    void setPlayer(Player* player);
    void setColor(const sf::Color& color);



private:
    float attackRange;
    float chaseSpeed;
    float attackCooldown; 
    float attackTimer;    
    bool isAttacking = false;     
    float colorTimer;
    bool isDead;
    float despawnTimer;
    Player* targetPlayer;
    float patrolLeft;
    float patrolRight;
    float scale;
    float patrolSpeed;
    friend class Game;
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
    sf::View camera;

    int state;
    int option;
    sf::Texture titleTexture;
    sf::Sprite titleSprite;

    Background background;
    Background mainmenu;
    Player player;
    Platform platform1;
    Platform platform2;
    Platform platform3;
    Platform platform4;
    Platform platform5;
    Platform platform6;
    Platform platform7;
    Platform platform8;
    Platform platform9;
    Platform Platform10;
    Platform Platform11;
    Platform Platform12;
    Platform Platform13;
    Platform Platform14;
    Platform Platform15;
    Platform Platform16;
    Platform Platform17;
    Platform Platform18;
    Platform Platform19;
    Platform Platform20;
    Enemy enemy1;
    Enemy enemy2;
    Enemy enemy3;
    Enemy enemy4;
    Enemy enemy5;
    Enemy enemy6;

    HealthBar healthBar;
    SoulBar soulBar;

    void processEvents();
    void update(float dt);
    void render();
};

#endif