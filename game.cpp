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
#include <utility>
#include <algorithm>

#include "include/game.hpp"


const std::string DEFAULT_WALL_COLOR = Color::PINK;
const std::string PLAYER1COLOR = Color::LIGHT_BLUE;
const std::string PLAYER2COLOR = Color::LIGHT_RED;

using std::cout;
using std::endl;
using std::min;
using std::max;

void p(int i){
    cout << i << endl;
}

Game::Game(bool user_sim, bool verbose, int rounds, int sim_delay)
        : is_user_sim(user_sim), 
        verbose(verbose),
        rounds(rounds), 
        sim_delay(sim_delay)
        {
            game_state = GameState();
    }

void Game::print_commands() {
    cout << "1. Move your piece by entering mx,y where x is the row number and y column number" << endl;
    cout << "2. Place a wall by entering wa,bd where a is the row letter and b column letter, and where d represents the wall direction and it can be either [v,h]. They represent the vertical or horizontal orientations." << endl;
    cout << '\n' << endl;
}

void Game::Initialize(){
    for (int i = 0; i < SIZE; ++i){
        MoveKeyValues += std::to_string(i);
    }

    for (int i = 0; i < SIZE - 1; ++i) {
        WallKeyValues += static_cast<char>('A' + i);
    }
}

void Game::quick_run(const string& bot1, const string& bot2) {
    cout << '\n' << endl;
    cout << '\n' << endl;
    cout << "### Quick Running rounds ###" << endl;
    player_simulation_algorithms[0] = bot1;
    player_simulation_algorithms[1] = bot2;
    cout << "Chosen algorithm for player 1 is " << bot1 << endl;
    cout << "Chosen algorithm for player 2 is " << bot2 << endl;
    cout << '\n' << endl;
}

void Game::player1_user(){
    return;
}

vector<int> Game::randombot_agent(){
    return randombot_action(game_state);
}

void Game::execute_action( vector<int> action){
    if (action.size() == 2){
        game_state.move_piece(action);
    }
    else{
        game_state.place_wall(action);
    }
}

bool Game::player_simulation() {
    int index = 1 * (!game_state.player1);
    int player_number = index + 1;

    cout << "Player " << player_number << " (" << player_simulation_algorithms[index] << ") is thinking...   ";

    vector<int> action;

    std::clock_t t1 = std::clock();

    // if (player_simulation_algorithms[index] == "minimax-alpha-beta-pruning") {
    //     action = minimax_agent(cop, game_state.player1);
    // }
    // else if (player_simulation_algorithms[index] == "path-search") {
    //     action = pathsearch_agent(cop, game_state.player1);
    // }
    if (player_simulation_algorithms[index] == "randomBot") {
        action = randombot_agent();
    }
    
    // else if (player_simulation_algorithms[index] == "impatientBot") {
    //     action = impatientbot_agent(cop);
    // }
    // else if (player_simulation_algorithms[index] == "online-bot") {
    //     while (online_move == std::make_pair(0, 0)) {
    //         // Wait for online move
    //     }
    //     action = online_move;
    //     online_move = std::make_pair(0, 0); // Reset online_move
    // }
    else {
        cout << "No bot configured" << std::endl;
    }

    if (!action.empty()) {
        std::clock_t t2 = std::clock();
        execute_action(action);
        // if (game_state.player1){
        //     execution_times[-1].first.push_back(static_cast<double>(t2 - t1) / CLOCKS_PER_SEC);
        //     hist_per_round[-1].first.push_back(action);
        //     hist_per_round.emplace_back(vector<vector<int>>{}, vector<vector<int>>{});
        //     execution_times.emplace_back(vector<int>{}, vector<int>{});
        // }
        // else{
        //     execution_times[-1].second.push_back(static_cast<double>(t2 - t1) / CLOCKS_PER_SEC);
        //     hist_per_round[-1].second.push_back(action);
        // }

        if (action.size() == 2) {
            cout << "\r Player " << player_number << " (" << player_simulation_algorithms[index] << ") has moved his piece to " << action[0] << ", " << action[1] << "." << std::endl;
        }
        else {
            string orientation = (action[2]) ? "HORIZONTAL" : "VERTICAL";
            string loc = "(" + string(1, static_cast<char>('a' + action[0])) + ", " + string(1, static_cast<char>('a' + action[1])) + ")";
            cout << "\r Player " << player_number << " (" << player_simulation_algorithms[index] << ") has placed a " << orientation << " wall at " << loc << "." << std::endl;
        }
        cout << "This took " << static_cast<double>(t2 - t1) / CLOCKS_PER_SEC << " seconds." << std::flush;
        return true;
    }
    else {
        cout << "Player " << player_number << " (" << player_simulation_algorithms[index] << ") has no moves left." << std::endl;
        return false;
    }
}

