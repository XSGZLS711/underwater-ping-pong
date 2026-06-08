#include "view_internal.h"

//-------------------- Menu and overlay rendering --------------------

void draw_button(const button_data& button){
    double x = button.bounds.x;
    double y = button.bounds.y;
    double w = button.bounds.width;
    double h = button.bounds.height;

    color fill_color;
    color border_color;
    color text_color;
    color shine_color;

    if(button.hovered){
        fill_color = rgba_color(45, 145, 175, 215);
        border_color = COLOR_WHITE;
        text_color = COLOR_WHITE;
        shine_color = rgba_color(255, 255, 255, 80);
    }
    else{
        fill_color = rgba_color(8, 55, 82, 200);
        border_color = rgba_color(150, 230, 245, 190);
        text_color = rgb_color(220, 250, 255);
        shine_color = rgba_color(255, 255, 255, 45);
    }

    if(button.hovered){
        fill_rectangle(rgba_color(80, 220, 255, 35), x - 5, y - 5, w + 10, h + 10);
    }

    fill_rectangle(fill_color, button.bounds);
    draw_rectangle(border_color, button.bounds);

    fill_rectangle(
        shine_color,
        x + 6,
        y + 5,
        w - 12,
        h * 0.28
    );

    double center_x = x + w / 2;
    double text_y = y + (h - BUTTON_FONT_SIZE) / 2 - 2;

    draw_centered_text(button.text, text_color, center_x, text_y, BUTTON_FONT_SIZE);
}


void draw_profile_select(const game_data& game){
    draw_underwater_background(current_ticks());

    fill_rectangle(rgba_color(4, 28, 48, 210), 250, 75, 580, 570);
    draw_rectangle(rgba_color(160, 230, 245, 210), 250, 75, 580, 570);

    draw_centered_text("SELECT PROFILE", COLOR_WHITE, WINDOW_WIDTH / 2, 115, TITLE_FONT_SIZE);

    int max_visible = 4;
    int start = game.profile_select_menu.first_visible_index;
    int count = game.profiles.items.length();

    for(int i = 0; i < max_visible; i++){
        int profile_index = start + i;

        if(profile_index >= count){
            break;
        }

        const profile_data& profile = game.profiles.items[profile_index];

        double y = 175 + i * 75;

        bool selected = profile_index == game.profiles.current_index;

        if(selected){
            fill_rectangle(rgba_color(70, 190, 220, 85), 315, y - 8, 450, 58);
            draw_text(">>", COLOR_WHITE, get_system_font(), 24, 330, y);
        }

        color name_color = selected ? COLOR_WHITE : rgb_color(205, 240, 250);
        color detail_color = selected ? rgb_color(255, 225, 95) : rgb_color(165, 220, 230);

        draw_text(profile.name, name_color, get_system_font(), 24, 365, y);
        draw_text(
            "Coins: " + to_string(profile.total_coins) + "  Best: " + to_string(profile.best_score),
            detail_color,
            get_system_font(),
            17,
            365,
            y + 30
        );
    }

    draw_button(game.profile_select_menu.create_button);
    draw_button(game.profile_select_menu.continue_button);
}


void draw_profile_create(const game_data& game){
    draw_underwater_background(current_ticks());

    fill_rectangle(rgba_color(4, 28, 48, 210), 290, 120, 500, 430);
    draw_rectangle(rgba_color(160, 230, 245, 210), 290, 120, 500, 430);

    draw_centered_text("CREATE PROFILE", COLOR_WHITE, WINDOW_WIDTH / 2, 165, TITLE_FONT_SIZE);

    string display_name = game.profile_create_menu.typed_name + "_";

    draw_centered_text(
        "Name: " + display_name,
        rgb_color(225, 250, 255),
        WINDOW_WIDTH / 2,
        270,
        BODY_FONT_SIZE
    );

    draw_button(game.profile_create_menu.create_button);
    draw_button(game.profile_create_menu.back_button);
}


void draw_menu_player_panel(const game_data& game){
    fill_rectangle(rgba_color(5, 32, 52, 190), 28, 24, 280, 68);
    draw_rectangle(rgba_color(175, 235, 245, 190), 28, 24, 280, 68);

    const profile_data& profile = current_profile(game);

    draw_text("Player: " + profile.name, COLOR_WHITE, get_system_font(), 16, 48, 38);
    draw_text("Coins: " + to_string(profile.total_coins), rgb_color(255, 225, 95), get_system_font(), 16, 48, 64);
    draw_button(game.main_menu.switch_profile_button);
}

