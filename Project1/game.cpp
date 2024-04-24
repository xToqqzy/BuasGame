#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

// Define constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PLAYER_SPEED = 900.0f;
const float ITEM_SPEED = 200.0f;
const int NUM_BOMBS = 2;
const float FRUIT_SPAWN_INTERVAL = 0.4f;
const float BOMB_SPAWN_INTERVAL = 1.5f;
const int SCORE_PER_FRUIT = 10;

// Player class
class Player {
public:
    sf::Sprite sprite;
    sf::Vector2f velocity;

    Player(sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setScale(1.5f, 1.5f);
        sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100);
    }

    void update(float deltaTime) {
        // Update player position while keeping it within bounds
        sf::Vector2f nextPosition = sprite.getPosition() + velocity * deltaTime;
        nextPosition.x = std::max(0.0f, std::min(WINDOW_WIDTH - sprite.getGlobalBounds().width, nextPosition.x));
        nextPosition.y = std::max(0.0f, std::min(WINDOW_HEIGHT - sprite.getGlobalBounds().height, nextPosition.y));
        sprite.setPosition(nextPosition);
    }
};

// Item class (base class for fruits and bombs)
class Item {
public:
    sf::Sprite sprite;
    int points;
    bool collected;

    Item(sf::Texture& texture, float posX, float posY, int points) {
        sprite.setTexture(texture);
        sprite.setPosition(posX, posY);
        this->points = points;
        collected = false;
    }

    virtual void update(float deltaTime) {
        if (!collected)
            sprite.move(0, ITEM_SPEED * deltaTime);
    }
};

// Fruit classes
class Apple : public Item {
public:
    Apple(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, SCORE_PER_FRUIT) {} // Apple worth SCORE_PER_FRUIT points
};

class Watermelon : public Item {
public:
    Watermelon(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, SCORE_PER_FRUIT) {} // Watermelon worth SCORE_PER_FRUIT points
};

class Banana : public Item {
public:
    Banana(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, SCORE_PER_FRUIT) {} // Banana worth SCORE_PER_FRUIT points
};

// Bomb class
class Bomb : public Item {
public:
    Bomb(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, -50) {} // Bomb deducts 50 points
};

void handleEscapeMenu(sf::RenderWindow& window, bool& gamePaused) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font file" << std::endl;
        return;
    }

    // Create a transparent background for the pause menu
    sf::RectangleShape pauseOverlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 100));

    // Create pause text
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setString("Paused");
    pauseText.setCharacterSize(48);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(WINDOW_WIDTH / 2 - pauseText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - pauseText.getGlobalBounds().height / 2);

    // Create quit text with some space
    sf::Text quitText;
    quitText.setFont(font);
    quitText.setString("Quit Game (Press Q)");
    quitText.setCharacterSize(24);
    quitText.setFillColor(sf::Color::White);
    quitText.setPosition(WINDOW_WIDTH / 2 - quitText.getGlobalBounds().width / 2, pauseText.getPosition().y + 80); // Adjusted position

    window.draw(pauseOverlay);
    window.draw(pauseText);
    window.draw(quitText);
    window.display();

    bool paused = true;
    while (paused) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                    return;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    paused = false;
                    gamePaused = false;
                    return;
                }
            }
        }
    }
}


