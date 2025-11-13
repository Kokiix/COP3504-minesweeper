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
    n_cols = 10;
}

void GameInstance::load_image_assets() {
    tile_texture = sf::Texture("../assets/images/tile_hidden.png", false, sf::IntRect({10, 10}, {50, 50}));
}

void GameInstance::welcome_loop() {
    // TODO
}

void GameInstance::game_loop() {
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(32 * n_rows + 100), static_cast<unsigned>(n_cols * 32)}), "Minesweeper", sf::Style::Close);
    sf::Sprite test(tile_texture);
    test.setPosition(sf::Vector2f(50, 50));
    while (window.isOpen()) {
        window.clear();
        window.draw(test);
        window.display();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }
    }
}