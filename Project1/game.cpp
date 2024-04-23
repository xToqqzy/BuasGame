#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>

// Define constants
const int NUM_OBJECTS = 10;
const int NUM_ENEMIES = 5;
const float OBJECT_RADIUS = 20.0f;
const float ENEMY_SPEED = 0.8f; // Increased enemy speed
const float PLAYER_SPEED = 2.0f; // Player movement speed

// Define a structure to represent a player
struct Player {
    std::string name;
    int score;
};

// Function to save player data to a file
void savePlayerData(const std::vector<Player>& players) {
    std::ofstream file("leaderboard.txt");
    if (file.is_open()) {
        for (const Player& player : players) {
            file << player.name << " " << player.score << std::endl;
        }
        file.close();
    }
}

// Function to load player data from a file
std::vector<Player> loadPlayerData() {
    std::vector<Player> players;
    std::ifstream file("leaderboard.txt");
    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score) {
            players.push_back({ name, score });
        }
        file.close();
    }
    return players;
}

// Function to display the leaderboard
void displayLeaderboard(const std::vector<Player>& players) {
    std::cout << "Top 10 Leaderboard:\n";
    int rank = 1;
    for (const Player& player : players) {
        std::cout << rank << ". " << player.name << ": " << player.score << std::endl;
        rank++;
        if (rank > 10) break; // Display only the top 10 players
    }
}

// Function to update the leaderboard with the current player's score
void updateLeaderboard(std::vector<Player>& players, const Player& currentPlayer) {
    players.push_back(currentPlayer);
    std::sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        return a.score > b.score;
        });
    if (players.size() > 10) {
        players.pop_back(); // Keep only the top 10 players
    }
}

// Player class
class PlayerObject {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;

    PlayerObject(float windowWidth, float windowHeight) {
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

    void respawn(float windowWidth, float windowHeight) {
        // Respawn object at a random position
        shape.setPosition(static_cast<float>(rand() % static_cast<int>(windowWidth)),
            static_cast<float>(rand() % static_cast<int>(windowHeight)));
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
        // Assign random velocity
        velocity.x = (rand() % 2 == 0 ? ENEMY_SPEED : -ENEMY_SPEED); // Randomize direction
        velocity.y = (rand() % 2 == 0 ? ENEMY_SPEED : -ENEMY_SPEED); // Randomize direction
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

void handleGameOver(sf::RenderWindow& window, sf::Font& font, int& score, std::vector<Object>& objects, std::vector<Enemy>& enemies, sf::Texture& backgroundTexture, float windowWidth, float windowHeight, PlayerObject& player) {
    // Game over message
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over\nScore: " + std::to_string(score));
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 4); // Adjusted position

    // Return to Menu button
    Button returnToMenuButton(window.getSize().x / 2 - 120, window.getSize().y / 2 + 90, 200, 50, "Menu", font);

    // Restart button
    Button restartButton(window.getSize().x / 2 + 20, window.getSize().y / 2 + 90, 200, 50, "Restart", font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (returnToMenuButton.isClicked(mousePosition)) {
                    // Reset score
                    score = 0;
                    // Reset objects and enemies
                    objects.clear();
                    enemies.clear();
                    loadLevel1(objects, enemies, backgroundTexture, windowWidth, windowHeight);
                    // Reset player position
                    player.sprite.setPosition(windowWidth / 2, windowHeight / 2);
                    // Close the game over window
                    return;
                }
                else if (restartButton.isClicked(mousePosition)) {
                    // Reset score
                    score = 0;
                    // Reset objects and enemies
                    objects.clear();
                    enemies.clear();
                    loadLevel1(objects, enemies, backgroundTexture, windowWidth, windowHeight);
                    // Reset player position
                    player.sprite.setPosition(windowWidth / 2, windowHeight / 2);
                    // Close the game over window
                    return;
                }
            }
        }

        // Clear the window
        window.clear();

        // Draw game over message
        window.draw(gameOverText);

        // Draw return to menu button
        window.draw(returnToMenuButton.shape);
        window.draw(returnToMenuButton.text);

        // Draw restart button
        window.draw(restartButton.shape);
        window.draw(restartButton.text);

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
    PlayerObject player(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Score counter
    int score = 0;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Handle error if font loading fails
    }

    // Title screen
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString("Collect Game\nPress P to Play\nPress Q to Quit");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 4);

    // Quit button
    Button quitButton(WINDOW_WIDTH - 120, 10, 100, 50, "Quit Game", font);

