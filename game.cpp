#include "game.hpp"


Player::Player() :  onGround(true)
{
    sf::Image fullImage;
    if (!fullImage.loadFromFile("player.png"))
        std::cout << "Failed to load sprite sheet!\n";

    sf::IntRect frameRect(4025, 3965, 71, 130);
    sf::Image frameImage;
    frameImage.create(frameRect.width, frameRect.height);
    frameImage.copy(fullImage, 0, 0, frameRect, false);

    if (!texture.loadFromImage(frameImage))
        std::cout << "Failed to create frame texture!\n";

    sprite.setTexture(texture);
    sprite.setScale(-0.75f, 0.75f);
    sprite.setOrigin(sprite.getLocalBounds().width, 0);
    // sprite.setPosition(100.f, 500.f - sprite.getGlobalBounds().height);
    sprite.setPosition(100.f, 540.f);

}

void Player::update(float dt , sf::FloatRect platformBounds[])
{
    
    // bool moveright = (pos.x <= 960.f || (pos.y <= 540.f && pos.y > 465.f));

    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    //     sprite.move(-speed * dt, 0);
    //     sprite.setScale(0.75f, 0.75f);
    //     sprite.setOrigin(0, 0);
    // }
    // else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && moveright) {
    //     sprite.move(speed * dt, 0);
    //     sprite.setScale(-0.75f, 0.75f);
    //     sprite.setOrigin(sprite.getLocalBounds().width, 0);
    // }
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {
    //     timer = 1.f;
    //     onGround = false;
    // }
    // if (sprite.getPosition().y < 540 && onGround == true) {
    //     sprite.move(0, speed * dt * 2);
    // } else if (sprite.getPosition().y > 540) {
    //     onGround = true;
    //     timer = 0.f;
    //     sprite.setPosition(sprite.getPosition().x, 540.f);
    // } else if (timer > 0.5 && onGround == false) {
    //     sprite.move(0, -speed * dt * 2);
    //     timer -= dt;
    // } else if (timer > 0 && onGround == false) {
    //     sprite.move(0, speed * dt * 2);
    //     timer -= dt;
    // } else {
    //     timer = 0;
    //     onGround = true;
    // }
    float speed = 200.f;
    sf::Vector2f pos = sprite.getPosition();
    
    bool moveright = (pos.x <= 960.f || (pos.x > 960.f && pos.x <= 1235 && pos.y < 480.f )
    || (pos.x > 1235.f && pos.y < 405.f));
    if (pos.y >= 540.f && pos.x <= 960.f) {
        onGround = true;
    } else if (pos.y >= 465.f && pos.x > 960.f) {
        onGround = true;
    } else if (pos.y >= 465.f && pos.x <= 960.f) {
        onGround = false;
    } else if (pos.y >= 390.f && pos.x > 1235.f) {
        onGround = true;
    } else if (pos.y >= 390.f && pos.x <= 1235.f) {
        onGround = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        sprite.move(-speed * dt, 0);
        sprite.setScale(0.75f, 0.75f);
        sprite.setOrigin(0, 0);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && moveright) {
        sprite.move(speed * dt, 0);
        sprite.setScale(-0.75f, 0.75f);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {
        timer = 0.5f;
        onGround = false;
    }
    if (timer > 0.f && !onGround) {
        sprite.move(0, -speed * dt * 2);
        timer -= dt;
    } else {
        timer = 0;
    }
    if (!onGround && timer == 0) {
        sprite.move(0, speed * dt * 2);
    }
    std::cout << "Y pos:" << pos.y << " moveright:" << moveright << std::endl;

}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
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
//   GAME IMPLEMENTATION
// =========================
Game::Game()
    : platform1("platform.png", -20.f, 600.f),
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
    while (window.isOpen()) {
        processEvents();
        float dt = clock.restart().asSeconds();
        update(dt);
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
        platform6.getBounds()
    };
    player.update(dt , platformBounds);
}

void Game::render()
{
    window.clear(sf::Color::Cyan);

    platform1.draw(window);
    platform2.draw(window);
    platform3.draw(window);
    platform4.draw(window);
    platform5.draw(window);
    platform6.draw(window);
    platform7.draw(window);

    player.draw(window);

    window.display();
}