void Game::play() {
    while (rounds > 0) {
        double start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        // game_state.get_available_wall_placements();
        cout << endl;
        cout << "\n";
        print_game_stats();
        cout << "\n";
        print_board();
        cout << endl;

        if (game_state.is_goal_state()) {
            
            hist_per_round.emplace_back(vector<vector<int>>{}, vector<vector<int>>{});
            execution_times.emplace_back(vector<int>{}, vector<int>{});

            vector<int> exec1 = execution_times[-1].first;
            vector<int> exec2 = execution_times[-1].second;

            double avgtime1 = static_cast<double>(accumulate(exec1.begin(), exec1.end(), 0)) / exec1.size();
            double avgtime2 = static_cast<double>(accumulate(exec2.begin(), exec2.end(), 0)) / exec2.size();

            cout << "Execution averages this round: " << avgtime1 << " " << avgtime2 << endl;
            cout << "Number of moves this round: " << hist_per_round[-1].first.size() << " " << hist_per_round[-1].second.size() << endl;
            
            int winner_ind = game_state.get_winner();
            wins[winner_ind]++;
            rounds--;
            game_state.reinitialize();

            if (is_user_sim) {
                if (winner_ind == 0) {
                    print_colored_output("You won!", Color::GREEN);
                } else {
                    print_colored_output("You lost!", Color::RED);
                }
            } else {
                string winner = (winner_ind == 0) ? "P1" : "P2";
                print_colored_output("The winner is " + winner + ".", Color::CYAN);
                if (rounds >= 1) {
                    cout << "restarting in 3:" << std::flush;
                    std::this_thread::sleep_for(std::chrono::seconds(3));;
                }
            }
            continue;
        }
        
        if (game_state.player1) {
            if (is_user_sim) {
                player1_user();
            } else {
                bool res = player_simulation();
                if (!res) {
                    cout << "Bot has returned something unholy" << endl;
                    exit(1);
                }
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time < sim_delay) {
                    continue;
                }
            }
        } else {
            bool res = player_simulation();
            if (!res) {
                cout << "Bot has returned something unholy" << endl;
                exit(2);
            }
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time < sim_delay) {
                continue;
            }
        }
        game_state.player1 = !game_state.player1;
    }
}

void Game::print_game_stats() {
    if (!verbose) {
        return;
    }
    
    GameState& g = game_state;

    cout << PLAYER1COLOR << std::left << std::setw(15) << "Player 1 walls" << Color::WHITE << "|" << PLAYER2COLOR << std::left << std::setw(15) << "Player 2 walls" << Color::RESET << "|" << endl;
    cout << std::left << std::setw(15) << "" << "|" << std::left << std::setw(15) << "" << "|" << endl;
    cout << std::left << std::setw(15) << g.walls_per_player.first << "|" << std::left << std::setw(15) << g.walls_per_player.second << "|" << endl;
    cout << endl << endl;
}

void Game::print_board() {
    if (!verbose) {
        return;
    }

    GameState& g = game_state;

    for (int i = 0; i < g.size; ++i) {
        if (i == 0) {
            cout << "      " << std::setw(2) << i << "  " << DEFAULT_WALL_COLOR << static_cast<char>('a' + i) << Color::RESET;
        } else if (i == g.size - 1) {
            cout << " " << std::setw(2) << i << "  ";
        } else {
            cout << " " << std::setw(2) << i << "  " << DEFAULT_WALL_COLOR << static_cast<char>('a' + i) << Color::RESET;
        }
    }
    cout << endl << endl;

    for (int i = 0; i < g.walls_dim + g.size; ++i) {
        if (i % 2 == 0) {
            cout << std::setw(2) << i / 2 << "  ";
        } else {
            cout << DEFAULT_WALL_COLOR << std::setw(2) << static_cast<char>('a' + i / 2) << Color::RESET << "  ";
        }

        for (int j = 0; j < g.walls_dim + g.size; ++j) {
            if (i % 2 == 0) {
                int x = i / 2;
                int y = j / 2;

                if (j % 2 == 0) {
                    if (g.player1_pos == std::make_pair(x, y)) {
                        cout << PLAYER1COLOR << " P1 " << Color::RESET;
                    } else if (g.player2_pos == std::make_pair(x, y)) {
                        cout << PLAYER2COLOR << " P2 " << Color::RESET;
                    } else {
                        cout << "    ";
                    }
                } else {
                    if (g.is_ver_wall(min(g.walls_dim - 1, x), y)) {
                        cout << getWallColor(g, min(g.walls_dim - 1, x), y) << " \u2503" << Color::RESET;
                    } else if (g.is_ver_wall(max(0, x - 1), y)) {
                        cout << getWallColor(g, max(0, x - 1), y) << " \u2503" << Color::RESET;
                    } else {
                        cout << " |";
                    }
                }
            } else {
                if (j % 2 == 0) {
                    int x = i / 2;
                    int y = j / 2;

                    if (g.is_hor_wall(x, min(g.walls_dim - 1, y))) {
                        string line = "";
                        for (int k = 0; k < 5; ++k) {
                            line += "\u2501";
                        }
                        cout << getWallColor(g, x, min(g.walls_dim - 1, y)) << line << Color::RESET;
                    } else if (g.is_hor_wall(x, max(0, y - 1))) {
                        string line = "";
                        for (int k = 0; k < 5; ++k) {
                            line += "\u2501";
                        }
                        cout << getWallColor(g, x, max(0, y - 1)) << line << Color::RESET;
                    } else {
                        string line = "";
                        for (int k = 0; k < 5; ++k) {
                            line += "\u23AF";
                        }
                        cout << line;
                    }
                } else {
                    if (!g.is_hor_wall(i/2, j/2) && !g.is_ver_wall(i/2, j/2)) {
                        cout << "o";
                    } else {
                        cout << getWallColor(g, i / 2, j / 2) << "o" << Color::RESET;
                    }
                }
            }
        }
        cout << endl;
    }
}

string Game::getWallColor(GameState& g, int i, int j) {
    return (g.is_wall_player1(i, j)) ? PLAYER1COLOR : PLAYER2COLOR;
}

void Game::print_colored_output(const string& text, const string& color, bool wipe, const string& _end) {
    if (wipe) {
        cout << '\r' << color << text << Color::RESET << _end << std::flush;
    } else {
        cout << color << text << Color::RESET << _end << std::flush;
    }
}


int main() {
    // Your program code here
    Game g = Game(false, true, 2, 0);
    g.print_commands();
    g.play();
    return 0;
}