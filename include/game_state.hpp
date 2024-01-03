// game_state.hpp
#ifndef GAMESTATE_H
#define GAMESTATE_H

// #include <iostream>
// #include <iomanip> // Include the header for std::setw
#include <vector>
// #include <numeric> // For accumulate
// #include <thread>
// #include <string>
// #include <ctime>
// #include <unordered_set>
// #include <cstdlib>
// #include <chrono>
// #include <utility>
// #include <algorithm>

#include "constants.hpp"
#include "astar.hpp"

using std::vector;
using std::pair;

class GameState {
public:
    bool player1;
    int size;
    int num_walls;
    int walls_dim;
    pair<int, int> player1_pos;
    pair<int, int> player2_pos;
    pair<int, int> walls_per_player;
    bool check_wall_blocks_exit_on_gen;
    vector<int> ver_walls; // vertical walls (ints of 1's and 0's)
    vector<int> hor_walls; //horizontal walls (ints of 1's and 0's)
    vector<int> player_walls; // which player placed a wall (only entries which are 1 in ver_walls or hor_walls actually treated as 'holding anything')
    vector< vector<int> > saved_wall_placements;

    GameState();

    void reinitialize();

    void copy(GameState& game_state);

    bool is_hor_wall(const int x, const int y);

    bool is_ver_wall(const int x, const int y);

    bool is_wall_player1(const int x, const int y);

    void set_wall(const int x, const int y, const bool isHorizontal);

    void set_which_player_placed_wall(const int x, const int y, const bool is_player1);

    void clear_wall(const int x, const int y, const bool isHorizontal);

    bool is_wall_blocking_move(const pair<int,int>& pos, const pair<int,int>& new_pos);

    bool is_valid_move(pair<int,int>& pos, pair<int,int>& new_pos);

    vector<vector<int>> get_available_moves(bool computeNewWallPlacements = true);

    bool is_wall_placement_valid(const pair<int,int>& pos, const bool isHorizontal);

    bool is_wall_blocking_exit(const pair<int, int> pos, const int isHorizontal);

    vector<vector<int>> get_available_wall_placements(bool compute_new_wall_placements = true);

    void update_available_wall_placements();

    bool is_goal_state();

    int get_winner();

    void place_wall(vector<int> inp, bool check_if_valid = true, bool compute_new_wall_placements = true);

    void move_piece(vector<int> new_move, bool compute_new_wall_placements = true);
};

#endif // GAMESTATE_H