#include <iostream>
#include <iomanip> // Include the header for std::setw
#include <vector>
#include <numeric> // For accumulate
#include <thread>
#include <string>
#include <ctime>
#include <unordered_set>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <utility>
#include <algorithm>


#include "include/game_state.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::unordered_set;
using std::min;
using std::max;



GameState::GameState() 
        : player1(true), 
        size(SIZE), 
        num_walls(WALLS), 
        walls_dim(SIZE-1), 
        walls_per_player(std::make_pair(WALLS, WALLS)), 
        player1_pos(std::make_pair(SIZE-1, SIZE/2)), 
        player2_pos(std::make_pair(0,SIZE/2)), 
        check_wall_blocks_exit_on_gen(true) {
        
        reinitialize();
    }

void GameState::reinitialize() {
    
    player1 = true;
    player1_pos = std::make_pair(size-1, size/2); 
    player2_pos = std::make_pair(0,size/2); 
    ver_walls.clear();
    hor_walls.clear();
    player_walls.clear();
    ver_walls.resize(walls_dim, 0);
    hor_walls.resize(walls_dim, 0);
    player_walls.resize(walls_dim, 0);
    walls_per_player = std::make_pair(num_walls, num_walls);
    saved_wall_placements.clear();
}

void GameState::copy(GameState& game_state) {
    game_state.player1 = player1;
    game_state.size = size;
    game_state.walls_dim = walls_dim;
    game_state.ver_walls = ver_walls;
    game_state.hor_walls = hor_walls;
    game_state.walls_per_player = std::make_pair(num_walls, num_walls);
    game_state.player1_pos = player1_pos;
    game_state.player2_pos = player2_pos;
    game_state.saved_wall_placements = saved_wall_placements;
}

bool GameState::is_hor_wall(const int x, const int y){
    if (((hor_walls[x] >> y) & 1) == 1){
        return true;
    }
    return false;
}

bool GameState::is_ver_wall(const int x, const int y){
    if (((ver_walls[x] >> y) & 1) == 1){
        return true;
    }
    return false;
}

bool GameState::is_wall_player1(const int x, const int y){
    if (((player_walls[x] >> y) & 1) == 0){
        return true;
    }
    return false;
}

void GameState::set_wall(const int x, const int y, const bool isHorizontal){
    if (isHorizontal){
        hor_walls[x] |= (1 << y);
    }
    else{
        ver_walls[x] |= (1 << y);
    }
}

void GameState::set_which_player_placed_wall(const int x, const int y, const bool is_player1){
    if (is_player1){
        player_walls[x] &= ~(1 << y);
    }
    else{
        player_walls[x] |= (1 << y);
    }
}

void GameState::clear_wall(const int x, const int y, const bool isHorizontal){
    if (isHorizontal){
        hor_walls[x] = hor_walls[x] & ~(1 << y);
    }
    else{
        ver_walls[x] = ver_walls[x] & ~(1 << y);
    }
}

bool GameState::is_wall_blocking_move(const pair<int,int>& pos, const pair<int,int>& new_pos) {
    // Move sideways (check for vertical wall)
    if (pos.first == new_pos.first) {
        int y = min(min(pos.second, new_pos.second), walls_dim - 1);
        if (is_ver_wall(min(walls_dim - 1, pos.first), y) ||
            is_ver_wall(max(0, pos.first - 1), y)) {
            return true;
        }
    }
    // Move vertically (check for horizontal wall)
    else if (pos.second == new_pos.second) {
        int x = min(min(pos.first, new_pos.first), walls_dim - 1);
        if (is_hor_wall(x, min(walls_dim - 1, pos.second)) ||
            is_hor_wall(x, max(0, pos.second - 1))) {
            return true;
        }
    }
    return false;
}

bool GameState::is_valid_move(pair<int,int>& pos, pair<int,int>& new_pos) {
        // Out of bounds
        if (!(0 <= new_pos.first && new_pos.first < size && 0 <= new_pos.second && new_pos.second < size)) {
            return false;
        }

        // No wall between player and new_pos
        if (is_wall_blocking_move(pos, new_pos)) {
            return false;
        }

        return true;
    }

