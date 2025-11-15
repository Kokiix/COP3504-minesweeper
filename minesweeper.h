#pragma once

#include <cstdio>
#include <SFML/Graphics/Texture.hpp>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

class Tile {
public:
    bool hidden = true;
    sf::Sprite sprite;
    Tile(float x, float y, const sf::Texture &texture): sprite(texture) {
        sprite.setPosition({x * 32, y * 32});
    }
};

class GameInstance {
    size_t n_rows;
    size_t n_cols;
    sf::Texture tile_texture;
    sf::RenderWindow window;
    std::vector<std::vector<Tile>> board;


    void read_config_file();
    void load_image_assets();
    void welcome_loop();
    void game_loop();

    void draw_tiles();
public:
    GameInstance() {
        read_config_file();
        load_image_assets();

        window = sf::RenderWindow(
            sf::VideoMode(
                {static_cast<unsigned>(32 * n_cols), static_cast<unsigned>(n_rows * 32 + 100)}),
            "Minesweeper", sf::Style::Close);
        for (size_t i = 0; i < n_cols; i++) {
            board.push_back(std::vector<Tile>());
            for (size_t j = 0; j < n_rows; j++) {
                board[i].push_back(Tile(i, j, tile_texture));
            }
        }

        welcome_loop();
        game_loop();
    }
};
