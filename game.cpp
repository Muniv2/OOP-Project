#include "game.hpp"

// =========================
//   UI ELEMENT IMPLEMENTATION
// =========================
UIElement::UIElement() {
    // Load font (you might want to load a proper font file)
    if (!font.loadFromFile("arial.ttf")) {
        // If font loading fails, we'll use the default font
        std::cout << "Note: Using default font (arial.ttf not found)" << std::endl;
    }
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
}

// =========================
//   HEALTH BAR IMPLEMENTATION
// =========================
HealthBar::HealthBar(int* health, int maxHP) 
    : playerHealth(health), maxHealth(maxHP) {
    text.setPosition(20.f, 20.f); // Top-left corner
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

// =========================
//   SOUL BAR IMPLEMENTATION
// =========================
SoulBar::SoulBar(int* soul) : playerSoul(soul) {
    text.setPosition(20.f, 50.f); // Below health bar
}

void SoulBar::draw(sf::RenderWindow& window) {
    window.draw(text);
}

void SoulBar::update() {
    std::string soulText = "Soul: " + std::to_string(*playerSoul);
    text.setString(soulText);
}


// =========================
//   CHARACTER IMPLEMENTATION xxxx
// =========================
Character::Character() 
    : vx(0), vy(0), gravity(800.f), health(100), maxHealth(100), 
      facingRight(true), onGround(false), damageFlashTimer(0.f)
{
    // Base initialization - derived classes will set up their specific sprites
}

void Character::resetColor() {
    sprite.setColor(originalColor);
}

void Character::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

bool Character::isDead() const {
    return health <= 0;
}

// Character damage flash implementation
void Character::triggerDamageFlash()
{
    damageFlashTimer = 0.5f;
    originalColor = sprite.getColor();
    sprite.setColor(sf::Color::Red);
}

void Character::updateDamageFlash(float dt)
{
    if (damageFlashTimer > 0.f) {
        damageFlashTimer -= dt;
        if (damageFlashTimer <= 0.f) {
            resetColor(); // Use the new method instead of direct access
        }
    }
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
      isAttackOnCooldown(false), attackCooldownTimer(0.f)
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
    // Set attack damage for the player's melee attack
    currentAttack.setDamage(30);
    attackCooldownTimer = 0.f;
}

void Player::attack()
{
    // Only attack if not on cooldown
    if (!isAttackOnCooldown) {
        currentAttack.activate(getPosition(), facingRight);
        isAttackOnCooldown = true;  // Set the flag
        attackCooldownTimer = 2.0f; // 2 second cooldown
        std::cout << "Attack started! Cooldown: 2.0s" << std::endl;
    }
}



void Player::update(float dt, sf::FloatRect platformBounds[])
{
    // Update attack cooldown
    if (isAttackOnCooldown) {
        attackCooldownTimer -= dt;
        if (attackCooldownTimer <= 0.f) {
            isAttackOnCooldown = false;  // Reset the flag
            attackCooldownTimer = 0.f;
            std::cout << "Attack cooldown finished - ready to attack!" << std::endl;
        }
    }

    
    // Update damage flash
    updateDamageFlash(dt);
    
    // Update current attack position to follow player
    if (currentAttack.isActive()) {
        float offsetX = facingRight ? 20.f : -20.f;
        float offsetY = -5.f;
        currentAttack.sprite.setPosition(getPosition().x + offsetX, getPosition().y + offsetY);
    }

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
    currentAttack.draw(window);
}

// Player-specific functions (implement these later)
void Player::jump() {
    if (onGround) {
        vy = jumpForce;
        onGround = false;
    }
}

// void Player::meleeAttack() {
//     // Implement attack logic later
//     isAttacking = true;
//     attackCooldown = 0.3f; // 300ms cooldown
// }

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
//   ENEMY IMPLEMENTATION
// =========================
Enemy::Enemy(float startX, float startY, float leftBound, float rightBound, float patrolSpeed, float detectionRadius)
    : leftBoundary_(leftBound), rightBoundary_(rightBound), patrolSpeed_(patrolSpeed),
      detectionRadius_(detectionRadius), playerDetected_(false),
      isAttackOnCooldown_(false), attackCooldownTimer_(0.f), attackDamage_(20),
      isPatrolling_(true), movingRight_(true)
{

    // Load enemy sprite
    sf::Image fullImage;
    if (fullImage.loadFromFile("enemy.png")) {
        // Use the larger frame rectangle you specified
        sf::IntRect frameRect(0, 0, 415, 326);
        sf::Image frameImage;
        frameImage.create(frameRect.width, frameRect.height, sf::Color::Transparent);
        frameImage.copy(fullImage, 0, 0, frameRect, true);
        texture.loadFromImage(frameImage);
    }

    sprite.setTexture(texture);
    sprite.setScale(0.35f, 0.35f); // Your scale
    sprite.setPosition(startX, startY);
    facingRight = true;
    
     currentAttack.setDamage(attackDamage_);
    
    // Initialize physics
    vx = 0;
    vy = 0;
    gravity = 800.f;
    onGround = false;
}



void Enemy::update(float dt, sf::FloatRect platformBounds[])
{
    // Update attack cooldown
    if (isAttackOnCooldown_) {
        attackCooldownTimer_ -= dt;
        if (attackCooldownTimer_ <= 0.f) {
            isAttackOnCooldown_ = false;
            attackCooldownTimer_ = 0.f;
        }
    }
    
    // Update damage flash
    updateDamageFlash(dt);
    
    // Update current attack
    currentAttack.update(dt);
    
    // Update current attack position to follow enemy
    if (currentAttack.isActive()) {
        float offsetX = facingRight ? 30.f : -30.f;
        float offsetY = -10.f;
        currentAttack.sprite.setPosition(getPosition().x + offsetX, getPosition().y + offsetY);
    }
    
    // Check if player is in detection range
    bool wasDetected = playerDetected_;
    playerDetected_ = isPlayerInRange(playerPosition_);
    
    if (playerDetected_) {
        // Stop patrolling
        isPatrolling_ = false;
        vx = 0;
        
        // Continuously face the player based on current position
        if (playerPosition_.x < getPosition().x) {
            // Player is to the left - face left
            facingRight = false;
            sprite.setScale(0.35f, 0.35f);
            sprite.setOrigin(0, 0);
        } else {
            // Player is to the right - face right
            facingRight = true;
            sprite.setScale(-0.35f, 0.35f);
            sprite.setOrigin(sprite.getLocalBounds().width, 0);
        }
        
        // Attack if cooldown is ready
        if (attackCooldownTimer_ <= 0.f) {
            performMeleeAttack();
        }
    } else {
        // Player left detection range - resume patrolling
        if (wasDetected && !isPatrolling_) {
            isPatrolling_ = true;
            // Keep the current facing direction for patrol
        }
        
        // Patrol movement
        if (isPatrolling_) {
            patrolMovement(dt);
        }
    }
    
    // Apply gravity
    vy += gravity * dt;
    
    // Store current position
    sf::Vector2f currentPos = sprite.getPosition();
    
    // Calculate new position
    sf::Vector2f newPos = currentPos + sf::Vector2f(vx * dt, vy * dt);
    
    // Apply horizontal movement first
    sprite.setPosition(newPos.x, currentPos.y);
    
    // Check horizontal collisions with platforms
    sf::FloatRect enemyBounds = getBounds();
    bool horizontalCollision = false;
    for (int i = 0; i < 7; i++) {
        if (enemyBounds.intersects(platformBounds[i])) {
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
    
    // Check vertical collisions with platforms
    enemyBounds = getBounds();
    onGround = false;
    
    for (int i = 0; i < 7; i++) {
        if (enemyBounds.intersects(platformBounds[i])) {
            sf::FloatRect platform = platformBounds[i];
            
            // Landing on platform from above
            if (vy > 0 && enemyBounds.top + enemyBounds.height > platform.top) {
                sprite.setPosition(sprite.getPosition().x, platform.top - enemyBounds.height);
                vy = 0;
                onGround = true;
            }
            // Hitting platform from below
            else if (vy < 0 && enemyBounds.top < platform.top + platform.height) {
                sprite.setPosition(sprite.getPosition().x, platform.top + platform.height);
                vy = 0;
            }
            break;
        }
    }
    if (playerDetected_ && !isAttackOnCooldown_) {
        performMeleeAttack();
    }

}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
    currentAttack.draw(window);
}



void Enemy::patrolMovement(float dt)
{
    // Move in current direction
    if (movingRight_) {
        vx = patrolSpeed_;
        facingRight = true;
        sprite.setScale(-0.35f, 0.35f);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    } else {
        vx = -patrolSpeed_;
        facingRight = false;
        sprite.setScale(0.35f, 0.35f);
        sprite.setOrigin(0, 0);
    }
    
    // Check boundaries and instantly turn
    sf::Vector2f currentPos = getPosition();
    if (currentPos.x >= rightBoundary_) {
        movingRight_ = false;
    } else if (currentPos.x <= leftBoundary_) {
        movingRight_ = true;
    }
}

void Enemy::updateAttackCooldown(float dt)
{
    if (attackCooldownTimer_ > 0.f) {
        attackCooldownTimer_ -= dt;
    }
}

bool Enemy::isPlayerInRange(const sf::Vector2f& playerPosition) const
{
    sf::Vector2f enemyPos = getPosition();
    float dx = playerPosition.x - enemyPos.x;
    float dy = playerPosition.y - enemyPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return distance <= detectionRadius_;
}

void Enemy::performMeleeAttack()
{
    if (!isAttackOnCooldown_) {
        currentAttack.activate(getPosition(), facingRight);
        isAttackOnCooldown_ = true;
        attackCooldownTimer_ = 2.0f; // 2 second cooldown
        }
}

// sf::Vector2f Enemy::getAttackRangeCenter() const
// {
//     return getPosition();
// }
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
//   MELEE ATTACK IMPLEMENTATION
// =========================
MeleeAttack::MeleeAttack()
    : active(false), activeTime(0.f), maxActiveTime(0.2f), damage(20)
{
    // Load attack texture
    if (!texture.loadFromFile("att.png")) {
        std::cout << "Failed to load attack texture: att.png" << std::endl;
        // Create a placeholder rectangle if texture fails to load
        sf::Image placeholder;
        placeholder.create(50, 50, sf::Color::Red);
        texture.loadFromImage(placeholder);
    }
    
    sprite.setTexture(texture);
    sprite.setScale(0.15f, 0.15f);
}

void MeleeAttack::update(float dt)
{
    if (active) {
        activeTime += dt;
        if (activeTime >= maxActiveTime) {
            active = false;
            activeTime = 0.f;
        }
    }
}

void MeleeAttack::draw(sf::RenderWindow& window)
{
    if (active) {
        window.draw(sprite);
    }
}

void MeleeAttack::activate(const sf::Vector2f& position, bool facingRight)
{
    this->active = true;
    this->activeTime = 0.f;
    
    // Position the attack sprite based on character facing direction
    float offsetX = facingRight ? 30.f : -30.f; // Smaller offset
    float offsetY = -10.f; // Adjust to position in hand
    
    sprite.setPosition(position.x + offsetX, position.y + offsetY);
    
    // Flip sprite based on direction
    if (facingRight) {
        sprite.setScale(0.15f, 0.15f);
        sprite.setOrigin(0, 0);
    } else {
        sprite.setScale(-0.15f, 0.15f);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }
}

sf::FloatRect MeleeAttack::getHitbox() const
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
    : background("bgimg.png"),
      platform1("platform.png", -20.f, 600.f),
      platform2("platform.png", 255.f, 600.f),
      platform3("platform.png", 530.f, 600.f),
      platform4("platform.png", 705.f, 600.f),
      platform5("platform.png", 980.f, 525.f),
      platform6("platform.png", 1255.f, 450.f),
      platform7("platform.png", 1530.f, 375.f),
      // Initialize enemy with: startX, startY, leftBound, rightBound, patrolSpeed, detectionRadius
    enemy(700.f, 200.f, 600.f, 800.f, 100.f, 50.f),  // Changed Y to 200 and increased detection radius,  // ADD THIS LINE
      healthBar(&player.health, player.maxHealth),
      soulBar(&player.soul)
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int width  = desktop.width  * 0.8f;
    unsigned int height = desktop.height * 0.8f;

    window.create(sf::VideoMode(width, height), "Hollow Knight Inspired Game");
    window.setFramerateLimit(60);
}

void Game::checkAttackCollisions()
{
    // Check if player's attack hits enemy (only if enemy is in range)
    MeleeAttack* playerAttack = player.getCurrentAttack();
    if (playerAttack->isActive() && 
        playerAttack->getHitbox().intersects(enemy.getBounds()) &&
        enemy.isPlayerInRange(player.getPosition())) {  // ADD THIS CHECK
        
        enemy.takeDamage(playerAttack->getDamage());
        enemy.triggerDamageFlash();
        std::cout << "Enemy hit! Health: " << enemy.health << std::endl;
        
        if (enemy.isDead()) {
            std::cout << "Enemy defeated!" << std::endl;
            enemy.health = enemy.maxHealth;
            enemy.setPosition(700.f, 200.f);
        }
    }
    
    // Check if enemy's attack hits player (only if player is in range)
    MeleeAttack* enemyAttack = enemy.getCurrentAttack();
    if (enemyAttack->isActive() && 
        enemyAttack->getHitbox().intersects(player.getBounds()) &&
        enemy.isPlayerInRange(player.getPosition())) {  // ADD THIS CHECK
        
        player.takeDamage(enemyAttack->getDamage());
        player.triggerDamageFlash();
        std::cout << "Player hit! Health: " << player.health << std::endl;
        
        if (player.isDead()) {
            std::cout << "Player died!" << std::endl;
            player.health = player.maxHealth;
            player.setPosition(100.f, 200.f);
        }
    }
}


// =========================
//   GAME RUN FUNCTION (ADD THIS BACK)
// =========================
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
        
        // Health damage when 'P' is pressed
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
            healthBar.takeDamage(10);
            std::cout << "Health reduced! Current health: " << player.health << std::endl;
        }
        
        // Soul increase when 'O' is pressed
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
            player.gainSoul(1);
            std::cout << "Soul increased! Current soul: " << player.soul << std::endl;
        }
        
        // Simple attack on J key press
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::J) {
            player.attack();
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
        platform7.getBounds()
    };
    
    player.update(dt, platformBounds);
    
    // Update enemy with player position for detection
    enemy.setPlayerPosition(player.getPosition());
    enemy.update(dt, platformBounds);
    
    // Update attack collisions
    checkAttackCollisions();
    
    // Update UI elements
    healthBar.update();
    soulBar.update();
}

void Game::render()
{
    window.clear();

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

    // Draw enemy (ADD THIS LINE)
    enemy.draw(window);

    // Draw UI elements last (on top of everything)
    healthBar.draw(window);
    soulBar.draw(window);

    window.display();
}