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

    void handle_click(const sf::Texture &revealed) {
        sprite.setTexture(revealed);
    }
};

class GameInstance {
    size_t n_rows;
    size_t n_cols;
    sf::Texture hidden_tile_texture;
    sf::Texture revealed_tile_texture;
    sf::RenderWindow window;
    std::vector<std::vector<Tile>> board;


    void read_config_file();
    void load_image_assets();
    void welcome_loop();
    void game_loop();

    void redraw_screen();
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
                board[i].push_back(Tile(i, j, hidden_tile_texture));
            }
        }

        welcome_loop();
        game_loop();
    }
};
