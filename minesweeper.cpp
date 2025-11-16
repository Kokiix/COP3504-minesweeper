#include "minesweeper.h"

#include <random>
#include <sstream>

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

void GameInstance::read_config_file() {
    // TODO
    // placeholder:
    n_rows = 10;
    n_cols = 20;
    n_mines = 10;
}

void GameInstance::load_image_assets() {
    hidden_tile_texture = sf::Texture("../assets/images/tile_hidden.png", false, sf::IntRect({0, 0}, {32, 32}));
    revealed_tile_texture = sf::Texture("../assets/images/tile_revealed.png", false, sf::IntRect({0, 0}, {32, 32}));
    for (size_t i = 1; i < 9; i++) {
        std::ostringstream path;
        path << "../assets/images/number_" << i << ".png";
        number_textures[i] = sf::Texture(path.str(), false, sf::IntRect({0, 0}, {32, 32}));
    }
}

void GameInstance::welcome_loop() {
    // TODO
}

void GameInstance::board_setup() {
    for (size_t i = 0; i < n_cols; i++) {
        board.push_back(std::vector<Tile>());
        for (size_t j = 0; j < n_rows; j++) {
            board[i].push_back(Tile(i, j, hidden_tile_texture));
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> random_col(0, n_cols - 1);
    std::uniform_int_distribution<int> random_row(0, n_rows - 1);
    for (size_t i = 0; i < n_mines; i++) {
        board[random_col(rng)][random_row(rng)].is_mine = true;
    }
}

void GameInstance::game_loop() {
    redraw_screen();
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i clickPos = sf::Mouse::getPosition(window);
                board[clickPos.x / 32][clickPos.y / 32].handle_click(revealed_tile_texture);
                redraw_screen();
            }
            else if (event->is<sf::Event::Closed>()) window.close();
        }
    }
}

void GameInstance::redraw_screen() {
    window.clear(sf::Color::White);
    for (size_t i = 0; i < n_cols; i++) {
        for (size_t j = 0; j < n_rows; j++) {
            window.draw(board[i][j].tile_sprite);
        }
    }
    // TODO: draw dashboard
    window.display();
}