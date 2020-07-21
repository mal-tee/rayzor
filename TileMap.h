#ifndef RAYZOR_TILEMAP_H
#define RAYZOR_TILEMAP_H

#include <iostream>
#include <SFML/Graphics.hpp>

//see https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
class TileMap : public sf::Drawable, public sf::Transformable {
public:
    void load(const std::string &file, int tilesize, const int *tiles, unsigned int width, unsigned int height) {
        if (!m_tileset.loadFromFile(file)) {
            std::cout << "Error loading tilemap" << std::endl;
            return;
        }
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);
        for (unsigned int x = 0; x < width; x++) {
            for (unsigned int y = 0; y < height; y++) {
                int type = tiles[x + y * width];

                unsigned int u = type % (m_tileset.getSize().x / tilesize);
                unsigned int v = type / (m_tileset.getSize().x / tilesize);

                sf::Vertex *tmp = &m_vertices[(x + y * width) * 4];
                tmp[0].position = sf::Vector2f((float) (x * tilesize), (float) (y * tilesize));
                tmp[1].position = sf::Vector2f((float) ((x + 1) * tilesize), (float) (y * tilesize));
                tmp[2].position = sf::Vector2f((float) ((x + 1) * tilesize), (float) ((y + 1) * tilesize));
                tmp[3].position = sf::Vector2f((float) (x * tilesize), (float) ((y + 1) * tilesize));

                tmp[0].texCoords = sf::Vector2f((float) (u * tilesize), (float) (v * tilesize));
                tmp[1].texCoords = sf::Vector2f((float) ((u + 1) * tilesize), (float) (v * tilesize));
                tmp[2].texCoords = sf::Vector2f((float) ((u + 1) * tilesize), (float) ((v + 1) * tilesize));
                tmp[3].texCoords = sf::Vector2f((float) (u * tilesize), (float) ((v + 1) * tilesize));
            }
        }
    }

private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &m_tileset;
        target.draw(m_vertices, states);
    }
};

#endif //RAYZOR_TILEMAP_H
