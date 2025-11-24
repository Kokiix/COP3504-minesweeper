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
    n_cols = 16;
    n_mines = 40;
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

    for (size_t col = 0; col < n_rows; col++) {
        for (size_t row = 0; row < n_rows; row++) {
            if (board[col][row].is_mine) {
                board[col][row].is_mine = true;
                board[col][row].tile_sprite.setTexture(mine_texture);

                if (col != 0) {
                    board[col - 1][row].n_mines_near++;
                    if (row != 0) {board[col - 1][row - 1].n_mines_near++;}
                    if (row != n_rows - 1) {board[col - 1][row + 1].n_mines_near++;}
                }
                if (col != n_cols - 1) {
                    board[col + 1][row].n_mines_near++;
                    if (row != 0) {board[col + 1][row - 1].n_mines_near++;}
                    if (row != n_rows - 1) {board[col + 1][row + 1].n_mines_near++;}
                }
                if (row != 0) {board[col][row - 1].n_mines_near++;}
                if (row != n_rows - 1) {board[col][row + 1].n_mines_near++;}
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
            if (x != 0) {clear_tile(x - 1, y);}
            if (x != n_cols - 1) {clear_tile(x + 1, y);}
            if (y != 0) {clear_tile(x, y - 1);}
            if (y != n_rows - 1) {clear_tile(x, y + 1);}
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

GameInstance::~GameInstance() {
    // delete board;
}