vector<vector<int>> GameState::get_available_moves(bool computeNewWallPlacements) {
    vector< vector<int> > availableMoves;

    vector< pair<int, int> > dirs = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    pair<int, int> pos, otherPos;

    if (player1) {
        pos = player1_pos;
        otherPos = player2_pos;
    } else {
        pos = player2_pos;
        otherPos = player1_pos;
    }

    // Standard moves
    for (const auto& dir : dirs) {
        int x = pos.first + dir.first;
        int y = pos.second + dir.second;

        pair<int, int> tpos = std::make_pair(x, y);
        if (is_valid_move(pos, tpos) && tpos != otherPos) {
            availableMoves.push_back({x,y});
        }
    }

    int xDiff = abs(otherPos.first - pos.first);
    int yDiff = abs(otherPos.second - pos.second);

    // if players next to each other
    if ((xDiff == 0 && yDiff == 1) || (xDiff == 1 && yDiff == 0)) {
        int xDir = otherPos.first - pos.first;
        int yDir = otherPos.second - pos.second;
        int xJump = otherPos.first + xDir;
        int yJump = otherPos.second + yDir;

        if (is_valid_move(pos, otherPos)) {

            // Vertical or horizontal jump
            pair<int, int> tpos = std::make_pair(xJump, yJump);
            if (is_valid_move(otherPos, tpos)) {
                availableMoves.push_back({xJump, yJump});
            }

            // Diagonal jump
            else if (xDiff == 0) {
                for (int i : {1, -1}) {
                    int xJump = otherPos.first + i;
                    int yJump = otherPos.second;
                    pair<int, int> tpos = std::make_pair(xJump, yJump);
                    if (is_valid_move(otherPos, tpos)) {
                        availableMoves.push_back({xJump, yJump});
                    }
                }
            } 
            else if (yDiff == 0) {
                for (int i : {1, -1}) {
                    int xJump = otherPos.first;
                    int yJump = otherPos.second + i;
                    pair<int, int> tpos = std::make_pair(xJump, yJump);
                    if (is_valid_move(otherPos, tpos)) {
                        availableMoves.push_back({xJump, yJump});
                    }
                }
            }
        }
    }
    return availableMoves;
}

bool GameState::is_wall_placement_valid(const pair<int,int>& pos, const bool isHorizontal) {
    int x = pos.first;
    int y = pos.second;

    if (!(0 <= x && x < walls_dim && 0 <= y && y < walls_dim)) {
        return false;
    }

    if (is_hor_wall(x,y) || is_ver_wall(x,y)){
        return false;
    }
    else if (isHorizontal){
        if (y > 0 && is_hor_wall(x, y-1) || (y < walls_dim - 1 && is_hor_wall(x, y+1))) {
            return false;
        }
    }
    else{
        if ((x > 0 && is_ver_wall(x-1, y)) || (x < walls_dim - 1 && is_ver_wall(x+1, y))){
            return false;
        }
    }

    if (check_wall_blocks_exit_on_gen) {
        if (is_wall_blocking_exit(pos, isHorizontal)) {
            return false;
        }
    }

    return true;
}

bool GameState::is_wall_blocking_exit(const pair<int, int> pos, const int isHorizontal) {
    
    set_wall(pos.first, pos.second, isHorizontal);

    std::pair<double, double> path_dists = aStarSearch(*this);
    bool exit_blocked = (path_dists.first != std::numeric_limits<double>::infinity() && path_dists.second != std::numeric_limits<double>::infinity());
    // bool exit_blocked = false;
    
    clear_wall(pos.first, pos.second, isHorizontal);
    
    return exit_blocked;
}

