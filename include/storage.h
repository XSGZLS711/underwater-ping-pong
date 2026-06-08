#ifndef STORAGE_H
#define STORAGE_H

#include "model.h"

profiles_data load_profiles();
void save_profiles(const profiles_data& profiles);

leaderboard_data load_leaderboard();
void save_leaderboard(const leaderboard_data& leaderboard);

void load_game_data(game_data& game);
void save_game_data(const game_data& game);

#endif