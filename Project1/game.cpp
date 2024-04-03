#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Define constants
const int NUM_OBJECTS = 10;
const int NUM_ENEMIES = 5;
const float OBJECT_RADIUS = 20.0f;
const float ENEMY_SPEED = 0.6f; // Increased enemy speed
const float PLAYER_SPEED = 2.4f; // Player movement speed

// Player class
class Player {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;

    Player(float windowWidth, float windowHeight) {
        if (!texture.loadFromFile("assets/player.png")) {
            // Handle error if texture loading fails
        }
        sprite.setTexture(texture);
        sprite.setPosition(windowWidth / 2, windowHeight / 2);
    }

    void update(float windowWidth, float windowHeight) {
        sprite.move(velocity);

        // Constrain player within window boundaries
        if (sprite.getPosition().x < 0) {
            sprite.setPosition(0, sprite.getPosition().y);
        }
        if (sprite.getPosition().x > windowWidth - sprite.getGlobalBounds().width) {
            sprite.setPosition(windowWidth - sprite.getGlobalBounds().width, sprite.getPosition().y);
        }
        if (sprite.getPosition().y < 0) {
            sprite.setPosition(sprite.getPosition().x, 0);
        }
        if (sprite.getPosition().y > windowHeight - sprite.getGlobalBounds().height) {
            sprite.setPosition(sprite.getPosition().x, windowHeight - sprite.getGlobalBounds().height);
        }
    }
};

// Object class
class Object {
public:
    sf::CircleShape shape;
    bool isCollected;

    Object(float x, float y) {
        shape.setRadius(OBJECT_RADIUS);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
        isCollected = false;
    }
};

// Enemy class
class Enemy {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;

    Enemy(float x, float y) {
        if (!texture.loadFromFile("assets/enemy.png")) {
            // Handle error if texture loading fails
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.5f, 0.5f); // Scale the sprite to half its original size
        velocity.x = ENEMY_SPEED * (rand() % 3 + 1); // Random speed between 1 and 3 times ENEMY_SPEED
        velocity.y = ENEMY_SPEED * (rand() % 3 + 1); // Random speed between 1 and 3 times ENEMY_SPEED
    }

    void update(float windowWidth, float windowHeight) {
        sprite.move(velocity);
        // Bounce off the window edges
        if (sprite.getPosition().x < 0 || sprite.getPosition().x > windowWidth)
            velocity.x = -velocity.x;
        if (sprite.getPosition().y < 0 || sprite.getPosition().y > windowHeight)
            velocity.y = -velocity.y;
    }
};

// Button class
class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;

    Button(float x, float y, float width, float height, std::string buttonText, sf::Font& font) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color(0, 0, 0, 128)); // Transparent black color

        text.setFont(font);
        text.setString(buttonText);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(x + 10, y + 10);
    }

    bool isClicked(const sf::Vector2f& mousePosition) {
        return shape.getGlobalBounds().contains(mousePosition);
    }
};

// Load Level 1 assets and configurations
void loadLevel1(std::vector<Object>& objects, std::vector<Enemy>& enemies, sf::Texture& backgroundTexture, float windowWidth, float windowHeight) {
    // Load background texture for Level 1
    if (!backgroundTexture.loadFromFile("assets/background_level1.jpg")) {
        // Handle error if texture loading fails
    }

    // Create objects to collect for Level 1
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        float x = static_cast<float>(rand() % static_cast<int>(windowWidth));
        float y = static_cast<float>(rand() % static_cast<int>(windowHeight));
        objects.push_back(Object(x, y));
    }

    // Create enemies for Level 1
    for (int i = 0; i < NUM_ENEMIES; ++i) {
        float x = static_cast<float>(rand() % static_cast<int>(windowWidth));
        float y = static_cast<float>(rand() % static_cast<int>(windowHeight));
        enemies.push_back(Enemy(x, y));
    }
}

