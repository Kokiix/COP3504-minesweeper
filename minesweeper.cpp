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
    tile_texture = sf::Texture("../assets/images/tile_hidden.png", false, sf::IntRect({0, 0}, {32, 32}));
}

void GameInstance::welcome_loop() {
    // TODO
}

void GameInstance::game_loop() {
    while (window.isOpen()) {
        window.clear();
        draw_tiles();
        window.display();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }
    }
}

void GameInstance::draw_tiles() {
    for (size_t i = 0; i < n_cols; i++) {
        for (size_t j = 0; j < n_rows; j++) {
            window.draw(board[i][j].sprite);
        }
    }

}