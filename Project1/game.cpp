#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

// Define constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PLAYER_SPEED = 600.0f; // Increased player speed
const float ITEM_SPEED = 200.0f; // Decreased item speed for smoother gameplay
const int NUM_BOMBS = 2; // Increased number of bombs
const float FRUIT_SPAWN_INTERVAL = 0.4f; // Adjusted fruit spawn interval
const float BOMB_SPAWN_INTERVAL = 1.5f; // Adjusted bomb spawn interval
const int SCORE_PER_FRUIT = 10; // Adjusted score per fruit

// Player class
class Player {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Player() {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100);
    }

    void update(float deltaTime) {
        shape.move(velocity * deltaTime);
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

// Add more fruit classes...

// Bomb class
class Bomb : public Item {
public:
    Bomb(sf::Texture& texture, float posX, float posY)
        : Item(texture, posX, posY, -50) {} // Bomb deducts 50 points
};

int main() {
    // Create game window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Fruit Picker");

    // Create player
    Player player;

    // Load textures for fruits and bombs
    sf::Texture appleTexture;
    if (!appleTexture.loadFromFile("assets/banana.png")) {
        std::cerr << "Failed to load apple.png" << std::endl;
        return 1;
    }

    sf::Texture watermelonTexture;
    if (!watermelonTexture.loadFromFile("assets/watermelon.png")) {
        std::cerr << "Failed to load watermelon.png" << std::endl;
        return 1;
    }

    sf::Texture bananaTexture;
    if (!bananaTexture.loadFromFile("assets/banana.png")) {
        std::cerr << "Failed to load banana.png" << std::endl;
        return 1;
    }

    sf::Texture bombTexture;
    if (!bombTexture.loadFromFile("assets/bomb.png")) {
        std::cerr << "Failed to load bomb.png" << std::endl;
        return 1;
    }

    // Create vectors to hold items
    std::vector<Item*> items;
    std::vector<Bomb> bombs;

    int score = 0;

    // Game loop
    sf::Clock clock;
    float timeSinceLastFruitSpawn = 0.0f;
    float timeSinceLastBombSpawn = 0.0f;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        float dtSeconds = deltaTime.asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

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
            if (item->sprite.getGlobalBounds().intersects(player.shape.getGlobalBounds()) && !item->collected) {
                item->collected = true;
                score += item->points;
            }
        }
        for (auto& bomb : bombs) {
            if (bomb.sprite.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
                // Handle bomb collision (e.g., player dies and restarts)
                window.close();
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

        // Clear window
        window.clear();

        // Draw player
        window.draw(player.shape);

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
