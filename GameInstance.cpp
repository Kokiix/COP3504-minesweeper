#include "GameInstance.h"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"

void GameInstance::read_config_file() {
    std::ifstream config_file("../assets/config.cfg");
    config_file >> n_cols >> n_rows >> n_mines;
    tiles_revealed = 0;
    config_file.close();
}

void GameInstance::load_assets() {
    if (!font.openFromFile("../assets/font.ttf")) {
        throw std::runtime_error("Could not open font file");
    };
    std::vector<std::string> texture_names {"tile_hidden", "tile_revealed", "mine", "flag"};
    for (std::string s : texture_names) {
        textures.insert(
            std::pair<std::string, sf::Texture*>(
                s,
                new sf::Texture(
                    "../assets/images/" + s + ".png", false,
                    sf::IntRect({0, 0}, {32, 32}))));
    }

    std::vector<std::string> ui_names {"debug", "face_happy", "face_lose", "face_win", "pause",
        "play", "leaderboard"};
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

    for (int i = 0; i < 10; i++) {
        stopwatch_textures[i] = sf::Texture("../assets/images/digits.png", false, sf::IntRect({i * 21, 0}, {21, 32}));
    }


    std::random_device rd;
    rng = std::mt19937(rd());
}

void GameInstance::init_ui_sprites() {
    std::function create_button = [this](std::string name, float center_offset) {
        sf::Sprite* s = new sf::Sprite(*textures[name]);
        s->setPosition({(n_cols / 2 + center_offset) * 32.0f, (n_rows + 0.5f) * 32.0f});
        this->UI_elements.insert(std::pair<std::string, sf::Sprite*>(name, s));
    };

    create_button("debug", -4);
    create_button("face_happy", -2);
    create_button("pause", 0);
    create_button("leaderboard", 2);

    std::function create_stopwatch = [this](float x, std::string name) {
        sf::Sprite* s = new sf::Sprite(stopwatch_textures[0]);
        s->setPosition({x, (n_rows + 1) * 32.0f});
        this->UI_elements.insert(std::pair<std::string, sf::Sprite*>(name, s));
    };
    create_stopwatch(32, "hundreds");
    create_stopwatch(53, "tens");
    create_stopwatch(74, "ones");
}

void GameInstance::draw_welcome(std::string name) {
    welcome_window->clear(sf::Color::White);
    sf::Text title(font, "WELCOME TO MINESWEEPER!", 24);
    title.setStyle(sf::Text::Bold);
    title.setStyle(sf::Text::Underlined);
    title.setFillColor(sf::Color::Black);
    title.setPosition({welcome_window->getSize().x / 2.0f - title.getLocalBounds().size.x / 2, welcome_window->getSize().y / 5.0f});
    welcome_window->draw(title);

    sf::Text prompt(font, "Enter Your Name:", 20);
    prompt.setStyle(sf::Text::Bold);
    prompt.setFillColor(sf::Color::Black);
    prompt.setPosition({welcome_window->getSize().x / 2.0f - prompt.getLocalBounds().size.x / 2, welcome_window->getSize().y / 3.0f});
    welcome_window->draw(prompt);

    sf::Text name_entry(font, name + "|", 18);
    name_entry.setStyle(sf::Text::Bold);
    name_entry.setFillColor(sf::Color::Black);
    name_entry.setPosition({welcome_window->getSize().x / 2.0f - name_entry.getLocalBounds().size.x / 2,
        welcome_window->getSize().y / 3.0f + prompt.getLocalBounds().size.y + 15.0f});
    welcome_window->draw(name_entry);

    welcome_window->display();
}

void GameInstance::welcome_loop() {
    draw_welcome("");
    std::string name_entry = "";
    while (welcome_window->isOpen()) {
        while (const std::optional event = welcome_window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                welcome_window->close();
                break;
            }

            if (auto e = event->getIf<sf::Event::TextEntered>()) {
                if (e->unicode == 10 && name_entry != "") {
                    player_name = name_entry;
                    welcome_window->close();
                    break;
                }

                if (e->unicode == 8 && name_entry != "") {
                    name_entry.pop_back();
                    draw_welcome(name_entry);
                } else if (std::isalpha(e->unicode) && name_entry.length() < 10) {
                    if (name_entry == "") name_entry.push_back(std::toupper(e->unicode));
                    else name_entry.push_back(std::tolower(e->unicode));
                    draw_welcome(name_entry);
                }
            }
        }
    }
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
    auto last_time = std::chrono::steady_clock::now();
    auto curr_time = std::chrono::steady_clock::now();
    int elapsed;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (auto click_event = event->getIf<sf::Event::MouseButtonPressed>()) {
                handle_click(click_event);
                redraw_screen();
            }
            else if (event->is<sf::Event::Closed>()) window.close();
        }

        if (!game_over && !paused && tiles_revealed > 0) {
            curr_time = std::chrono::steady_clock::now();
            elapsed = std::chrono::duration<double>(curr_time - last_time).count();
            if (elapsed >= 1 && time < 999) {
                last_time = curr_time;
                time++;
                display_time();
            }
        }
    }
}

