#include "minesweeper.h"

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
}

void GameInstance::load_image_assets() {
    hidden_tile_texture = sf::Texture("../assets/images/tile_hidden.png", false, sf::IntRect({0, 0}, {32, 32}));
    revealed_tile_texture = sf::Texture("../assets/images/tile_revealed.png", false, sf::IntRect({0, 0}, {32, 32}));
}

void GameInstance::welcome_loop() {
    // TODO
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
            window.draw(board[i][j].sprite);
        }
    }
    // TODO: draw dashboard
    window.display();
}