    // Game loop
    bool inGame = false; // Flag to indicate if the game is running
    bool paused = false; // Flag to indicate if the game is paused
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P && !inGame && !paused) {
                    inGame = true; // Start the game
                }
                else if (event.key.code == sf::Keyboard::Escape && inGame) {
                    paused = true; // Toggle pause state
                }
                else if (event.key.code == sf::Keyboard::Q && !inGame && !paused) {
                    window.close(); // Quit if in title screen
                }
            }
        }

        if (!inGame) {
            // Clear the window
            window.clear();

            // Draw title screen
            window.draw(titleText);

            // Draw quit button
            window.draw(quitButton.shape);
            window.draw(quitButton.text);

            // Display the window
            window.display();
        }
        else {
            if (paused) {
                // Draw paused menu
                sf::Text pausedText;
                pausedText.setFont(font);
                pausedText.setString("Paused");
                pausedText.setCharacterSize(40);
                pausedText.setFillColor(sf::Color::White);
                pausedText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50);

                // Continue button
                Button continueButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 20, 200, 50, "Continue", font);

                // Return to Menu button
                Button returnToMenuButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 100, 200, 50, "Menu", font);

                // Restart button
                Button restartButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 180, 200, 50, "Restart", font);

                while (window.isOpen() && paused) {
                    sf::Event event;
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            window.close();
                        }
                        // Handle button clicks
                        if (event.type == sf::Event::MouseButtonPressed) {
                            sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                            if (continueButton.isClicked(mousePosition)) {
                                paused = false; // Resume the game
                            }
                            else if (returnToMenuButton.isClicked(mousePosition)) {
                                inGame = false; // Return to title screen
                                paused = false;
                            }
                            else if (restartButton.isClicked(mousePosition)) {
                                // Reset score
                                score = 0;
                                // Reset objects and enemies
                                objects.clear();
                                enemies.clear();
                                loadLevel1(objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
                                // Reset player position
                                player.sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                                // Resume the game
                                paused = false;
                            }
                        }
                    }

                    // Clear the window
                    window.clear();

                    // Draw paused menu
                    window.draw(pausedText);
                    window.draw(continueButton.shape);
                    window.draw(continueButton.text);
                    window.draw(returnToMenuButton.shape);
                    window.draw(returnToMenuButton.text);
                    window.draw(restartButton.shape);
                    window.draw(restartButton.text);

                    // Display the window
                    window.display();
                }
            }
            else {
                // Player movement
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    player.sprite.move(0, -PLAYER_SPEED);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    player.sprite.move(-PLAYER_SPEED, 0);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    player.sprite.move(0, PLAYER_SPEED);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    player.sprite.move(PLAYER_SPEED, 0);
                }

                // Update player
                player.update(WINDOW_WIDTH, WINDOW_HEIGHT);

                // Collision detection between player and objects
                for (Object& obj : objects) {
                    if (player.sprite.getGlobalBounds().intersects(obj.shape.getGlobalBounds()) && !obj.isCollected) {
                        obj.isCollected = true;
                        score++;
                        collectSound.play();
                    }
                }

                // Check if objects need to be respawned
                bool respawnNeeded = false;
                for (const Object& obj : objects) {
                    if (obj.isCollected) {
                        respawnNeeded = true;
                        break;
                    }
                }
                if (respawnNeeded) {
                    for (Object& obj : objects) {
                        if (obj.isCollected) {
                            obj.respawn(WINDOW_WIDTH, WINDOW_HEIGHT);
                        }
                    }
                }

                // Collision detection between player and enemies
                for (Enemy& enemy : enemies) {
                    if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                        // Game over
                        std::vector<Player> players = loadPlayerData();
                        Player currentPlayer;
                        currentPlayer.name = "Player"; // Set the player name to whatever you want
                        currentPlayer.score = score;
                        updateLeaderboard(players, currentPlayer);
                        savePlayerData(players);
                        displayLeaderboard(players);
                        handleGameOver(window, font, score, objects, enemies, backgroundTexture, WINDOW_WIDTH, WINDOW_HEIGHT, player);
                        inGame = false;
                    }
                }

                // Update enemies
                for (Enemy& enemy : enemies) {
                    enemy.update(WINDOW_WIDTH, WINDOW_HEIGHT);
                }

                // Clear the window
                window.clear();

                // Draw background
                window.draw(backgroundSprite);

                // Draw objects
                for (const Object& obj : objects) {
                    if (!obj.isCollected) {
                        window.draw(obj.shape);
                    }
                }

                // Draw enemies
                for (const Enemy& enemy : enemies) {
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

                // Display the window
                window.display();
            }
        }
    }

    return 0;
}