void draw_menu_info_button(const button_data& button){
    double center_x = button.bounds.x + button.bounds.width / 2;
    double center_y = button.bounds.y + button.bounds.height / 2;

    color fill = button.hovered ? rgb_color(140, 230, 245) : rgba_color(222, 250, 255, 230);
    color border = button.hovered ? COLOR_WHITE : rgb_color(34, 100, 130);

    fill_circle(fill, center_x, center_y, 25);
    draw_circle(border, center_x, center_y, 25);
    draw_centered_text("i", rgb_color(12, 55, 76), center_x, center_y - 14, 28);
}

void draw_menu_icon_button(const button_data& button, string icon_text, color icon_color){
    color panel_color = button.hovered ? rgba_color(35, 96, 122, 220) : rgba_color(7, 42, 65, 195);
    color border_color = button.hovered ? COLOR_WHITE : rgba_color(160, 230, 245, 180);

    fill_rectangle(panel_color, button.bounds);
    draw_rectangle(border_color, button.bounds);

    double center_x = button.bounds.x + button.bounds.width / 2;
    double icon_y = button.bounds.y + 32;
    double text_y = button.bounds.y + 63;

    fill_circle(rgba_color(255, 255, 255, 60), center_x, icon_y, 24);
    draw_circle(icon_color, center_x, icon_y, 24);
    draw_centered_text(icon_text, icon_color, center_x, icon_y - 13, 24);
    draw_centered_text(button.text, COLOR_WHITE, center_x, text_y, 18);
}

bool mouse_near_character_showcase(){
    rectangle showcase_area = {250, 210, 580, 230};
    return point_in_rectangle(mouse_position(), showcase_area);
}

void draw_character_arrow_button(const button_data& button, bool points_left, bool visible){
    if(!visible){
        return;
    }

    double center_x = button.bounds.x + button.bounds.width / 2;
    double center_y = button.bounds.y + button.bounds.height / 2;

    color panel_color = button.hovered ? rgba_color(120, 220, 240, 170) : rgba_color(5, 28, 44, 110);
    color arrow_color = button.hovered ? COLOR_WHITE : rgba_color(220, 250, 255, 215);

    fill_circle(panel_color, center_x, center_y, 34);
    draw_circle(arrow_color, center_x, center_y, 34);

    if(points_left){
        fill_triangle(
            arrow_color,
            center_x - 12, center_y,
            center_x + 10, center_y - 18,
            center_x + 10, center_y + 18
        );
    }
    else{
        fill_triangle(
            arrow_color,
            center_x + 12, center_y,
            center_x - 10, center_y - 18,
            center_x - 10, center_y + 18
        );
    }
}

void draw_menu_shark_showcase(){
    int frame = (current_ticks() / 200) % 6;
    draw_sheet_frame(PLAYER_SHARK_SHEET, frame, WINDOW_WIDTH / 2, 320, 0.68, 0.68);
}

void draw_menu(const game_data& game){
    draw_underwater_background(current_ticks());
    draw_menu_player_panel(game);

    draw_button(game.main_menu.quit_button);

    draw_centered_text("UNDERWATER PING PONG", COLOR_WHITE, WINDOW_WIDTH / 2, 135, TITLE_FONT_SIZE);

    draw_menu_shark_showcase();

    bool show_character_arrows = mouse_near_character_showcase();
    draw_character_arrow_button(game.main_menu.left_character_button, true, show_character_arrows);
    draw_character_arrow_button(game.main_menu.right_character_button, false, show_character_arrows);

    draw_button(game.main_menu.unlock_button);
    draw_button(game.main_menu.start_button);
    draw_menu_info_button(game.main_menu.info_button);

    draw_menu_icon_button(game.main_menu.leaderboard_button, "#", rgb_color(160, 225, 255));
}


void draw_instructions(const game_data& game){
    draw_underwater_background(current_ticks());
    draw_centered_text("INSTRUCTIONS", COLOR_WHITE, WINDOW_WIDTH / 2, 120, TITLE_FONT_SIZE);

    draw_centered_text("Move the mouse to control the four bars.", rgb_color(225, 250, 255), WINDOW_WIDTH / 2, 210, BODY_FONT_SIZE);
    draw_centered_text("Click near the ball to create a short water pulse.", rgb_color(225, 250, 255), WINDOW_WIDTH / 2, 250, BODY_FONT_SIZE);
    draw_centered_text("Hit fish for score and items. Avoid torpedo explosions.", rgb_color(225, 250, 255), WINDOW_WIDTH / 2, 290, BODY_FONT_SIZE);
    draw_centered_text("Press P during play to pause.", rgb_color(225, 250, 255), WINDOW_WIDTH / 2, 330, BODY_FONT_SIZE);

    draw_button(game.instructions_menu.back_button);
}