vector<vector<int>> GameState::get_available_wall_placements(bool compute_new_wall_placements) {
    vector<vector<int>> wall_placements;
    
    if (player1) {
        if (walls_per_player.first <= 0) {
            return wall_placements;
        }
    } else if (walls_per_player.second <= 0) {
        return wall_placements;
    }
    
    // Valid wall placement already known
    if (!saved_wall_placements.empty()) {
        return saved_wall_placements;
    }
    
    for (int i = 0; i < walls_dim; ++i) {
        for (int j = 0; j < walls_dim; ++j) {

            pair<int, int> tpos = std::make_pair(i,j);
            bool isHorizontal = true;
            
            // Check horizontals
            if (is_wall_placement_valid(tpos, isHorizontal)) {
                wall_placements.push_back({i,j, isHorizontal});
            }
            
            isHorizontal = false;
            
            // Check verticals
            if (is_wall_placement_valid(tpos, isHorizontal)) {
                wall_placements.push_back({i, j, isHorizontal});
            }
        }
    }
    
    // Save for reuse
    saved_wall_placements = wall_placements;
    
    return wall_placements;
}

void GameState::update_available_wall_placements() {
    vector<vector<int>> new_wall_placements;
    for (vector<int> item : saved_wall_placements) {
        pair<int, int> pos = std::make_pair(item[0], item[1]);
        bool orientation = item[2];
        if (is_wall_placement_valid(pos, orientation)) {
            new_wall_placements.push_back({pos.first, pos.second, orientation});
        }
    }
    saved_wall_placements = new_wall_placements;
}

bool GameState::is_goal_state() {
        if (player1) return player1_pos.first == 0;
        else return player2_pos.first == size-1;
    }

int GameState::get_winner(){
    return (player1_pos.first == 0) ? 0 : 1;
}

void GameState::place_wall(const vector<int> inp, bool check_if_valid, bool compute_new_wall_placements) {
    int x = inp[0];
    int y = inp[1];
    pair<int, int> pos = std::make_pair(x,y);
    int isHorizontal = inp[2];

    if (check_if_valid) {
        if (!is_wall_placement_valid(pos, isHorizontal)) {
            cout << "CRAP " << x << ", " << y << ", " << isHorizontal << endl;
            exit(2);
        }
    }

    if (player1) {
        walls_per_player.first--;
    } else {
        walls_per_player.second--;
    }

    set_wall(x, y, isHorizontal);
    set_which_player_placed_wall(x, y, player1);

    // Update wall placements
    if (compute_new_wall_placements) {
        update_available_wall_placements();
    } else {
        saved_wall_placements.clear();
    }
}

void GameState::move_piece(const vector<int> new_move, bool compute_new_wall_placements) {
    pair<int, int> pos, otherpos;

    if (player1) {
        pos = player1_pos;
        otherpos = player2_pos;
    } else {
        pos = player2_pos;
        otherpos = player1_pos;
    }

    pair<int, int> new_pos = std::make_pair(new_move[0], new_move[1]);

    if (!is_valid_move(pos, new_pos) || new_pos == otherpos) {
        cout << "CRAP " << new_pos.first << ", " << new_pos.second << endl;
        exit(2);
    }

    if (player1) {
        player1_pos = new_pos;
    } else {
        player2_pos = new_pos;
    }

    if (compute_new_wall_placements) {
        vector<vector<int>> wall_placements = get_available_wall_placements(false);

        for (int i : {-1, 0}) {
            for (int j : {-1, 0}) {
                vector<int> w1 = {new_pos.first + i, new_pos.second + j, 1};
                vector<int> w2 = {new_pos.first + i, new_pos.second + j, 0};
                vector<int> w3 = {pos.first + i, pos.second + j, 1};
                vector<int> w4 = {pos.first + i, pos.second + j, 0};

                for (const vector<int>& w : {w1, w2, w3, w4}) {
                    auto index = std::find(wall_placements.begin(), wall_placements.end(), w);
                    if (index != wall_placements.end()) {
                        wall_placements.erase(index);
                    }
                    pair<int, int> tpos = std::make_pair(w[0], w[1]);
                    if (is_wall_placement_valid(tpos, w[2])){
                        wall_placements.push_back(w);
                    }
                }
            }
        }
        saved_wall_placements = wall_placements;
    }
}
