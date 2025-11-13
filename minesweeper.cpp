#include "minesweeper.h"

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Window.hpp"

void GameInstance::read_config_file() {
    // TODO
    // placeholder:
    n_rows = 10;
    n_cols = 10;
}

void GameInstance::welcome_loop() {
    // TODO
}

void GameInstance::game_loop() {
    sf::Window window(sf::VideoMode({static_cast<unsigned>(32 * n_rows + 100), static_cast<unsigned>(n_cols * 32)}), "Minesweeper");
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }
    }
}