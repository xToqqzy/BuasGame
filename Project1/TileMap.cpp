#include "TileMap.h"
#include <fstream>
#include <sstream>
#include <iostream>

TileMap::TileMap(const std::string& tilesetFile, int tileSize, int mapWidth, int mapHeight)
    : tileSize(tileSize) {
    // Load tileset texture
    if (!tilesetTexture.loadFromFile(tilesetFile)) {
        std::cerr << "Failed to load tileset texture: " << tilesetFile << std::endl;
        exit(1);
    }

    // Read map data from a file
    std::ifstream mapFile("map.txt");
    if (!mapFile.is_open()) {
        std::cerr << "Failed to open map file: map.txt" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(mapFile, line)) {
        std::istringstream iss(line);
        int tileIndex;
        while (iss >> tileIndex) {
            tileData.push_back({ tileIndex, sf::Vector2f(tileSize * tileData.size(), tileSize) });
        }
    }

    mapFile.close();
}

TileMap::~TileMap() {
}

void TileMap::update(float deltaTime) {
    // No updates needed for a static tilemap
}

void TileMap::render(sf::RenderTarget& target) {
    for (const auto& tileData : tileData) {
        sf::Sprite tileSprite;
        tileSprite.setTexture(tilesetTexture);
        tileSprite.setTextureRect(sf::IntRect(tileData.tileIndex * tileSize, 0, tileSize, tileSize));
        tileSprite.setPosition(tileData.position);
        target.draw(tileSprite);
    }
}