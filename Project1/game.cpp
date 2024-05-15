#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "TileMap.h"

// Define constants
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PLAYER_SPEED = 900.0f;
const float ITEM_SPEED = 200.0f;
const int NUM_BOMBS = 2;
const float FRUIT_SPAWN_INTERVAL = 0.4f;
const float BOMB_SPAWN_INTERVAL = 1.5f;
const int SCORE_PER_FRUIT = 10;
const float PLAYER_WIDTH = 150.0f;
const float PLAYER_HEIGHT = 350.0f;
const float COLLISION_REDUCTION = 10.0f;

// Define constants for animation
const int FRAME_WIDTH = 24; // Width of each frame
const int FRAME_HEIGHT = 24; // Height of each frame
const int NUM_FRAMES = 3; // Number of frames in the animation (neutral, left, right)

// Player class
class Player {
public:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    int currentFrame; // Current frame index for animation

    Player(sf::Texture& texture) : currentFrame(0) {
        sprite.setTexture(texture);
        sprite.setScale(1.5f, 1.5f);
        // Set initial texture rectangle to display the neutral frame
        sprite.setTextureRect(sf::IntRect(FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT));
        // Adjust starting position to the bottom of the window
        sprite.setPosition((WINDOW_WIDTH - PLAYER_WIDTH) / 2, WINDOW_HEIGHT - PLAYER_HEIGHT);
    }

    void update(float deltaTime) {
        // Update player position while keeping it within bounds
        sf::Vector2f nextPosition = sprite.getPosition() + velocity * deltaTime;
        nextPosition.x = std::max(0.0f, std::min(WINDOW_WIDTH - PLAYER_WIDTH, nextPosition.x));
        nextPosition.y = std::max(0.0f, std::min(WINDOW_HEIGHT - PLAYER_HEIGHT, nextPosition.y));
        sprite.setPosition(nextPosition);

        // Update animation frame based on velocity
        if (velocity.x < 0) {
            currentFrame = 17; // Left animation frame
        }
        else if (velocity.x > 0) {
            currentFrame = 18; // Right animation frame
        }
        else {
            currentFrame = 0; // Neutral frame
        }
        // Update texture rectangle for animation
        sprite.setTextureRect(sf::IntRect(currentFrame * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT));
    }
};

// Texture loading function
void loadPlayerTexture(sf::Texture& texture) {
    if (!texture.loadFromFile("assets/player_spritesheet.png")) {
        std::cerr << "Failed to load player_spritesheet.png" << std::endl;
        exit(1);
    }
}

// Function to load tree texture
void loadTreeTexture(sf::Texture& texture) {
    if (!texture.loadFromFile("assets/tree.png")) {
        std::cerr << "Failed to load tree.png" << std::endl;
        exit(1);
    }
}

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

    virtual sf::FloatRect getCollisionBounds() const {
        // By default, use the sprite's global bounds for collision detection
        return sprite.getGlobalBounds();
    }
};

// Fruit classes
class Apple : public Item {
public:
    Apple(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, SCORE_PER_FRUIT) {} // Apple worth SCORE_PER_FRUIT points
};

// Bomb class
class Bomb : public Item {
public:
    Bomb(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, -50) {} // Bomb deducts 50 points

    // Override the getCollisionBounds function to adjust bomb collision bounds
    sf::FloatRect getCollisionBounds() const override {
        // Adjust collision bounds based on current animation frame
        sf::FloatRect bounds = sprite.getGlobalBounds();
        // Reduce bounds by COLLISION_REDUCTION pixels on each side
        bounds.left += COLLISION_REDUCTION;
        bounds.width -= 2 * COLLISION_REDUCTION;
        bounds.top += COLLISION_REDUCTION;
        bounds.height -= 2 * COLLISION_REDUCTION;
        return bounds;
    }
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
    // Get desktop resolution
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    // Create a fullscreen window with desktop resolution
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height), "Fruit Picker", sf::Style::Fullscreen);

    // Load textures
    sf::Texture appleTexture;
    if (!appleTexture.loadFromFile("assets/apple.png")) {
        std::cerr << "Failed to load apple.png" << std::endl;
        return 1;
    }

    sf::Texture bombTexture;
    if (!bombTexture.loadFromFile("assets/bomb.png")) {
        std::cerr << "Failed to load bomb.png" << std::endl;
        return 1;
    }

    sf::Texture playerTexture;
    loadPlayerTexture(playerTexture); // Load player sprite sheet
    // Ensure that the player texture dimensions are divisible by the frame dimensions
    if (playerTexture.getSize().x % FRAME_WIDTH != 0 || playerTexture.getSize().y != FRAME_HEIGHT) {
        std::cerr << "Player sprite sheet dimensions are invalid" << std::endl;
        return 1;
    }

    sf::Texture treeTexture;
    loadTreeTexture(treeTexture); // Load tree texture

    // Create player
    Player player(playerTexture);

    // Create the middle tree
    sf::Sprite middleTree;
    middleTree.setTexture(treeTexture);
    middleTree.setPosition((WINDOW_WIDTH - middleTree.getGlobalBounds().width) / 2, 0);

    // Create vectors for items and bombs
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

            // Spawn fruits from the top with random X positions across multiple lines
            timeSinceLastFruitSpawn += dtSeconds;
            if (timeSinceLastFruitSpawn > FRUIT_SPAWN_INTERVAL) {
                for (int i = 0; i < 2; ++i) { // Reduce the number of lines
                    float posX = std::rand() % (WINDOW_WIDTH - 200) + 100; // Random X position across the screen (avoiding edges)
                    float posY = 50 * (i + 1); // Start above the screen, increment Y for each line
                    items.push_back(new Apple(appleTexture, posX, posY));
                }
                timeSinceLastFruitSpawn = 0.0f;
            }

            // Spawn bombs from the top with random X positions across multiple lines
            timeSinceLastBombSpawn += dtSeconds;
            if (timeSinceLastBombSpawn > BOMB_SPAWN_INTERVAL) {
                for (int i = 0; i < 2; ++i) { // Reduce the number of lines
                    float posX = std::rand() % (WINDOW_WIDTH - 200) + 100; // Random X position across the screen (avoiding edges)
                    float posY = 50 * (i + 1); // Start above the screen, increment Y for each line
                    bombs.push_back(Bomb(bombTexture, posX, posY));
                }
                timeSinceLastBombSpawn = 0.0f;
            }

            // Item collisions (fruits and bombs)
            for (auto& item : items) {
                if (item->getCollisionBounds().intersects(player.sprite.getGlobalBounds()) && !item->collected) {
                    item->collected = true;
                    score += item->points;
                }
            }

            for (auto& bomb : bombs) {
                if (bomb.getCollisionBounds().intersects(player.sprite.getGlobalBounds())) {
                    // Game over
                    gameOver = true;
                    if (handleGameOverMenu(window)) {
                        // Restart game
                        for (auto& item : items)
                            delete item;
                        items.clear();
                        bombs.clear();
                        score = 0;
                        player.sprite.setPosition((WINDOW_WIDTH - PLAYER_WIDTH) / 2, WINDOW_HEIGHT - PLAYER_HEIGHT);
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

        // Draw the middle tree
        window.draw(middleTree);

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

    return 0;
}