string leaderboard_time_text(long survival_time_ms){
    int total_seconds = survival_time_ms / 1000;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;

    string seconds_text = to_string(seconds);
    if(seconds < 10){
        seconds_text = "0" + seconds_text;
    }

    return to_string(minutes) + ":" + seconds_text;
}

void draw_leaderboard(const game_data& game){
    draw_underwater_background(current_ticks());

    fill_rectangle(rgba_color(4, 28, 48, 205), 190, 85, 700, 520);
    draw_rectangle(rgba_color(160, 230, 245, 210), 190, 85, 700, 520);

    draw_centered_text("LEADERBOARD", COLOR_WHITE, WINDOW_WIDTH / 2, 120, TITLE_FONT_SIZE);

    double header_y = 185;
    draw_text("Rank", rgb_color(160, 230, 245), get_system_font(), 20, 250, header_y);
    draw_text("Player", rgb_color(160, 230, 245), get_system_font(), 20, 350, header_y);
    draw_text("Score", rgb_color(160, 230, 245), get_system_font(), 20, 570, header_y);
    draw_text("Time", rgb_color(160, 230, 245), get_system_font(), 20, 710, header_y);

    draw_line(rgba_color(160, 230, 245, 160), 235, 220, 845, 220, option_line_width(2));

    if(game.leaderboard.entries.length() == 0){
        draw_centered_text(
            "No records yet",
            rgb_color(220, 250, 255),
            WINDOW_WIDTH / 2,
            350,
            BODY_FONT_SIZE
        );
    }
    else{
        int count = game.leaderboard.entries.length();
        if(count > LEADERBOARD_MAX_ENTRIES){
            count = LEADERBOARD_MAX_ENTRIES;
        }

        for(int i = 0; i < count; i++){
            const leaderboard_entry_data& entry = game.leaderboard.entries[i];

            double row_y = 245 + i * 34;

            color row_color = COLOR_WHITE;
            if(i == 0){
                row_color = rgb_color(255, 215, 0);
            }
            else if(i == 1){
                row_color = rgb_color(192, 192, 192);
            }
            else if(i == 2){
                row_color = rgb_color(205, 127, 50);
            }

            if(i % 2 == 0){
                fill_rectangle(rgba_color(255, 255, 255, 20), 230, row_y - 6, 620, 28);
            }

            draw_text(to_string(i + 1), row_color, get_system_font(), 20, 260, row_y);
            draw_text(entry.player_name, row_color, get_system_font(), 20, 350, row_y);
            draw_text(to_string(entry.score), row_color, get_system_font(), 20, 570, row_y);
            draw_text(leaderboard_time_text(entry.survival_time_ms), row_color, get_system_font(), 20, 710, row_y);
        }
    }

    draw_button(game.leaderboard_menu.back_button);
}


void draw_pause_overlay(const game_data& game){
    fill_rectangle(rgba_color(0, 0, 0, 120), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    draw_centered_text("PAUSED", COLOR_WHITE, WINDOW_WIDTH / 2, 235, TITLE_FONT_SIZE);
    draw_button(game.pause_menu.resume_button);
    draw_button(game.pause_menu.restart_button);
    draw_button(game.pause_menu.menu_button);
}

void draw_game_over_overlay(const game_data& game){
    fill_rectangle(rgba_color(255, 255, 255, 180), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    draw_centered_text("GAME OVER", COLOR_BLACK, WINDOW_WIDTH / 2, 210, TITLE_FONT_SIZE);
    draw_centered_text("Survival Time: " + to_string(game.round.settlement.survival_time_ms / 1000, 2), COLOR_BLACK, WINDOW_WIDTH / 2, 275, BODY_FONT_SIZE);
    draw_centered_text("Score Fish: " + to_string(game.round.settlement.score_fish_captured), COLOR_BLACK, WINDOW_WIDTH / 2, 300, BODY_FONT_SIZE);
    draw_centered_text("Item Fish: " + to_string(game.round.settlement.item_fish_captured), COLOR_BLACK, WINDOW_WIDTH / 2, 325, BODY_FONT_SIZE);
    draw_centered_text("Final Score: " + to_string(game.round.settlement.final_score), COLOR_BLACK, WINDOW_WIDTH / 2, 350, BODY_FONT_SIZE);
    draw_centered_text("Coins Earned: " + to_string(game.round.settlement.coins_earned), COLOR_BLACK, WINDOW_WIDTH / 2, 375, BODY_FONT_SIZE);

    draw_button(game.game_over_menu.restart_button);
    draw_button(game.game_over_menu.menu_button);
}
