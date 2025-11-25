#pragma once

#include <cstdio>
#include <SFML/Graphics/Texture.hpp>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

class Tile {
public:
    bool hidden = true;
    bool is_mine = false;
    bool flagged = false;
    bool draw_overlay = false;
    size_t n_mines_near = 0;
    sf::Sprite tile_sprite;
    sf::Sprite overlay_sprite;

    Tile(const sf::Texture &texture): tile_sprite(texture), overlay_sprite(texture) {}
};

class GameInstance {
    size_t n_rows;
    size_t n_cols;
    size_t n_mines;
    sf::Texture hidden_tile_texture;
    sf::Texture revealed_tile_texture;
    sf::Texture mine_texture;
    sf::Texture flag_texture;
    sf::Texture number_textures[9];
    sf::RenderWindow window;
    std::vector<std::vector<Tile>> board;


    void read_config_file();
    void load_image_assets();
    void welcome_loop();
    void board_setup();
    void game_loop();

    void redraw_screen();
    void handle_click(const sf::Event::MouseButtonPressed* event);
    void clear_tile(float x, float y);
    void toggle_flag(float x, float y);
    void operateOnNeighbors(float x, float y, std::function<void (float x, float y)> callback);
public:
    GameInstance() {
        read_config_file();
        load_image_assets();

        window = sf::RenderWindow(
            sf::VideoMode(
                {static_cast<unsigned>(32 * n_cols), static_cast<unsigned>(n_rows * 32 + 100)}),
            "Minesweeper", sf::Style::Close);

        welcome_loop();
        board_setup();
        game_loop();
    }
    ~GameInstance();
};
