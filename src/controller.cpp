#include "controller_internal.h"
#include "storage.h"
//-------------------- Screen updates --------------------

void update_button(button_data& button){
    button.was_hovered = button.hovered;

    button.hovered = point_in_rectangle(mouse_position(), button.bounds);
    button.clicked = button.hovered && mouse_clicked(LEFT_BUTTON);

    if(button.hovered && !button.was_hovered){
        play_sound_effect("button_hover", 0.35);
    }

    if(button.clicked){
        play_sound_effect("button_click", 0.5);
    }
}


void select_previous_profile(profiles_data& profiles){
    if(profiles.items.length() == 0){
        return;
    }

    profiles.current_index--;

    if(profiles.current_index < 0){
        profiles.current_index = 0;
    }
}

void select_next_profile(profiles_data& profiles){
    if(profiles.items.length() == 0){
        return;
    }

    profiles.current_index++;

    if(profiles.current_index >= profiles.items.length()){
        profiles.current_index = profiles.items.length() - 1;
    }
}

void keep_selected_profile_visible(game_data& game){
    int max_visible = 4;
    int selected = game.profiles.current_index;

    if(selected < game.profile_select_menu.first_visible_index){
        game.profile_select_menu.first_visible_index = selected;
    }

    if(selected >= game.profile_select_menu.first_visible_index + max_visible){
        game.profile_select_menu.first_visible_index = selected - max_visible + 1;
    }
}

void update_profile_select(game_data& game){
    update_button(game.profile_select_menu.create_button);
    update_button(game.profile_select_menu.continue_button);

    if(key_typed(UP_KEY)){
        select_previous_profile(game.profiles);
        keep_selected_profile_visible(game);
        save_game_data(game);
    }
    else if(key_typed(DOWN_KEY)){
        select_next_profile(game.profiles);
        keep_selected_profile_visible(game);
        save_game_data(game);
    }
    else if(game.profile_select_menu.continue_button.clicked || key_typed(RETURN_KEY)){
        game.current_screen = MENU_SCREEN;
    }
    else if(game.profile_select_menu.create_button.clicked){
        game.profile_create_menu.typed_name = "";

        rectangle input_box = {390, 245, 300, 45};
        start_reading_text(input_box, "");


        game.current_screen = PROFILE_CREATE_SCREEN;
    }
}


void add_profile(profiles_data& profiles, string name){
    profile_data profile;
    profile.name = name;
    profile.total_coins = 0;
    profile.best_score = 0;
    profile.games_played = 0;
    profile.selected_character = 0;

    profiles.items.add(profile);
    profiles.current_index = profiles.items.length() - 1;
}

bool valid_profile_name(string name){
    return name != "" && name.length() < 12;
}

bool profile_name_exists(const profiles_data& profiles, string name){
    for(int i = 0; i < profiles.items.length(); i++){
        if(name == profiles.items[i].name){
            return true;
        }
    }
    return false;
}

void update_profile_create(game_data& game){
    update_button(game.profile_create_menu.create_button);
    update_button(game.profile_create_menu.back_button);

    if(reading_text()){
        game.profile_create_menu.typed_name = text_input();
    }

    if(game.profile_create_menu.create_button.clicked){
        string name = trim(game.profile_create_menu.typed_name);
        if(valid_profile_name(name) && !profile_name_exists(game.profiles, name)){
            add_profile(game.profiles, name);
            keep_selected_profile_visible(game);
            save_game_data(game);
            end_reading_text();
            game.current_screen = PROFILE_SELECT_SCREEN;
        }
    }
    else if(game.profile_create_menu.back_button.clicked){
        end_reading_text();
        game.current_screen = PROFILE_SELECT_SCREEN;
    }
}


