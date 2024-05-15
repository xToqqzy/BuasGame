#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/Graphics.hpp>
#include <vector>

class TileMap {
public:
    TileMap(const std::string& tilesetFile, int tileSize, int mapWidth, int mapHeight);
    ~TileMap();
    void update(float deltaTime);
    void render(sf::RenderTarget& target);

private:
    struct TileData {
        int tileIndex;
        sf::Vector2f position;
    };

    sf::Texture tilesetTexture;
    int tileSize;
    std::vector<TileData> tileData;
};

#endif // TILEMAP_H