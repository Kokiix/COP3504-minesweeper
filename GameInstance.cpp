#include "GameInstance.h"

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
    n_cols = 30;
    n_mines = 60;
}

void GameInstance::load_image_assets() {
    std::vector<std::string> texture_names {"tile_hidden", "tile_revealed", "mine", "flag"};
    for (std::string s : texture_names) {
        textures.insert(
            std::pair<std::string, sf::Texture*>(
                s,
                new sf::Texture(
                    "../assets/images/" + s + ".png", false,
                    sf::IntRect({0, 0}, {32, 32}))));
    }

    std::vector<std::string> ui_names {"debug", "face_happy"};
    for (std::string s : ui_names) {
        textures.insert(
            std::pair<std::string, sf::Texture*>(
                s,
                new sf::Texture(
                    "../assets/images/" + s + ".png", false,
                    sf::IntRect({0, 0}, {64, 64}))));
    }
    for (size_t i = 1; i < 9; i++) {
        std::ostringstream path;
        path << "../assets/images/number_" << i << ".png";
        number_textures[i] = sf::Texture(path.str(), false, sf::IntRect({0, 0}, {32, 32}));
    }


    std::random_device rd;
    rng = std::mt19937(rd());
}

void GameInstance::init_ui_sprites() {
    sf::Sprite s = sf::Sprite(*textures["face_happy"]);
    s.setPosition({(n_cols / 2 - 1) * 32.0f, (n_rows + 0.5f) * 32.0f});
    UI_elements.push_back(s);

    s.setTexture(*textures["debug"]);
    s.setPosition({(n_cols / 2 - 3) * 32.0f, (n_rows + 0.5f) * 32.0f});
    UI_elements.push_back(s);
}

void GameInstance::welcome_loop() {
    // TODO
}

void GameInstance::board_setup() {
    std::vector<Tile> shuffle_list;
    shuffle_list.reserve(n_rows * n_cols);
    for (size_t i = 0; i < n_rows * n_cols; i++) {
        Tile t(*textures["tile_hidden"]);
        if (i < n_mines) {t.is_mine = true;}
        shuffle_list.emplace_back(t);
    }

    // TODO: gen mines after first click to prevent instadeath
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
            Tile& t = board[col][row];
            if (t.is_mine) {
                t.is_mine = true;
                if (debug_mode) {
                    t.overlay_sprite.setTexture(*textures["mine"]);
                    t.draw_overlay = true;
                }
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
    float x = clickPos.x / 32;
    float y = clickPos.y / 32;
    if (event->button == sf::Mouse::Button::Left) {
        if (y >= n_rows) {
            if (x > n_cols / 2 - 2 && x < n_cols / 2 + 1) {
                board.clear();
                board_setup();
                redraw_screen();
            } else if (x > n_cols / 2 - 4 && x < n_cols / 2 - 1) {
                toggle_debug();
            }
        } else {
            clear_tile(x, y);
        }
    } else if (event->button == sf::Mouse::Button::Right) {
        toggle_flag(x, y);
    }
}

void GameInstance::clear_tile(float x, float y) {
    if (board[x][y].hidden && !board[x][y].is_mine && !board[x][y].flagged) {
        board[x][y].tile_sprite.setTexture(*textures["tile_revealed"]);
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
            board[x][y].overlay_sprite.setTexture(*textures["flag"]);
            board[x][y].flagged = true;
        }
    }
}

void GameInstance::toggle_debug() {
    debug_mode = !debug_mode;
    for (size_t i = 0; i < n_cols; i++) {
        for (size_t j = 0; j < n_rows; j++) {
            Tile& t = board[i][j];
            if (t.is_mine) {
                t.overlay_sprite.setTexture(*textures["mine"]);
                t.draw_overlay = debug_mode;
            }
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
    for (sf::Sprite s : UI_elements) {
        window.draw(s);
    }
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
    for (auto pair : textures) {
        delete pair.second;
    }
}