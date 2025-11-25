#include "minesweeper.h"

#include <iostream>
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
    n_rows = 16;
    n_cols = 25;
    n_mines = 60;
}

void GameInstance::load_image_assets() {
    hidden_tile_texture = sf::Texture("../assets/images/tile_hidden.png", false, sf::IntRect({0, 0}, {32, 32}));
    revealed_tile_texture = sf::Texture("../assets/images/tile_revealed.png", false, sf::IntRect({0, 0}, {32, 32}));
    mine_texture = sf::Texture("../assets/images/mine.png", false, sf::IntRect({0, 0}, {32, 32}));
    flag_texture = sf::Texture("../assets/images/flag.png", false, sf::IntRect({0, 0}, {32, 32}));
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
    std::vector<Tile> shuffle_list;
    shuffle_list.reserve(n_rows * n_cols);
    for (size_t i = 0; i < n_rows * n_cols; i++) {
        Tile t(hidden_tile_texture);
        if (i < n_mines) {t.is_mine = true;}
        shuffle_list.emplace_back(t);
    }

    // TODO: gen mines after first click to prevent instadeath
    std::random_device rd;
    std::mt19937 rng(rd());
    std::ranges::shuffle(shuffle_list, rng);

    for (size_t col = 0; col < n_cols; col++) {
        board.push_back(std::vector<Tile>());
        for (size_t row = 0; row < n_rows; row++) {
            board[col].push_back(std::move(shuffle_list[col * n_rows + row]));
            board[col][row].tile_sprite.setPosition({col * 32.0f, row * 32.0f});
            board[col][row].overlay_sprite.setPosition({col * 32.0f, row * 32.0f});
        }
    }

    for (size_t col = 0; col < n_cols; col++) {
        for (size_t row = 0; row < n_rows; row++) {
            if (board[col][row].is_mine) {
                board[col][row].is_mine = true;
                // board[col][row].tile_sprite.setTexture(mine_texture);

                operateOnNeighbors(col, row, [this](float x, float y) {board[x][y].n_mines_near++;});
            }
        }
    }
}

void GameInstance::game_loop() {
    redraw_screen();
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (auto click_event = event->getIf<sf::Event::MouseButtonPressed>()) {
                handle_click(click_event);
                redraw_screen();
            }
            else if (event->is<sf::Event::Closed>()) window.close();
        }
    }
}

void GameInstance::handle_click(const sf::Event::MouseButtonPressed* event) {
    sf::Vector2i clickPos = event->position;
    if (event->button == sf::Mouse::Button::Left) {
        clear_tile(clickPos.x / 32, clickPos.y / 32);
    } else if (event->button == sf::Mouse::Button::Right) {
        toggle_flag(clickPos.x / 32, clickPos.y / 32);
    }
}

void GameInstance::clear_tile(float x, float y) {
    if (board[x][y].hidden && !board[x][y].is_mine && !board[x][y].flagged) {
        board[x][y].tile_sprite.setTexture(revealed_tile_texture);
        board[x][y].hidden = false;
        if (board[x][y].n_mines_near > 0) {
            board[x][y].overlay_sprite.setTexture(number_textures[board[x][y].n_mines_near]);
            board[x][y].draw_overlay = true;
        } else {
            operateOnNeighbors(x, y, [this](float x, float y) {this->clear_tile(x, y);});
        }
    }
}

void GameInstance::toggle_flag(float x, float y) {
    if (board[x][y].hidden) {
        if (board[x][y].flagged) {
            board[x][y].draw_overlay = false;
            board[x][y].flagged = false;
        } else {
            board[x][y].draw_overlay = true;
            board[x][y].overlay_sprite.setTexture(flag_texture);
            board[x][y].flagged = true;
        }
    }
}

void GameInstance::redraw_screen() {
    window.clear(sf::Color::White);
    for (size_t i = 0; i < n_cols; i++) {
        for (size_t j = 0; j < n_rows; j++) {
            window.draw(board[i][j].tile_sprite);
            if (board[i][j].draw_overlay) window.draw(board[i][j].overlay_sprite);
        }
    }
    // // TODO: draw dashboard
    window.display();
}

void GameInstance::operateOnNeighbors(float x, float y, std::function<void (float x, float y)> callback) {
    int col_end = x == n_cols - 1 ? 1 : 2;
    int row_end = y == n_rows - 1 ? 1 : 2;
    for (int col_offset = x == 0 ? 0 : -1; col_offset < col_end; col_offset++) {
        for (int row_offset = y == 0 ? 0 : -1; row_offset < row_end; row_offset++) {
            if (col_offset == 0 && row_offset == 0) continue;
            callback(x + col_offset, y + row_offset);
        }
    }
}

GameInstance::~GameInstance() {
    // delete board;
}