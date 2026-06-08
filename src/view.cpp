#include "view_internal.h"

//-------------------- Generic drawing helpers --------------------

//根据当前时间计算应该播放sheet里哪个frame
int loop_animation_frame(long time_ms, int icon_index, loop_animation_info animation){
    long local_time = (time_ms - icon_index * animation.interval_ms) % animation.cycle_ms;

    if(local_time < 0){
        local_time += animation.cycle_ms;
    }
    if(local_time >= animation.sparkle_ms){
        return 0;
    }

    int frame = local_time / animation.frame_ms;
    return frame;
}
int animation_frame(const animation_playback& playback, const animation_view_data& view, long now){
    long elapsed = now - playback.start_time;
    int frame = elapsed / view.frame_ms;

    if(view.loop){
        frame = frame % view.frame_count;
    }
    else if(frame >= view.frame_count){
        frame = view.frame_count - 1;
    }

    return frame + view.row * view.sheet.columns;
}
//画sheet里特定frame
void draw_sheet_frame(const sheet_info& sheet, int frame, double center_x, double center_y, double scale_x, double scale_y, double angle, bool flip_y){
    int column = frame % sheet.columns;
    int row = frame / sheet.columns;

    rectangle part = {
        column * sheet.frame_width,
        row * sheet.frame_height,
        sheet.frame_width,
        sheet.frame_height
    };

    double x = center_x - sheet.frame_width / 2;
    double y = center_y - sheet.frame_height / 2;

    drawing_options opts = option_part_bmp(part);

    if(flip_y){
        opts = option_flip_y(opts);
    }
    opts = option_rotate_bmp(angle, opts);
    opts = option_scale_bmp(scale_x, scale_y, opts);

    draw_bitmap(sheet.bitmap_name, x, y, opts);
}

void draw_idle_icon(const loop_animation_info& icon, double center_x, double center_y, int icon_index, double scale, long now, bool flip_y){
    int frame = loop_animation_frame(now, icon_index, icon);

    if(frame >= icon.sheet.frame_count){
        frame = icon.sheet.frame_count - 1;
    }

    draw_sheet_frame(
        icon.sheet,
        frame,
        center_x,
        center_y,
        scale,
        scale,
        0,
        flip_y
    );
}

point_2d screen_shake_offset(const screen_shake_data& shake, long now){
    if(now >= shake.end_ms){
        return {0, 0};
    }

    long duration = shake.end_ms - shake.start_ms;
    long elapsed = now - shake.start_ms;

    double progress = (double)elapsed / duration;
    double fade = 1.0 - progress;
    double current_strength = shake.strength * fade * fade;

    double x = sin(now * 0.07) * current_strength;
    double y = cos(now * 0.11) * current_strength;

    return {x, y};
}

void draw_centered_text(const string& text, color text_color, double center_x, double y, int font_size){
    font ui_font = get_system_font();
    double x = center_x - text_width(text, ui_font, font_size) / 2;
    draw_text(text, text_color, ui_font, font_size, x, y);
}

//-------------------- Main draw flow --------------------

void draw_playing(const game_data& game, bool advance_animation){
    clear_screen(COLOR_WHITE);
    point_2d shake = screen_shake_offset(game.round.screen_shake, game.round.clock.time_passed_ms);
    move_camera_to(-shake.x, -shake.y);

    draw_underwater_background(game.round.clock.time_passed_ms);
    draw_water_pulse(game.round);
    draw_fishes(game);
    draw_ball(game.round.ball, advance_animation, game.debug.show_collision);
    draw_floating_scores(game.round);
    draw_bars(game);
    draw_torpedos(game);

    move_camera_to(0, 0);

    draw_hud(game.round);
}

void draw_game(const game_data& game){
    switch (game.current_screen){
        case PROFILE_SELECT_SCREEN: draw_profile_select(game); break;
        case PROFILE_CREATE_SCREEN: draw_profile_create(game); break;
        case MENU_SCREEN: draw_menu(game); break;
        case INSTRUCTIONS_SCREEN: draw_instructions(game); break;
        case LEADERBOARD_SCREEN: draw_leaderboard(game); break;
        case PLAYING_SCREEN: draw_playing(game, true); break;
        case PAUSE_SCREEN: draw_playing(game, false); draw_pause_overlay(game); break;
        case GAME_OVER_SCREEN: draw_playing(game, false); draw_game_over_overlay(game); break;
    }
    refresh_screen(60);
}
