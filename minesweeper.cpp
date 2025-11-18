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
        int col = random_col(rng);
        int row = random_row(rng);
        board[col][row].is_mine = true;
        board[col][row].tile_sprite.setTexture(revealed_tile_texture);
        // TODO: create f that takes callback for each direction
        if (col != 0) {board[col - 1][row].n_mines_near++;}
        if (col != n_cols - 1) {board[col + 1][row].n_mines_near++;}
        if (row != 0) {board[col][row - 1].n_mines_near++;}
        if (row != n_rows - 1) {board[col][row + 1].n_mines_near++;}
    }
}

void GameInstance::game_loop() {
    redraw_screen();
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i clickPos = sf::Mouse::getPosition(window);
                float x = clickPos.x / 32;
                float y = clickPos.y / 32;
                Tile clicked = board[x][y];
                board[x][y].tile_sprite.setTexture(revealed_tile_texture);
                if (clicked.n_mines_near > 0) {
                    board[x][y].number_sprite = new sf::Sprite(number_textures[clicked.n_mines_near]);
                    board[x][y].number_sprite->setPosition({x * 32, y * 32});
                }
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
            if (board[i][j].number_sprite) window.draw(*board[i][j].number_sprite);
        }
    }
    // TODO: draw dashboard
    window.display();
}