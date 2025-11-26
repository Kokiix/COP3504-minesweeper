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
    size_t n_rows;
    size_t n_cols;
    size_t n_mines;
    size_t tiles_revealed;
    sf::Texture number_textures[9];
    sf::Texture stopwatch_textures[10];
    sf::RenderWindow window;
    sf::RenderWindow* welcome_window;
    sf::Font font;
    std::map<std::string, sf::Texture*> textures;
    std::map<std::string, sf::Sprite*> UI_elements;
    std::vector<std::vector<Tile>> board;
    std::string player_name = "";
    bool debug_mode = false;
    bool game_over = false;
    bool paused = false;
    size_t time = 0;
    std::mt19937 rng;


    void read_config_file();
    void load_assets();
    void init_ui_sprites();
    void draw_welcome(std::string name);
    void welcome_loop();
    void handle_typing(const sf::Event::KeyPressed* e, sf::Text& entry);
    void board_setup();
    void game_loop();
    void redraw_screen();
    void display_time();
    void handle_ui_click(float x);
    void handle_click(const sf::Event::MouseButtonPressed* event);
    void clear_tile(float x, float y);
    void toggle_flag(float x, float y);
    void toggle_debug();
    void operateOnNeighbors(float x, float y, std::function<void (float x, float y)> callback);
    void write_to_leaderboard();
    void leaderboard_loop();
public:
    GameInstance() {
        read_config_file();
        load_assets();
        init_ui_sprites();

        window = sf::RenderWindow(
        sf::VideoMode(
            {static_cast<unsigned>(32 * n_cols), static_cast<unsigned>(n_rows * 32 + 100)}),
        "Minesweeper", sf::Style::Close);
        board_setup();
        redraw_screen();

        welcome_window = new sf::RenderWindow(
            sf::VideoMode({static_cast<unsigned>(16 * 32 + 100), static_cast<unsigned>(16 * 32)}),
            "Minesweeper",
            sf::Style::Close);
        welcome_loop();
        delete welcome_window;

        game_loop();
    }
    ~GameInstance();
};
