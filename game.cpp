#include "game.hpp"


UIElement::UIElement() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Note: Using default font (arial.ttf not found)" << std::endl;
    }
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
}

HealthBar::HealthBar(int* health, int maxHP) : playerHealth(health), maxHealth(maxHP) {
    text.setPosition(20.f, 20.f); 
}

void HealthBar::draw(sf::RenderWindow& window) {
    window.draw(text);
}

void HealthBar::update() {
    std::string healthText = "Health: " + std::to_string(*playerHealth) + "/" + std::to_string(maxHealth);
    text.setString(healthText);
}

void HealthBar::takeDamage(int damage) {
    if (*playerHealth > 0) {
        *playerHealth -= damage;
        if (*playerHealth < 0) *playerHealth = 0;
    }
}

SoulBar::SoulBar(int* soul) : playerSoul(soul) {
    text.setPosition(20.f, 50.f);
}

void SoulBar::draw(sf::RenderWindow& window) {
    window.draw(text);
}

void SoulBar::update() {
    std::string soulText = "Soul: " + std::to_string(*playerSoul);
    text.setString(soulText);
}

Background::Background(const std::string& filename)
{
    if (!texture.loadFromFile(filename)) {
        std::cout << "Failed to load background: " << filename << std::endl;
    }
    sprite.setTexture(texture);
}

void Background::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

Character::Character() : vx(0), vy(0), gravity(800.f), health(100), 
facingRight(true), onGround(false) {}

sf::FloatRect Character::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Vector2f Character::getPosition() const {
    return sprite.getPosition();
}

bool Character::isFacingRight() const {
    return facingRight;
}

Player::Player() : maxHealth(100), moveSpeed(300.f), jumpForce(-550.f),
    soul(0), maxSoul(100), isAttacking(false), attackDuration(0.f), 
    attackCooldown(0.f), attacked(true)
{
    sf::Image fullImage;
    if (fullImage.loadFromFile("player.png")) {
        sf::IntRect frameRect(4025, 3965, 71, 130);
        sf::Image frameImage;
        frameImage.create(frameRect.width, frameRect.height, sf::Color::Transparent);
        frameImage.copy(fullImage, 0, 0, frameRect, true);
        texture.loadFromImage(frameImage);

        sf::IntRect attackFrameRect(1770, 2777, 108, 43);
        sf::Image attackFrameImage;
        attackFrameImage.create(attackFrameRect.width, attackFrameRect.height, sf::Color::Transparent);
        attackFrameImage.copy(fullImage, 0, 0, attackFrameRect, true);
        attackTexture.loadFromImage(attackFrameImage);
    }

    sprite.setTexture(texture);
    sprite.setScale(1.0f, 1.0f); 
    sprite.setPosition(100.f, 200.f);
    facingRight = true;

    attackSprite.setTexture(attackTexture);
}

void Player::respawn() {
    health = maxHealth;
    soul = 0;
    vx = 0;
    vy = 0;

    sprite.setPosition(100.f , 200.f);  
    facingRight = true;
    onGround = false;

    std::cout << "Player respawned!" << std::endl;
}

void Player::meleeAttack() {
    if (attackCooldown <= 0.f) {
        isAttacking = true;
        attackDuration = 0.075f;   
        attackCooldown = 0.75f;       
        attacked = false;
    }
}

sf::FloatRect Player::getAttackHitbox() const {
    sf::FloatRect b = sprite.getGlobalBounds();

    float width = 45.f;  
    float height = 100.f; 

    if (facingRight)
        return sf::FloatRect(b.left + b.width, b.top + 20.f, width, height);
    else
        return sf::FloatRect(b.left - width, b.top + 20.f, width, height);
}


