#pragma once

#include <cstdio>
#include <map>
#include <random>
#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"

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
    // Core process (taking place in constructor below)
    size_t n_rows;
    size_t n_cols;
    size_t n_mines;
    void read_config_file();
    sf::Texture number_textures[9];
    sf::Texture stopwatch_textures[10];
    sf::Font font;
    std::map<std::string, sf::Texture*> textures;
    void load_assets();
    std::map<std::string, sf::Sprite*> UI_elements;
    void init_ui_sprites();

    // game window is created
    sf::RenderWindow window;
    std::vector<std::vector<Tile>> board;
    size_t tiles_revealed;
    void board_setup();
    void draw_screen();

    // welcome window is created
    sf::RenderWindow* welcome_window;
    std::string player_name = "";
    void draw_welcome(std::string name);
    void welcome_loop();

    bool debug_mode = false;
    bool game_over = false;
    bool paused = false;
    size_t time = 0;
    std::mt19937 rng;
    void game_loop();
    void handle_click(const sf::Event::MouseButtonPressed* event);
    void handle_ui_click(float x);

    void clear_tile(float x, float y);
    void toggle_flag(float x, float y);
    void toggle_debug();
    void operateOnNeighbors(float x, float y, std::function<void (float x, float y)> callback);

    void display_time();
    void write_to_leaderboard();
    void leaderboard_loop();
public:
    ~GameInstance();
    GameInstance() {
        read_config_file();
        load_assets();
        init_ui_sprites();

        window = sf::RenderWindow(
        sf::VideoMode(
            {static_cast<unsigned>(32 * n_cols), static_cast<unsigned>(n_rows * 32 + 100)}),
        "Minesweeper", sf::Style::Close);
        board_setup();
        draw_screen();

        welcome_window = new sf::RenderWindow(
            sf::VideoMode({static_cast<unsigned>(16 * 32 + 100), static_cast<unsigned>(16 * 32)}),
            "Minesweeper",
            sf::Style::Close);
        welcome_loop();
        delete welcome_window;

        game_loop();
    }
};
