#include "storage.h"

#include <fstream>  //file stream
#include <sstream>  //string stream
#include <filesystem>

using namespace std;

const string DATA_FOLDER = "data";
const string PROFILES_FILE = "data/profiles.txt";
const string LEADERBOARD_FILE = "data/leaderboard.txt";

//如果 data 文件夹不存在，就创建它；如果已经存在，就什么都不做。
void ensure_data_folder(){
    filesystem::create_directories(DATA_FOLDER);
}

void save_profiles(const profiles_data& profiles){
    ensure_data_folder();

    ofstream file(PROFILES_FILE);
    if(!file.is_open()){
        return;
    }

    file << "CURRENT|" << profiles.current_index << "\n";

    for(int i = 0; i < profiles.items.length(); i++){
        const profile_data& profile = profiles.items[i];

        file << "PROFILE|"
             << profile.name << "|"
             << profile.total_coins << "|"
             << profile.best_score << "|"
             << profile.games_played << "|"
             << profile.selected_character
             << "\n";
    }
}

profiles_data load_profiles(){
    profiles_data profiles;
    profiles.current_index = 0;

    ifstream file(PROFILES_FILE);
    if(!file.is_open()){
        return new_profiles();
    }

    string line;
    //不为空行就循环
    while(getline(file, line)){
        stringstream ss(line);

        string tag;
        getline(ss, tag, '|');  //从当前这一行里读取内容，直到遇到 | 为止。

        if(tag == "CURRENT"){
            string index_text;
            getline(ss, index_text, '\n');
            profiles.current_index = stoi(index_text);  //string to int
        }
        else if(tag == "PROFILE"){
            profile_data profile;

            string coins_text;
            string best_score_text;
            string games_played_text;
            string select_character_text;

            getline(ss, profile.name, '|');
            getline(ss, coins_text, '|');
            getline(ss, best_score_text, '|');
            getline(ss, games_played_text, '|');
            getline(ss, select_character_text, '\n');

            profile.total_coins = stoi(coins_text);
            profile.best_score = stoi(best_score_text);
            profile.games_played = stoi(games_played_text);
            profile.selected_character = stoi(select_character_text);

            profiles.items.add(profile);
        }
    }
    if(profiles.items.length() == 0){
        return new_profiles();
    }
    //越界保护
    if(profiles.current_index < 0 || profiles.current_index >= profiles.items.length()){
        profiles.current_index = 0;
    }
    return profiles;
}

void save_leaderboard(const leaderboard_data& leaderboard){
    ensure_data_folder();

    ofstream file(LEADERBOARD_FILE);
    if(!file.is_open()){
        return;
    }

    for(int i = 0; i < leaderboard.entries.length(); i++){
        const leaderboard_entry_data& entry = leaderboard.entries[i];

        file << "ENTRY|"
             << entry.player_name << "|"
             << entry.score << "|"
             << entry.survival_time_ms
             << "\n";
    }
}

leaderboard_data load_leaderboard(){
    leaderboard_data leaderboard;

    ifstream file(LEADERBOARD_FILE);
    if(!file.is_open()){
        return leaderboard;
    }

    string line;

    while(getline(file, line)){
        stringstream ss(line);

        string tag;
        getline(ss, tag, '|');

        if(tag == "ENTRY"){
            leaderboard_entry_data entry;

            string score_text;
            string survival_time_text;

            getline(ss, entry.player_name, '|');
            getline(ss, score_text, '|');
            getline(ss, survival_time_text, '\n');

            entry.score = stoi(score_text);
            entry.survival_time_ms = stol(survival_time_text);  //string to long

            leaderboard.entries.add(entry);
        }
    }

    return leaderboard;
}

void load_game_data(game_data& game){
    game.profiles = load_profiles();
    game.leaderboard = load_leaderboard();
}

void save_game_data(const game_data& game){
    save_profiles(game.profiles);
    save_leaderboard(game.leaderboard);
}