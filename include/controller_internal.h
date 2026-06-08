#ifndef CONTROLLER_INTERNAL_H
#define CONTROLLER_INTERNAL_H

#include "controller.h"

//-------------------- Animation helpers --------------------

struct animation_rule{
    int priority;
    long duration_ms;
    bool loop;
};

//-------------------- Game flow and clock --------------------

void play_game_music_from_start();
void resume_game_music();
void pause_game_music();
void stop_game_music();
void settle_round(game_data& game);
void finish_round(game_data& game);
void update_finish_round(game_data& game);
void update_round_clock(round_data& round);
void update_debug(game_data& game);
//-------------------- Difficulty, items and floating score --------------------

void update_difficulty(round_data& round);
void update_item_effects(round_data& round);
void update_floating_scores(round_data& round);

//-------------------- Bar helpers --------------------

void update_bars(bars_data& bars);
void handle_ball_bars_collision(round_data& round);
vector_2d reflect_velocity(vector_2d v0, vector_2d normal);

//-------------------- Ball helpers --------------------

void handle_water_pulse(round_data& round);
void apply_impulse_to_ball(ball_data& ball, const impulse_data& impulse);
void update_ball(ball_data& ball);
void try_play_ball_animation(ball_data& ball, ball_animation_state state, long now);
void handle_ball_window_collision(ball_data& ball, bool window_collision);
void update_ball_animation(ball_data& ball, long now);
bool ball_out_of_play_area(const ball_data& ball);
//-------------------- Fish helpers --------------------

void update_fishes(round_data& round);
void handle_ball_fish_collision(round_data& round);
void try_play_fish_animation(fish_animation_data& animation, fish_animation_state state, long now);
bool apply_hit_to_fish(fish_data& fish, int hit_power);
void capture_fish(round_data& round, int fish_index);

//-------------------- Torpedo helpers --------------------

void update_torpedoes(game_data& game);

#endif
