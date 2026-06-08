#include "controller_internal.h"
#include "storage.h"
//-------------------- Game flow and settlement --------------------

void play_game_music_from_start(){
    set_music_volume(0.25);

    if(music_playing() || music_paused()){
        stop_music();
    }
    play_music("underwater_bgm", -1);
}

void resume_game_music(){
    set_music_volume(0.25);

    if(music_paused()){
        resume_music();
    }
    else if(!music_playing()){
        play_music("underwater_bgm", -1);
    }
}

void pause_game_music(){
    if(music_playing()){
        pause_music();
    }
}

void stop_game_music(){
    if(music_playing() || music_paused()){
        stop_music();
    }
}

void sort_leaderboard(leaderboard_data& leaderboard){
    int count = leaderboard.entries.length();
    leaderboard_entry_data temp;
    dynamic_array<leaderboard_entry_data>& e = leaderboard.entries;
    for(int i = count - 1; i >= 1; i--){
        for(int j = 0; j < i; j++){
            if(e[j].score < e[j + 1].score ||
              (e[j].score == e[j + 1].score && e[j].survival_time_ms < e[j + 1].survival_time_ms)
            )
            {
                temp = e[j];
                e[j] = e[j + 1];
                e[j + 1] = temp;
            }
        }
    }
}
//添加排序并删减记录
void add_leaderboard_entry(leaderboard_data& leaderboard, leaderboard_entry_data entry){
    leaderboard.entries.add(entry);

    sort_leaderboard(leaderboard);
    
    while(leaderboard.entries.length() > LEADERBOARD_MAX_ENTRIES){
        leaderboard.entries.remove(leaderboard.entries.length() - 1);
    }
}

void settle_round(game_data& game){
    if(game.round.settlement.completed){
        return;
    }
    game.round.settlement.final_score = game.round.player.score;
    game.round.settlement.item_fish_captured = game.round.stats.item_fish_captured;
    game.round.settlement.score_fish_captured = game.round.stats.score_fish_captured;
    game.round.settlement.survival_time_ms = game.round.clock.time_passed_ms;

    game.round.settlement.coins_earned = game.round.settlement.final_score / SCORE_PER_COIN;

    profile_data& profile = current_profile(game);
    profile.total_coins += game.round.settlement.coins_earned;
    
    if(game.round.settlement.final_score > profile.best_score){
        profile.best_score = game.round.settlement.final_score;
    }
    profile.games_played++;
    
    add_leaderboard_entry(game.leaderboard, new_leaderboard_entry(game));
    game.round.settlement.completed = true;

    save_game_data(game);
}

void finish_round(game_data& game){
    settle_round(game);
    stop_game_music();
    game.current_screen = GAME_OVER_SCREEN;
}

void update_finish_round(game_data& game){
    if(game.round.player.health <= 0){
        game.round.player.health = 0;
        finish_round(game);
        return;
    }

    if(ball_out_of_play_area(game.round.ball)){
        game.round.player.health = 0;
        finish_round(game);
        return;
    }
}

void update_debug(game_data& game){
    if(key_typed(BACKSPACE_KEY)){
        game.debug.on = !game.debug.on;
        if(!game.debug.on){
            game.debug.show_collision = false;
            game.debug.window_collision = false;
        }
    }
    if(!game.debug.on){
        return;
    }
    
    if(game.debug.on && key_typed(NUM_1_KEY)){
        game.debug.show_collision = !game.debug.show_collision;
    }
    if(game.debug.on && key_typed(NUM_2_KEY)){
        game.debug.window_collision = !game.debug.window_collision;
    }
    if(game.debug.on && key_typed(UP_KEY)){
        game.round.player.health++;
    }
    if(game.debug.on && key_typed(DOWN_KEY)){
        game.round.player.health--;
    }
}
//-------------------- Game clock --------------------

void update_round_clock(round_data& round){
    long now = current_ticks();
    long delta = now - round.clock.last_update_ms;

    round.clock.time_passed_ms += delta;
    round.clock.last_update_ms = now;
}