void Player::update(float dt, sf::FloatRect platformBounds[])
{
   // std::cout << sprite.getPosition().x << "   " << sprite.getPosition().y << std::endl;

    vx = 0.f;
    float scale = 1.0f; 

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        heal();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        vx = -moveSpeed;
        facingRight = false;
        sprite.setScale(scale, scale);
        sprite.setOrigin(0, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        vx = moveSpeed;
        facingRight = true;
        sprite.setScale(-scale, scale);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {
        vy = jumpForce;
        onGround = false;
    }

    vy += gravity * dt;
    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f newPos = currentPos + sf::Vector2f(vx * dt, vy * dt);
    
    sprite.setPosition(newPos.x, currentPos.y);
    
    sf::FloatRect playerBounds = getBounds();
    bool horizontalCollision = false;
    for (int i = 0; i < 9; i++) {
        if (playerBounds.intersects(platformBounds[i])) {
            horizontalCollision = true;
            break;
        }
    }
    
    if (horizontalCollision) {
        sprite.setPosition(currentPos.x, currentPos.y);
        vx = 0;
    }
    
    sprite.setPosition(sprite.getPosition().x, newPos.y);
    
    playerBounds = getBounds();
    onGround = false;
    
    for (int i = 0; i < 9; i++) {
        if (playerBounds.intersects(platformBounds[i])) {
            sf::FloatRect platform = platformBounds[i];
            
            if (vy > 0 && playerBounds.top + playerBounds.height > platform.top) {
                sprite.setPosition(sprite.getPosition().x, platform.top - playerBounds.height);
                vy = 0;
                onGround = true;
            }
            else if (vy < 0 && playerBounds.top < platform.top + platform.height) {
                sprite.setPosition(sprite.getPosition().x, platform.top + platform.height);
                vy = 0;
            }
            break;
        }
    }

    if (sprite.getPosition().y >= 700.f) respawn();

    if (isAttacking) {
        attackDuration -= dt;
        if (attackDuration <= 0.f) {
            isAttacking = false;
        }
    }

    if (attackCooldown > 0.f) {
        attackCooldown -= dt;
        if (attackCooldown < 0.f) attackCooldown = 0.f;
    }

}


void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);

    if (isAttacking) {
        sf::FloatRect hb = getAttackHitbox();
        if (facingRight) {
            attackSprite.setPosition(hb.left, hb.top + 50);
            attackSprite.setScale(1.0f, 1.0f); 
            window.draw(attackSprite);
        } else {
            attackSprite.setPosition(hb.left + 40, hb.top + 50);
            attackSprite.setScale(-1.0f, 1.0f); 
            window.draw(attackSprite);
        }
    }
}

void Player::gainSoul(int amount) {
    soul += amount;
    if (soul > maxSoul) soul = maxSoul;
}

void Player::heal() {
    if (soul >= 10 && health != 100) { 
        health += 20;
        if (health > 100) health = 100;
        soul -= 10;
    }
}

void Player::setColor(const sf::Color& color) {
    sprite.setColor(color);
}

Enemy::Enemy(float startX, float startY, float leftBound, float rightBound) {
    texture.loadFromFile("enemy2.png");
    sprite.setTexture(texture);

    sprite.setPosition(startX, startY);
    sprite.setScale(0.75f, 0.75f);

    attackRange = 30.f;
    chaseSpeed = 200.f;
    attackCooldown = 1.f; 
    attackTimer = 0.f;   

    vx = 100.f;

    facingRight = true;

    patrolLeft = leftBound;
    patrolRight = rightBound;
    patrolSpeed = 150.f;

    health = 30;

    isDead = false;
    despawnTimer = 1.f;
    colorTimer = 0.f;
}

// float Enemy::distanceToPlayer(const Player& player) {
//     return std::abs(player.getPosition().x - sprite.getPosition().x);
// }

float Enemy::distanceToPlayer(Player& player) {
    sf::Vector2f p = player.getPosition();
    sf::Vector2f e = sprite.getPosition();

    float dx = p.x - e.x;
    float dy = p.y - e.y;

    return std::sqrt(dx * dx + dy * dy);
}


void Enemy::setColor(const sf::Color& color) {
    sprite.setColor(color);
}

void Enemy::setPlayer(Player* player) {
    targetPlayer = player;
}

