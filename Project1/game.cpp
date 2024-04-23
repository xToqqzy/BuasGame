#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Define constants
const int NUM_OBJECTS = 10;
const int NUM_ENEMIES = 5;
const float OBJECT_RADIUS = 10.0f;
const float ENEMY_SPEED = 0.2f; // Increased enemy speed
const float PLAYER_SPEED = 0.4f; // Player movement speed

// Define Pac-Man grid constants
const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 21;
const int CELL_SIZE = 30;
const int WALL = 1;
const int EMPTY = 0;
const int PELLET = 2;

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
        sprite.setScale(0.2f, 0.2f); // Scale the sprite to match player size
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
void loadLevel1(std::vector<Object>& objects, std::vector<Enemy>& enemies, float windowWidth, float windowHeight) {
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

// Draw Pac-Man like background
void drawBackground(sf::RenderWindow& window) {
    // Pac-Man grid representing the background
    int grid[GRID_HEIGHT][GRID_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
        {1, 2, 1, 2, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 2, 0, 1},
        {1, 0, 1, 2, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
   
    };

    // Draw the grid
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            if (grid[i][j] == WALL) {
                cell.setFillColor(sf::Color(0, 0, 128)); // Blue walls
            }
            else if (grid[i][j] == EMPTY) {
                cell.setFillColor(sf::Color::Black); // Black empty spaces
            }
            else if (grid[i][j] == PELLET) {
                cell.setFillColor(sf::Color::Yellow); // Yellow pellets
            }
            window.draw(cell);
        }
    }
}

int main() {
    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Get the screen resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float WINDOW_WIDTH = static_cast<float>(desktop.width);
    float WINDOW_HEIGHT = static_cast<float>(desktop.height);

    // Adjust window size for Pac-Man-like maze
    WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
    WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;

    // Create the window
    sf::RenderWindow window(sf::VideoMode(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT)), "Collect Game");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Handle error if font loading fails
        return EXIT_FAILURE;
    }

    // Score counter text
    sf::Text objectCountText;
    objectCountText.setFont(font);
    objectCountText.setCharacterSize(24);
    objectCountText.setFillColor(sf::Color::White);
    objectCountText.setPosition(10, 40); // Adjust the position as needed

    // Load Level 1
    std::vector<Object> objects;
    std::vector<Enemy> enemies;
    loadLevel1(objects, enemies, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Player setup
    sf::CircleShape player(10.f); // Player circle shape
    player.setFillColor(sf::Color::Green);
    player.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Game loop
    bool paused = false; // Flag to indicate if the game is paused
    bool gameOver = false; // Flag to indicate if the game is over
    int score = 0; // Score counter
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameOver && !paused) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        paused = true; // Pause the game
                    }
                }
            }
            else if (gameOver) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        // Play again
                        gameOver = false;
                        paused = false;
                        score = 0;
                        objects.clear();
                        enemies.clear();
                        loadLevel1(objects, enemies, WINDOW_WIDTH, WINDOW_HEIGHT);
                    }
                    else if (event.key.code == sf::Keyboard::Q) {
                        // Quit game
                        window.close();
                    }
                }
            }
            else if (paused) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        paused = false; // Resume the game
                    }
                    else if (event.key.code == sf::Keyboard::Q) {
                        window.close(); // Quit game
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        // Restart game
                        paused = false;
                        score = 0;
                        objects.clear();
                        enemies.clear();
                        loadLevel1(objects, enemies, WINDOW_WIDTH, WINDOW_HEIGHT);
                    }
                }
            }
        }

        if (!paused && !gameOver) {
            // Player movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.getPosition().y > 0) {
                player.move(0, -PLAYER_SPEED);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.getPosition().x > 0) {
                player.move(-PLAYER_SPEED, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.getPosition().y < WINDOW_HEIGHT - player.getRadius() * 2) {
                player.move(0, PLAYER_SPEED);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && player.getPosition().x < WINDOW_WIDTH - player.getRadius() * 2) {
                player.move(PLAYER_SPEED, 0);
            }

            // Collision detection between player and objects
            for (Object& obj : objects) {
                if (player.getGlobalBounds().intersects(obj.shape.getGlobalBounds()) && !obj.isCollected) {
                    obj.isCollected = true;
                    score++;
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
            for (const Enemy& enemy : enemies) {
                if (player.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                    // Game over
                    gameOver = true;

                }
            }
            \

            // Update enemies
            for (Enemy& enemy : enemies) {
                enemy.update(WINDOW_WIDTH, WINDOW_HEIGHT);
            }

            // Clear the window
            window.clear();

            // Draw Pac-Man-like background
            drawBackground(window);

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
            window.draw(player);

            // Update and draw score counter
            objectCountText.setString("Score: " + std::to_string(score));
            window.draw(objectCountText);
        }

        // If game over, display game over message and buttons
        if (gameOver) {
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over!");
            gameOverText.setCharacterSize(36);
            gameOverText.setFillColor(sf::Color::White);
            gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - gameOverText.getGlobalBounds().height);

            sf::Text playAgainText;
            playAgainText.setFont(font);
            playAgainText.setString("Play Again? (press R)");
            playAgainText.setCharacterSize(24);
            playAgainText.setFillColor(sf::Color::White);
            playAgainText.setPosition(WINDOW_WIDTH / 2 - playAgainText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 50);

            sf::Text quitGameText;
            quitGameText.setFont(font);
            quitGameText.setString("Quit Game (press Q)");
            quitGameText.setCharacterSize(24);
            quitGameText.setFillColor(sf::Color::White);
            quitGameText.setPosition(WINDOW_WIDTH / 2 - quitGameText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 100);

            window.draw(gameOverText);
            window.draw(playAgainText);
            window.draw(quitGameText);
        }

        // If paused, display pause menu with options
        if (paused) {
            sf::Text pauseText;
            pauseText.setFont(font);
            pauseText.setString("Game Paused");
            pauseText.setCharacterSize(36);
            pauseText.setFillColor(sf::Color::White);
            pauseText.setPosition(WINDOW_WIDTH / 2 - pauseText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - pauseText.getGlobalBounds().height);

            sf::Text continueText;
            continueText.setFont(font);
            continueText.setString("ESC (Continue playing)");
            continueText.setCharacterSize(24);
            continueText.setFillColor(sf::Color::White);
            continueText.setPosition(WINDOW_WIDTH / 2 - continueText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 50);

            sf::Text restartText;
            restartText.setFont(font);
            restartText.setString("R (Restart)");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition(WINDOW_WIDTH / 2 - restartText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 100);

            sf::Text quitText;
            quitText.setFont(font);
            quitText.setString("Q (Quit)");
            quitText.setCharacterSize(24);
            quitText.setFillColor(sf::Color::White);
            quitText.setPosition(WINDOW_WIDTH / 2 - quitText.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 150);

            window.draw(pauseText);
            window.draw(continueText);
            window.draw(restartText);
            window.draw(quitText);
        }

        // Display the window
        window.display();
    }

    return 0;
}