// Function to handle game over screen
void handleGameOver(sf::RenderWindow& window, sf::Font& font) {
    // Game over message
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 2 - 40);

    // Restart button
    Button restartButton(window.getSize().x / 2 - 50, window.getSize().y / 2 + 20, 100, 50, "Restart", font);

    // Quit button
    Button quitButton(window.getSize().x / 2 - 50, window.getSize().y / 2 + 90, 100, 50, "Quit Game", font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (restartButton.isClicked(mousePosition)) {
                    // Restart the game
                    return;
                }
                else if (quitButton.isClicked(mousePosition)) {
                    // Quit the game
                    window.close();
                }
            }
        }

        // Clear the window
        window.clear();

        // Draw game over message
        window.draw(gameOverText);

        // Draw restart button
        window.draw(restartButton.shape);
        window.draw(restartButton.text);

        // Draw quit button
        window.draw(quitButton.shape);
        window.draw(quitButton.text);

        // Display the window
        window.display();
    }
}

// Function to handle level completion
void handleLevelCompletion(sf::RenderWindow& window, sf
    ::Font& font, bool& gameRestarted) {
    // Level completed message
    sf::Text levelCompletedText;
    levelCompletedText.setFont(font);
    levelCompletedText.setString("Level Completed!");
    levelCompletedText.setCharacterSize(40);
    levelCompletedText.setFillColor(sf::Color::White);
    levelCompletedText.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 - 40);

    // Restart button
    Button restartButton(window.getSize().x / 2 - 50, window.getSize().y / 2 + 20, 100, 50, "Restart Level", font);

    // Quit button
    Button quitButton(window.getSize().x / 2 - 50, window.getSize().y / 2 + 90, 100, 50, "Quit Game", font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (restartButton.isClicked(mousePosition)) {
                    // Restart the game
                    gameRestarted = true;
                    return;
                }
                else if (quitButton.isClicked(mousePosition)) {
                    // Quit the game
                    window.close();
                }
            }
        }

        // Clear the window
        window.clear();

        // Draw level completed message
        window.draw(levelCompletedText);

        // Draw restart button
        window.draw(restartButton.shape);
        window.draw(restartButton.text);

        // Draw quit button
        window.draw(quitButton.shape);
        window.draw(quitButton.text);

        // Display the window
        window.display();
    }
}

