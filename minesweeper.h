#pragma once

#include <cstdio>

class GameInstance {
    size_t n_rows;
    size_t n_cols;
    void read_config_file();
    void welcome_loop();
    void game_loop();
public:
    GameInstance() {
        read_config_file();
        welcome_loop();
        game_loop();
    }
};
