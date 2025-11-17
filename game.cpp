#include "game.hpp"

// =========================
//   CHARACTER IMPLEMENTATION xxxx
// =========================
Character::Character() 
    : vx(0), vy(0), gravity(800.f), health(100), maxHealth(100), 
      facingRight(true), onGround(false)
{
    // Base initialization - derived classes will set up their specific sprites
}

void Character::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

bool Character::isDead() const {
    return health <= 0;
}

sf::FloatRect Character::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Vector2f Character::getPosition() const {
    return sprite.getPosition();
}

bool Character::isFacingRight() const {
    return facingRight;
}

Player::Player()
    : moveSpeed(200.f), jumpForce(-400.f), soul(0), maxSoul(100),
      isAttacking(false), attackCooldown(0.f)
{
    // Load and extract player sprite from sprite sheet
    sf::Image fullImage;
    if (fullImage.loadFromFile("player.png")) {
        // Extract character frame from sprite sheet
        sf::IntRect frameRect(4025, 3965, 71, 130);
        sf::Image frameImage;
        frameImage.create(frameRect.width, frameRect.height, sf::Color::Transparent);
        frameImage.copy(fullImage, 0, 0, frameRect, true);
        texture.loadFromImage(frameImage);
    }

    sprite.setTexture(texture);
    sprite.setScale(1.0f, 1.0f); // Large size
    sprite.setPosition(100.f, 200.f);
    facingRight = true;
}

void Player::update(float dt, sf::FloatRect platformBounds[])
{
    vx = 0.f;
    float scale = 1.0f; // Match constructor scale

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        vx = -moveSpeed;
        facingRight = false;
        sprite.setScale(scale, scale);
        sprite.setOrigin(0, 0);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        vx = moveSpeed;
        facingRight = true;
        sprite.setScale(-scale, scale);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }
    
    // ... rest of movement code remains the same
    // Handle jumping
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {
        vy = jumpForce;
        onGround = false;
    }
    
    // Apply gravity
    vy += gravity * dt;
    
    // Store current position
    sf::Vector2f currentPos = sprite.getPosition();
    
    // Calculate new position
    sf::Vector2f newPos = currentPos + sf::Vector2f(vx * dt, vy * dt);
    
    // Apply horizontal movement first
    sprite.setPosition(newPos.x, currentPos.y);
    
    // Check horizontal collisions
    sf::FloatRect playerBounds = getBounds();
    bool horizontalCollision = false;
    for (int i = 0; i < 7; i++) {
        if (playerBounds.intersects(platformBounds[i])) {
            horizontalCollision = true;
            break;
        }
    }
    
    // If horizontal collision, revert X position
    if (horizontalCollision) {
        sprite.setPosition(currentPos.x, currentPos.y);
        vx = 0;
    }
    
    // Apply vertical movement
    sprite.setPosition(sprite.getPosition().x, newPos.y);
    
    // Check vertical collisions
    playerBounds = getBounds();
    onGround = false;
    
    for (int i = 0; i < 7; i++) {
        if (playerBounds.intersects(platformBounds[i])) {
            sf::FloatRect platform = platformBounds[i];
            
            // Landing on platform from above
            if (vy > 0 && playerBounds.top + playerBounds.height > platform.top) {
                // Position player exactly on top of platform
                sprite.setPosition(sprite.getPosition().x, platform.top - playerBounds.height);
                vy = 0;
                onGround = true;
            }
            // Hitting platform from below
            else if (vy < 0 && playerBounds.top < platform.top + platform.height) {
                sprite.setPosition(sprite.getPosition().x, platform.top + platform.height);
                vy = 0;
            }
            break;
        }
    }
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

// Player-specific functions (implement these later)
void Player::jump() {
    if (onGround) {
        vy = jumpForce;
        onGround = false;
    }
}

void Player::meleeAttack() {
    // Implement attack logic later
    isAttacking = true;
    attackCooldown = 0.3f; // 300ms cooldown
}

void Player::gainSoul(int amount) {
    soul += amount;
    if (soul > maxSoul) soul = maxSoul;
}

void Player::heal() {
    if (soul >= 10) { // Cost 10 soul to heal
        health += 20;
        if (health > maxHealth) health = maxHealth;
        soul -= 10;
    }
}


// =========================
//   PLATFORM IMPLEMENTATION
// =========================
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

// =========================
//   BACKGROUND IMPLEMENTATION
// =========================
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

// =========================
//   GAME IMPLEMENTATION
// =========================
Game::Game()
    : background("bgimg.png"),  // Your background image
      platform1("platform.png", -20.f, 600.f),
      platform2("platform.png", 255.f, 600.f),
      platform3("platform.png", 530.f, 600.f),
      platform4("platform.png", 705.f, 600.f),
      platform5("platform.png", 980.f, 525.f),
      platform6("platform.png", 1255.f, 450.f),
      platform7("platform.png", 1530.f, 375.f)
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int width  = desktop.width  * 0.8f;
    unsigned int height = desktop.height * 0.8f;

    window.create(sf::VideoMode(width, height), "Hollow Knight Inspired Game");
    window.setFramerateLimit(60);
}


void Game::run()
{
    sf::Clock clock;
    const float targetFrameTime = 1.0f / 60.0f; // 60 FPS
    float accumulatedTime = 0.0f;
    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        accumulatedTime += dt;
        
        // Process events every frame
        processEvents();
        
        // Update with fixed timestep for consistent physics
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
        platform7.getBounds()
    };
    player.update(dt, platformBounds);
}

void Game::render()
{
    window.clear(); // Remove the cyan color

    // Draw background first
    background.draw(window);

    // Draw platforms
    platform1.draw(window);
    platform2.draw(window);
    platform3.draw(window);
    platform4.draw(window);
    platform5.draw(window);
    platform6.draw(window);
    platform7.draw(window);

    // Draw player
    player.draw(window);

    window.display();
}