void update_menu(game_data& game){
    update_button(game.main_menu.start_button);
    update_button(game.main_menu.info_button);
    update_button(game.main_menu.left_character_button);
    update_button(game.main_menu.right_character_button);
    update_button(game.main_menu.unlock_button);
    update_button(game.main_menu.leaderboard_button);
    update_button(game.main_menu.quit_button);
    update_button(game.main_menu.switch_profile_button);

    if(game.main_menu.start_button.clicked || key_typed(SPACE_KEY)){
        game.round = new_round();
        game.current_screen = PLAYING_SCREEN;
        play_game_music_from_start();
    }
    else if(game.main_menu.info_button.clicked){
        game.current_screen = INSTRUCTIONS_SCREEN;
    }
    else if(game.main_menu.leaderboard_button.clicked){
        game.current_screen = LEADERBOARD_SCREEN;
    }
    else if(game.main_menu.quit_button.clicked){
        game.should_quit = true;
    }
    else if(game.main_menu.switch_profile_button.clicked){
        keep_selected_profile_visible(game);
        game.current_screen = PROFILE_SELECT_SCREEN;
    }
}


void update_instructions(game_data& game){
    update_button(game.instructions_menu.back_button);

    if(game.instructions_menu.back_button.clicked || key_typed(M_KEY)){
        game.current_screen = MENU_SCREEN;
    }
}


void update_playing(game_data& game){
    update_debug(game);

    update_round_clock(game.round);
    update_difficulty(game.round);
    update_item_effects(game.round);

    update_bars(game.round.bars);
    handle_water_pulse(game.round);
    update_fishes(game.round);
    update_torpedoes(game);
    update_ball(game.round.ball);

    handle_ball_fish_collision(game.round);
    handle_ball_bars_collision(game.round);
    handle_ball_window_collision(game.round.ball, game.debug.window_collision);
    update_floating_scores(game.round);

    update_ball_animation(game.round.ball, game.round.clock.time_passed_ms);

    update_finish_round(game);

    if(key_typed(P_KEY)){
        pause_game_music();
        game.current_screen = PAUSE_SCREEN;
    }
}


void update_pause(game_data& game){
    update_button(game.pause_menu.resume_button);
    update_button(game.pause_menu.restart_button);
    update_button(game.pause_menu.menu_button);

    if(game.pause_menu.resume_button.clicked || key_typed(P_KEY)){
        game.round.clock.last_update_ms = current_ticks();
        game.current_screen = PLAYING_SCREEN;
        resume_game_music();
    }
    else if(game.pause_menu.restart_button.clicked || key_typed(R_KEY)){
        game.round = new_round();
        game.current_screen = PLAYING_SCREEN;
        play_game_music_from_start();
    }
    else if(game.pause_menu.menu_button.clicked || key_typed(M_KEY)){
        stop_game_music();
        settle_round(game);
        game.current_screen = MENU_SCREEN;
    }
}


void update_leaderboard(game_data& game){
    update_button(game.leaderboard_menu.back_button);

    if(game.leaderboard_menu.back_button.clicked || key_typed(M_KEY)){
        game.current_screen = MENU_SCREEN;
    }
}


void update_game_over(game_data& game){
    update_button(game.game_over_menu.restart_button);
    update_button(game.game_over_menu.menu_button);

    if(game.game_over_menu.restart_button.clicked || key_typed(R_KEY)){
        game.round = new_round();
        game.current_screen = PLAYING_SCREEN;
        play_game_music_from_start();
    }
    else if(game.game_over_menu.menu_button.clicked || key_typed(M_KEY)){
        game.current_screen = MENU_SCREEN;
    }
}


void update_game(game_data& game){
    switch (game.current_screen){
        case PROFILE_SELECT_SCREEN: update_profile_select(game); break;
        case PROFILE_CREATE_SCREEN: update_profile_create(game); break;
        case MENU_SCREEN: update_menu(game); break;
        case INSTRUCTIONS_SCREEN: update_instructions(game); break;
        case PLAYING_SCREEN: update_playing(game); break;
        case PAUSE_SCREEN: update_pause(game); break;
        case LEADERBOARD_SCREEN: update_leaderboard(game); break;
        case GAME_OVER_SCREEN: update_game_over(game); break;
    }
}