int main() {
    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Get the screen resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float WINDOW_WIDTH = static_cast<float>(desktop.width);
    float WINDOW_HEIGHT = static_cast<float>(desktop.height);

    // Create the window
    sf::RenderWindow window(sf::VideoMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT)), "Collect Game", sf::Style::Fullscreen);

    // Load background texture
    sf::Texture backgroundTexture;
    std::vector<Object> objects;
    std::vector<Enemy> enemies;

    // Load Level 1
    loadLevel1(objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Scale the background sprite to fit the window size
    float scaleX = WINDOW_WIDTH / backgroundTexture.getSize().x;
    float scaleY = WINDOW_HEIGHT / backgroundTexture.getSize().y;
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(scaleX, scaleY);

    // Load sound effect
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("assets/collect_sound.wav")) {
        // Handle error if sound loading fails
    }
    sf::Sound collectSound(buffer);

    // Load player character
    Player player(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Score counter
    int score = 0;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Handle error if font loading fails
    }

    // Quit button
    Button quitButton(WINDOW_WIDTH - 120, 10, 100, 50, "Quit Game", font);

    // Game loop
    bool gameRestarted = false; // Flag to indicate if game has been restarted
    bool gamePaused = false; // Flag to indicate if game is paused
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameRestarted && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (quitButton.isClicked(mousePosition)) {
                    window.close();
                }
            }

            if (!gameRestarted && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
                // Toggle pause menu
                gamePaused = !gamePaused;
                if (gamePaused) {
                    // If paused, show the pause menu
                    // Pause menu buttons
                    Button continueButton(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 20, 100, 50, "Continue", font);
                    Button restartLevelButton(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 50, 100, 50, "Restart Level", font);
                    Button quitButton(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 120,
                        100, 50, "Quit Game", font);

                    while (window.isOpen() && gamePaused) {
                        sf::Event pauseEvent;
                        while (window.pollEvent(pauseEvent)) {
                            if (pauseEvent.type == sf::Event::Closed) {
                                window.close();
                            }
                            if (pauseEvent.type == sf::Event::MouseButtonPressed) {
                                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(pauseEvent.mouseButton.x, pauseEvent.mouseButton.y));
                                if (continueButton.isClicked(mousePosition)) {
                                    // Continue the game
                                    gamePaused = false;
                                }
                                else if (restartLevelButton.isClicked(mousePosition)) {
                                    // Restart the level
                                    // Reload Level 1
                                    // Clear objects and enemies vectors
                                    objects.clear();
                                    enemies.clear();
                                    loadLevel1(objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
                                    // Reset player position
                                    player.sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                                    // Reset game over flag
                                    gameRestarted = true;
                                    gamePaused = false;
                                }
                                else if (quitButton.isClicked(mousePosition)) {
                                    // Quit the game
                                    window.close();
                                }
                            }
                        }

                        // Clear the window
                        window.clear();

                        // Draw pause menu
                        // Draw continue button
                        window.draw(continueButton.shape);
                        window.draw(continueButton.text);

                        // Draw restart level button
                        window.draw(restartLevelButton.shape);
                        window.draw(restartLevelButton.text);

                        // Draw quit button
                        window.draw(quitButton.shape);
                        window.draw(quitButton.text);

                        // Display the window
                        window.display();
                    }
                }
            }
        }

        // Restart the game if restarted
        if (gameRestarted) {
            gameRestarted = false; // Reset the flag
        }

        // Continue the game if not paused
        if (!gamePaused) {
            // Handle player controls
            player.velocity.x = 0;
            player.velocity.y = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                player.velocity.x = -PLAYER_SPEED;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                player.velocity.x = PLAYER_SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                player.velocity.y = -PLAYER_SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                player.velocity.y = PLAYER_SPEED;

            // Update player position
            player.update(WINDOW_WIDTH, WINDOW_HEIGHT);

            // Collision detection and scoring
            bool allCollected = true; // Flag to check if all collectibles are collected
            for (auto& obj : objects) {
                if (!obj.isCollected && player.sprite.getGlobalBounds().intersects(obj.shape.getGlobalBounds())) {
                    // Play collect sound
                    collectSound.play();
                    // Increment score
                    score++;
                    // Mark object as collected
                    obj.isCollected = true;
                }
                if (!obj.isCollected) {
                    allCollected = false; // At least one object is not collected
                }
            }

            // Check if all collectibles are collected
            if (allCollected) {
                // Handle level completion
                handleLevelCompletion(window, font, gameRestarted);
                // Reset score
                score = 0;
                // Restart the game
                objects.clear();
                enemies.clear();
                loadLevel1(objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
                // Reset player position
                player.sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            }

            // Update enemies position
            for (auto& enemy : enemies) {
                enemy.update(WINDOW_WIDTH, WINDOW_HEIGHT);
                // Collision detection with player
                if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                    // Game over
                    handleGameOver(window, font);
                    // Reset score
                    score = 0;
                    // Restart the game
                    objects.clear();
                    enemies.clear();
                    loadLevel1(objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
                    // Reset player position
                    player.sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                    // Reset game over flag
                    gameRestarted = true;
                }
            }
        }

        // Clear the window
        window.clear();

        // Draw background
        window.draw(backgroundSprite);

        // Draw objects
        for (auto& obj : objects) {
            if (!obj.isCollected) {
                window.draw(obj.shape);
            }
        }

        // Draw enemies
        for (auto& enemy : enemies) {
            window.draw(enemy.sprite);
        }

        // Draw player
        window.draw(player.sprite);

        // Draw score
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        // Draw quit button
        window.draw(quitButton.shape);
        window.draw(quitButton.text);

        // Display the window
        window.display();
    }

    return 0;
}


