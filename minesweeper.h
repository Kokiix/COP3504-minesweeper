#pragma once

#include <cstdio>
#include <SFML/Graphics/Texture.hpp>

class GameInstance {
    size_t n_rows;
    size_t n_cols;
    sf::Texture tile_texture;

    void read_config_file();
    void load_image_assets();
    void welcome_loop();
    void game_loop();
public:
    GameInstance() {
        read_config_file();
        load_image_assets();

        welcome_loop();
        game_loop();
    }
};