void GameInstance::handle_click(const sf::Event::MouseButtonPressed* event) {
    sf::Vector2i clickPos = event->position;
    float x = clickPos.x / 32;
    float y = clickPos.y / 32;
    if (event->button == sf::Mouse::Button::Left) {
        if (y >= n_rows) {
            handle_ui_click(x);
        } else if (!game_over && !paused) {
            if (board[x][y].is_mine && board[x][y].flagged == false) {
                game_over = true;
                UI_elements["face_happy"]->setTexture(*textures["face_lose"]);
            } else {
                clear_tile(x, y);
            }
        }
    } else if (event->button == sf::Mouse::Button::Right) {
        toggle_flag(x, y);
    }
}

void GameInstance::handle_ui_click(float x) {
    if (x > n_cols / 2 + 1 && x < n_cols / 2 + 4) {
        leaderboard_loop();
    } else if (x > n_cols / 2 - 1 && x < n_cols / 2 + 2) {
        if (paused) {
            paused = false;
            UI_elements["pause"]->setTexture(*textures["pause"]);
        } else {
            paused = true;
            UI_elements["pause"]->setTexture(*textures["play"]);
        }
    } else if (x > n_cols / 2 - 3 && x < n_cols / 2) {
        game_over = false;
        time = 0;
        tiles_revealed = 0;
        paused = false;
        UI_elements["pause"]->setTexture(*textures["pause"]);
        UI_elements["face_happy"]->setTexture(*textures["face_happy"]);
        board.clear();
        board_setup();
        display_time();
    } else if (x > n_cols / 2 - 5 && x < n_cols / 2 - 2) {
        toggle_debug();
    }
}

void GameInstance::clear_tile(float x, float y) {
    Tile& t = board[x][y];
    if (t.hidden && !t.is_mine && !t.flagged) {
        t.tile_sprite.setTexture(*textures["tile_revealed"]);
        t.hidden = false;
        if (t.n_mines_near > 0) {
            t.overlay_sprite.setTexture(number_textures[t.n_mines_near]);
            t.draw_overlay = true;
        } 

        if (++tiles_revealed == n_cols * n_rows - n_mines) {
            game_over = true;
            write_to_leaderboard();
            UI_elements["face_happy"]->setTexture(*textures["face_win"]);
        }

        if (t.n_mines_near == 0) {
            operateOnNeighbors(x, y, [this](float x, float y) {this->clear_tile(x, y);});
        }
    }
}

void GameInstance::write_to_leaderboard() {
    std::fstream file("../assets/leaderboard.txt");
    std::ostringstream new_leaderboard;
    std::string score;
    std::string name;
    bool made_change = false;
    for (size_t i = 1; i < 6; i++) {
        if (file.peek() != EOF) {
            file >> score >> name;
            if (time <= std::stoi(score) && !made_change) {
                new_leaderboard << time << " " << player_name << "\n";
                made_change = true;
            }
            if (i == 5 && made_change) continue;
            new_leaderboard << score << " " << name << "\n";
        }
    }
    file.close();
    file.open("../assets/leaderboard.txt", std::ios::out | std::ios::trunc);
    file << new_leaderboard.str();
    file.close();
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
    for (auto pair : UI_elements) {
        window.draw(*pair.second);
    }
    window.display();
}

void GameInstance::display_time() {
    UI_elements["ones"]->setTexture(stopwatch_textures[time % 10]);
    UI_elements["tens"]->setTexture(stopwatch_textures[time / 10 % 10]);
    UI_elements["hundreds"]->setTexture(stopwatch_textures[time / 100 % 10]);
    redraw_screen();
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
    for (auto pair : UI_elements) {
        delete pair.second;
    }
}

void GameInstance::leaderboard_loop() {
    sf::RenderWindow w(
    sf::VideoMode(
        {static_cast<unsigned>(16 * 16), static_cast<unsigned>(16 * 16 + 50)}),
    "Minesweeper", sf::Style::Close);

    w.clear(sf::Color::White);
    sf::Text title(font, "LEADERBOARD", 20);
    title.setStyle(sf::Text::Bold);
    title.setStyle(sf::Text::Underlined);
    title.setFillColor(sf::Color::Black);
    title.setPosition({w.getSize().x / 2.0f - title.getLocalBounds().size.x / 2, w.getSize().y / 2.0f - 120.0f});
    w.draw(title);

    std::ostringstream body_string;
    std::ifstream file("../assets/leaderboard.txt");
    std::string score;
    std::string name;
    for (size_t i = 1; i < 6; i++) {
        if (file.peek() != EOF) {
            file >> score >> name;
            int t = std::stoi(score);
            int min = t / 60;
            int sec = t % 60;
            std::string min_pad = min < 10 ? "0" : "";
            std::string sec_pad = sec < 10 ? "0" : "";
            body_string << i << ". " << min_pad << min << ":" << sec_pad << sec << "\t" << name;
            if (std::stoi(score) == time && name == player_name) {
                body_string << "*";
            }
        }
        body_string << "\n\n";
    }
    file.close();
    sf::Text body(font, body_string.str(), 18);
    body.setStyle(sf::Text::Bold);
    body.setFillColor(sf::Color::Black);
    body.setPosition({w.getSize().x / 2.0f - body.getLocalBounds().size.x / 2, w.getSize().y / 2.0f - 60});
    w.draw(body);

    w.display();

    while (w.isOpen()) {
        while (const std::optional event = w.pollEvent()) {
            if (event->is<sf::Event::Closed>()) w.close();
        }
    }
}