// Function to handle game over menu
bool handleGameOverMenu(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font file" << std::endl;
        return false;
    }

    // Create a transparent background for the game over menu
    sf::RectangleShape gameOverOverlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 100));

    // Create game over text
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Oh no... You died.\nWanna play again?");
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - gameOverText.getGlobalBounds().height / 2 - 40);

    // Create play again text
    sf::Text playAgainText;
    playAgainText.setFont(font);
    playAgainText.setString("Play again (Press R)");
    playAgainText.setCharacterSize(24);
    playAgainText.setFillColor(sf::Color::White);
    playAgainText.setPosition(WINDOW_WIDTH / 2 - playAgainText.getGlobalBounds().width / 2, gameOverText.getPosition().y + 100);

    // Create quit text
    sf::Text quitText;
    quitText.setFont(font);
    quitText.setString("Quit Game (Press Q)");
    quitText.setCharacterSize(24);
    quitText.setFillColor(sf::Color::White);
    quitText.setPosition(WINDOW_WIDTH / 2 - quitText.getGlobalBounds().width / 2, playAgainText.getPosition().y + 60);

    window.draw(gameOverOverlay);
    window.draw(gameOverText);
    window.draw(playAgainText);
    window.draw(quitText);
    window.display();

    while (true) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R)
                    return true; // Restart game
                else if (event.key.code == sf::Keyboard::Q)
                    return false; // Quit game
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Fruit Picker");

    // Create boundaries
    sf::RectangleShape boundary;
    boundary.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    boundary.setFillColor(sf::Color::Transparent);
    boundary.setOutlineThickness(2.0f);
    boundary.setOutlineColor(sf::Color::White);

    // Load textures...
    sf::Texture appleTexture;
    if (!appleTexture.loadFromFile("assets/apple.png")) {
        std::cerr << "Failed to load apple.png" << std::endl;
        return 1;
    }

    sf::Texture bananaTexture;
    if (!bananaTexture.loadFromFile("assets/banana.png")) {
        std::cerr << "Failed to load banana.png" << std::endl;
        return 1;
    }

    sf::Texture watermelonTexture;
    if (!watermelonTexture.loadFromFile("assets/watermelon.png")) {
        std::cerr << "Failed to load watermelon.png" << std::endl;
        return 1;
    }

    sf::Texture bombTexture;
    if (!bombTexture.loadFromFile("assets/bomb.png")) {
        std::cerr << "Failed to load bomb.png" << std::endl;
        return 1;
    }

    // Load other textures...

    // Create player...
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load player.png" << std::endl;
        return 1;
    }
    Player player(playerTexture);

    // Create vectors for items and bombs...
    std::vector<Item*> items;
    std::vector<Bomb> bombs;

    int score = 0;

    bool gamePaused = false;

    // Game loop
    sf::Clock clock;
    float timeSinceLastFruitSpawn = 0.0f;
    float timeSinceLastBombSpawn = 0.0f;
    bool gameOver = false;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        float dtSeconds = deltaTime.asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape && !gamePaused) {
                    gamePaused = true;
                    handleEscapeMenu(window, gamePaused);
                }
            }
        }

        if (!gamePaused && !gameOver) {
            // Player movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                player.velocity.x = -PLAYER_SPEED;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                player.velocity.x = PLAYER_SPEED;
            else
                player.velocity.x = 0;

            // Update player
            player.update(dtSeconds);

            // Spawn fruits
            timeSinceLastFruitSpawn += dtSeconds;
            if (timeSinceLastFruitSpawn > FRUIT_SPAWN_INTERVAL) {
                items.push_back(new Apple(appleTexture, rand() % WINDOW_WIDTH, -100));
                items.push_back(new Watermelon(watermelonTexture, rand() % WINDOW_WIDTH, -200));
                items.push_back(new Banana(bananaTexture, rand() % WINDOW_WIDTH, -300));
                timeSinceLastFruitSpawn = 0.0f;
            }

            // Spawn bombs
            timeSinceLastBombSpawn += dtSeconds;
            if (timeSinceLastBombSpawn > BOMB_SPAWN_INTERVAL) {
                for (int i = 0; i < NUM_BOMBS; ++i) {
                    bombs.push_back(Bomb(bombTexture, rand() % WINDOW_WIDTH, -400));
                }
                timeSinceLastBombSpawn = 0.0f;
            }

            // Item collisions (fruits and bombs)
            for (auto& item : items) {
                if (item->sprite.getGlobalBounds().intersects(player.sprite.getGlobalBounds()) && !item->collected) {
                    item->collected = true;
                    score += item->points;
                }
            }
            for (auto& bomb : bombs) {
                if (bomb.sprite.getGlobalBounds().intersects(player.sprite.getGlobalBounds())) {
                    // Game over
                    gameOver = true;
                    if (handleGameOverMenu(window)) {
                        // Restart game
                        for (auto& item : items)
                            delete item;
                        items.clear();
                        bombs.clear();
                        score = 0;
                        player.sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100);
                        gameOver = false;
                    }
                    else {
                        // Quit game
                        window.close();
                    }
                }
            }

            // Update items (fruits and bombs)
            for (auto it = items.begin(); it != items.end();) {
                (*it)->update(dtSeconds);
                if ((*it)->sprite.getPosition().y > WINDOW_HEIGHT) {
                    delete* it;
                    it = items.erase(it);
                }
                else {
                    ++it;
                }
            }
            for (auto& bomb : bombs)
                bomb.update(dtSeconds);
        }

        // Clear window
        window.clear();

        // Draw boundaries
        window.draw(boundary);

        // Draw player
        window.draw(player.sprite);

        // Draw items (fruits and bombs)
        for (const auto& item : items) {
            if (!item->collected)
                window.draw(item->sprite);
        }

        // Draw bombs
        for (const auto& bomb : bombs) {
            if (!bomb.collected)
                window.draw(bomb.sprite);
        }

        // Draw score
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font file" << std::endl;
            return 1;
        }
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        // Display content
        window.display();
    }

    // Clean up dynamically allocated memory
    for (auto& item : items)
        delete item;

    return 0;
}