void Enemy::update(float dt, sf::FloatRect platformBounds[]) {
    if (!targetPlayer) return;
    if (isDead) {
        if (despawnTimer > 0) despawnTimer -= dt;
        if (despawnTimer <= 0) {
            sprite.setPosition(-9999, -9999); 
        }
        return;
    }

    attackTimer -= dt;
    isAttacking = false;

    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f playerPos = targetPlayer->getPosition();
    // float dist = std::abs(playerPos.x - pos.x);
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y + 80.f - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    std::cout << dist << std::endl;
    float vx = 0.f;

    if (dist <= attackRange) {
        vx = 0.f;
        if (attackTimer <= 0.f) {
            isAttacking = true;
            attackTimer = attackCooldown;
            targetPlayer->health -= 10;
            if (targetPlayer->health < 0) targetPlayer->health = 0;
            std::cout << "Player hit! Current health: " << targetPlayer->health << std::endl;
        }
    } else if (playerPos.x >= patrolLeft && playerPos.x <= patrolRight) {
        vx = (playerPos.x > pos.x) ? chaseSpeed : -chaseSpeed;
    } else {
        vx = (facingRight) ? patrolSpeed : -patrolSpeed;
    }

    sprite.move(vx * dt, 0.f);

    if (vx > 0.f) facingRight = true;
    else if (vx < 0.f) facingRight = false;

    sprite.setScale(facingRight ? 0.75f : -0.75f, 0.75f);
    sprite.setOrigin(facingRight ? 0.f : texture.getSize().x, 0.f);

    if (sprite.getPosition().x >= patrolRight) {
        sprite.setPosition(patrolRight, pos.y);
        facingRight = false;
    } else if (sprite.getPosition().x <= patrolLeft) {
        sprite.setPosition(patrolLeft, pos.y);
        facingRight = true;
    }

    if (colorTimer > 0.f) {
        colorTimer -= dt;
        if (colorTimer <= 0) {
            colorTimer = 0;
            setColor(sf::Color::White);
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

Platform::Platform(const std::string& filename, float x, float y)
{
    sf::Image fullImage;
    if (!fullImage.loadFromFile("platform.png"))
        std::cout << "Failed to load platform sprite!\n";

    sf::IntRect frameRect(0, 330, 310, 160);
    sf::Image frameImage;
    frameImage.create(frameRect.width, frameRect.height);
    frameImage.copy(fullImage, 0, 0, frameRect, false);

    if (!texture.loadFromImage(frameImage))
        std::cout << "Failed to create platform texture!\n";

    sprite.setTexture(texture);
    sprite.setPosition(x, y);
}

void Platform::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const
{
    return sprite.getGlobalBounds();
}

Game::Game()
    : background("bgimg.png"),
      platform1("platform.png", -20.f, 750.f),
      platform2("platform.png", 255.f, 750.f),
      platform3("platform.png", 530.f, 750.f),
      platform4("platform.png", 705.f, 750.f),
      platform5("platform.png", 980.f, 750.f),
      platform6("platform.png", 1255.f, 750.f),
      platform7("platform.png", 1850.f, 750.f),
      platform8("platform.png", 2350.f, 600.f),
      platform9("platform.png", 2350.f, 275.f),
      healthBar(&player.health, player.maxHealth),
      soulBar(&player.soul),
      enemy(600.f, 725.f, 500.f, 800.f)
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int width  = desktop.width  * 0.8f;
    unsigned int height = desktop.height * 0.8f;

    window.create(sf::VideoMode(width, height), "Hollow Knight Inspired Game");
    window.setFramerateLimit(60);

    camera.setSize(window.getSize().x, window.getSize().y);
    camera.setCenter(player.getPosition());


    enemy.setPlayer(&player);
}


void Game::run()
{
    sf::Clock clock;
    const float targetFrameTime = 1.0f / 60.0f;
    float accumulatedTime = 0.0f;
    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        accumulatedTime += dt;
        processEvents();
        while (accumulatedTime >= targetFrameTime) {
            update(targetFrameTime);
            accumulatedTime -= targetFrameTime;
        }
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
            healthBar.takeDamage(10);
            std::cout << "Health reduced! Current health: " << player.health << std::endl;
        }
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
            player.gainSoul(1);
            std::cout << "Soul increased! Current soul: " << player.soul << std::endl;
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::J) {
            player.meleeAttack();
        }

    }
}


void Game::update(float dt)
{
    sf::FloatRect platformBounds[] = {
        platform1.getBounds(),
        platform2.getBounds(),
        platform3.getBounds(),
        platform4.getBounds(),
        platform5.getBounds(),
        platform6.getBounds(),
        platform7.getBounds(),
        platform8.getBounds(),
        platform9.getBounds()
    };
    
    player.update(dt, platformBounds);
    enemy.update(dt, platformBounds);
    
    if (player.isAttacking && !player.attacked) {
        sf::FloatRect attackBox = player.getAttackHitbox();
        if (attackBox.intersects(enemy.getBounds())) {
            // Deal damage
            player.attacked = true;
            enemy.health -= 10;  
            player.gainSoul(5);
            if (enemy.health <= 0) {
                enemy.health = -1;
                enemy.isDead = true;
                // enemy.despawnTimer = 1.0f;
                enemy.setColor(sf::Color(80, 80, 80));
            } else {
                std::cout << "Enemy hit! Current health: " << enemy.health << std::endl;
                enemy.setColor(sf::Color::Red);
                enemy.colorTimer = 0.5f;
            }

        }
    }

    sf::Vector2f camPos = camera.getCenter();
    camPos.x = player.getPosition().x;

    float smooth = 5.f;
    camPos.x = camera.getCenter().x + (player.getPosition().x - camera.getCenter().x) * dt * smooth;

    camPos.y = window.getSize().y / 2.f;
    if (camPos.x < window.getSize().x / 2.f)
        camPos.x = window.getSize().x / 2.f;

    float worldLeft  = 0.f;
    float worldRight = 3800.f;
    float halfWidth = camera.getSize().x / 2.f;

    if (camPos.x < worldLeft + halfWidth)
        camPos.x = worldLeft + halfWidth;

    if (camPos.x > worldRight - halfWidth)
        camPos.x = worldRight - halfWidth;

    camera.setCenter(camPos);

    healthBar.update();
    if (player.health <= 0) {
        player.respawn();
    }

    soulBar.update();
}

void Game::render()
{
    window.clear();
    window.setView(camera);

    background.draw(window);

    platform1.draw(window);
    platform2.draw(window);
    platform3.draw(window);
    platform4.draw(window);
    platform5.draw(window);
    platform6.draw(window);
    platform7.draw(window);
    platform8.draw(window);
    platform9.draw(window);

    player.draw(window);
    if (!enemy.isDead || enemy.despawnTimer > 0) enemy.draw(window);

    window.setView(window.getDefaultView());

    healthBar.draw(window);
    soulBar.draw(window);

    window.display();
}