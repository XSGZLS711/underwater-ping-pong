#include "view_internal.h"

//-------------------- View runtime animation state --------------------

animation ball_idle_animation;
animation ball_eat_animation;
animation ball_hurt_animation;
animation ball_stunned_animation;
animation ball_boost_animation;

ball_animation_state last_ball_view_state = BALL_IDLE;
long last_ball_animation_start_time = -1;

void create_game_visuals(){
    load_animation_script("player_shark_animation", "player_shark.txt");

    ball_idle_animation = create_animation("player_shark_animation", "idle");
    ball_eat_animation = create_animation("player_shark_animation", "eat");
    ball_hurt_animation = create_animation("player_shark_animation", "hurt");
    ball_stunned_animation = create_animation("player_shark_animation", "stunned");
    ball_boost_animation = create_animation("player_shark_animation", "boost");
}

//-------------------- Background rendering --------------------

void draw_underwater_background(long now){
    clear_screen(COLOR_WHITE);
    int frame = loop_animation_frame(now, 0, BACKGROUND_ANIMATION);
    draw_sheet_frame(
        BACKGROUND_SHEET,
        frame,
        WINDOW_WIDTH / 2,
        WINDOW_HEIGHT / 2,
        1,
        1
    );
}


//-------------------- Ball rendering --------------------

double shark_tilt_angle(vector_2d velocity){
    if(vector_magnitude(velocity) == 0){
        return 0;
    }

    double angle = vector_angle(velocity);

    if(angle > 180){
        angle -= 360;
    }

    if(velocity.x < 0){
        if(angle >= 0){
            angle -= 180;
        }
        else{
            angle += 180;
        }
    }

    return clamp_double(angle, -25, 25);
}

struct ball_animation_view_data{
    string bitmap_name;
    animation anim;
};

ball_animation_view_data ball_animation_view(ball_animation_state state){
    switch(state){
        case BALL_EAT: return {"player_shark_sheet", ball_eat_animation};
        case BALL_HURT: return {"player_shark_sheet", ball_hurt_animation};
        case BALL_STUNNED: return {"player_shark_sheet", ball_stunned_animation};
        case BALL_BOOST: return {"player_shark_sheet", ball_boost_animation};
        case BALL_IDLE:
        default: return {"player_shark_sheet", ball_idle_animation};
    }
}

void draw_ball(const ball_data& ball, bool advance_animation, bool debug){
    double image_size = 256;
    double scale = 0.25;

    bool facing_left = ball.velocity.x < 0;
    double angle = shark_tilt_angle(ball.velocity);

    double x = ball.pos.x - image_size / 2;
    double y = ball.pos.y - image_size / 2;

    ball_animation_state state = ball.animation.state;

    ball_animation_view_data view = ball_animation_view(state);
    animation current_animation = view.anim;
    string bitmap_name = view.bitmap_name;

    if(state != BALL_IDLE){
        //动画类型不同 或 动画开始时间不同
        if(state != last_ball_view_state || ball.animation.playback.start_time != last_ball_animation_start_time){
            restart_animation(current_animation);
            last_ball_view_state = state;
            last_ball_animation_start_time = ball.animation.playback.start_time;
        }
    }
    else{
        last_ball_view_state = BALL_IDLE;
    }

    drawing_options opts = option_with_animation(current_animation);

    if(facing_left){
        opts = option_flip_y(opts);
    }

    opts = option_scale_bmp(scale, scale, opts);
    opts = option_rotate_bmp(angle, opts);
    draw_bitmap(bitmap_name, x, y, opts);

    if(advance_animation){
        update_animation(current_animation);
    }

    if(debug){
        draw_circle(COLOR_RED, ball.pos, ball.radius);
    }
}


//-------------------- Bar rendering --------------------
void draw_bar(const bar_data& bar, long now, int& bar_index, bool debug){
    rectangle r = bar.bounds;

    double visual_length;
    double visual_thickness;
    double scale_x;
    double scale_y;

    double center_x = r.x + r.width / 2;
    double center_y = r.y + r.height / 2;
    
    int frame = loop_animation_frame(now, bar_index, BAR_ANIMATION);
    
    if(bar.orientation == HORIZONTAL_BAR){
        visual_length = r.width + 20;
        visual_thickness = r.height * 2.2;
        scale_x = visual_length / BAR_SHEET.frame_width;
        scale_y = visual_thickness / BAR_SHEET.frame_height;
        draw_sheet_frame(BAR_SHEET, frame, center_x, center_y, scale_x, scale_y);
    }
    else{
        visual_length = r.height + 20;
        visual_thickness = r.width * 2.2;
        scale_x = visual_length / BAR_SHEET.frame_width;
        scale_y = visual_thickness / BAR_SHEET.frame_height;
        draw_sheet_frame(BAR_SHEET, frame, center_x, center_y, scale_x, scale_y, 90);
    }
    bar_index++;
    if(debug){
        draw_rectangle(COLOR_GRAY, bar.bounds);
    }
}

void draw_bars(const game_data& game){
    long now = game.round.clock.time_passed_ms;
    int bar_index = 0;
    draw_bar(game.round.bars.top, now, bar_index, game.debug.show_collision);
    draw_bar(game.round.bars.bottom, now, bar_index, game.debug.show_collision);
    draw_bar(game.round.bars.left, now, bar_index, game.debug.show_collision);
    draw_bar(game.round.bars.right, now, bar_index, game.debug.show_collision);
}


//-------------------- Water pulse rendering --------------------

void draw_water_pulse(const round_data& round){
    if(round.clock.time_passed_ms < round.water_pulse.display_end_time){
        fill_circle(COLOR_SKY_BLUE, round.water_pulse.impulse.center, round.water_pulse.impulse.range);
    }
}


//-------------------- Fish rendering --------------------

//根据道具和状态确定动画
fish_animation_info fish_animation_for(item_name item, fish_animation_state state){
    if(item == NO_ITEM && state == FISH_ANIM_HURT){
        state = FISH_ANIM_SWIM;
    }

    for(int i = 0; i < FISH_ANIMATION_COUNT; i++){
        if(FISH_ANIMATIONS[i].item == item && FISH_ANIMATIONS[i].state == state){
            return FISH_ANIMATIONS[i];
        }
    }

    return FISH_ANIMATIONS[0];
}
animation_view_data fish_animation_view(item_name item, fish_animation_state state){
    fish_animation_info info = fish_animation_for(item, state);

    return {
        info.sheet,
        info.row,
        info.frame_ms,
        info.scale,
        info.sheet.frame_count,
        info.loop
    };
}

loop_animation_info icon_animation_for(item_name name){
    loop_animation_info info;
    switch(name){
        case ADD_LIFE: info = ICON_HEALTH_ANIMATION; break;
        case EXTEND_BAR: info = ICON_EXTEND_BAR_ANIMATION; break;
        case SLOW_BALL: info = ICON_SLOW_BALL_ANIMATION; break;
        case NO_ITEM:
        default: break;
    }
    return info;
}

void draw_item_icon(item_name name, double x, double y, int index, double scale, long now){
    loop_animation_info icon = icon_animation_for(name);
    draw_idle_icon(icon, x, y, index, scale, now);
}

void draw_fish_item_bubble(const fish_data& fish, long now){
    if(fish.item.type == ITEM_NONE || fish.pending_remove){
        return;
    }
    double bubble_x = fish.pos.x;
    double bubble_y = fish.pos.y - 34;
    //画气泡
    draw_idle_icon(ICON_BUBBLE_ANIMATION, bubble_x, bubble_y, 0, 0.72, now);
    //画道具图标
    draw_item_icon(fish.item.name, bubble_x, bubble_y, 0, 0.42, now);

    //画所需撞击次数
    double badge_x = bubble_x + 13;
    double badge_y = bubble_y + 13;
    int remaining_hits = fish.item.required_hits - fish.hit_count;
    if(remaining_hits < 0){
        remaining_hits = 0;
    }
    fill_circle(rgba_color(0, 0, 0, 160), badge_x, badge_y, 9);
    draw_circle(COLOR_WHITE, badge_x, badge_y, 9);
    draw_centered_text(to_string(remaining_hits), COLOR_WHITE, badge_x, badge_y - 7, 14);
}

void draw_fish(const fish_data& fish, long now, bool debug){
    if(!fish.active){
        return;
    }

    animation_view_data view = fish_animation_view(fish.item.name, fish.animation.state);
    int frame = animation_frame(fish.animation.playback, view, now);

    //画鱼本体
    draw_sheet_frame(view.sheet, frame, fish.pos.x, fish.pos.y, view.scale, view.scale, 0, fish.facing_left);
    
    //画道具气泡
    draw_fish_item_bubble(fish, now);

    if(debug){
        if(fish.item.name == NO_ITEM){
            draw_circle(COLOR_ORANGE, fish.pos, fish.radius);
        }
        else if(fish.item.name == EXTEND_BAR){
            draw_circle(COLOR_GREEN, fish.pos, fish.radius);
        }
        else if(fish.item.name == ADD_LIFE){
            draw_circle(COLOR_PINK, fish.pos, fish.radius);
        }
        else if(fish.item.name == SLOW_BALL){
            draw_circle(COLOR_BLUE, fish.pos, fish.radius);
        }
    }
}

void draw_fishes(const game_data& game){
    for(int i = 0; i < game.round.fishes.items.length(); i++){
        draw_fish(game.round.fishes.items[i], game.round.clock.time_passed_ms, game.debug.show_collision);
    }
}


//-------------------- Torpedo rendering --------------------

torpedo_animation_info torpedo_animation_for(torpedo_part part, torpedo_state state){
    for(int i = 0; i < TORPEDO_ANIMATION_COUNT; i++){
        if(TORPEDO_ANIMATIONS[i].state == state && TORPEDO_ANIMATIONS[i].part == part){
            return TORPEDO_ANIMATIONS[i];
        }
    }

    return TORPEDO_ANIMATIONS[0];
}
animation_view_data torpedo_animation_view(torpedo_part part, torpedo_state state){
    torpedo_animation_info info = torpedo_animation_for(part, state);

    return {
        info.sheet,
        info.row,
        info.frame_ms,
        info.scale,
        info.sheet.frame_count,
        info.loop
    };
}


void draw_torpedo_field(const torpedo_data& torpedo, long now){
    animation_view_data view = torpedo_animation_view(TORPEDO_FIELD, torpedo.state);
    view.scale = torpedo.trigger_radius * 2 / view.sheet.frame_width;

    int frame = animation_frame(torpedo.countdown_playback, view, now);
    draw_sheet_frame(view.sheet, frame, torpedo.pos.x, torpedo.pos.y, view.scale, view.scale);
}
void draw_torpedo_body(const torpedo_data& torpedo, long now){
    animation_view_data view = torpedo_animation_view(TORPEDO_BODY, torpedo.state);
    view.scale = torpedo.radius * 2 / view.sheet.frame_width;
    int frame = animation_frame(torpedo.countdown_playback, view, now);
    draw_sheet_frame(view.sheet, frame, torpedo.pos.x, torpedo.pos.y, view.scale, view.scale);
}
void draw_torpedo_explosion(const torpedo_data& torpedo, long now){
    animation_view_data view = torpedo_animation_view(TORPEDO_EXPLOSION, TORPEDO_EXPLODING);
    view.scale = torpedo.explosion_impulse.range * 2 / view.sheet.frame_width;

    int frame = animation_frame(torpedo.explosion_playback, view, now);

    draw_sheet_frame(view.sheet, frame, torpedo.pos.x, torpedo.pos.y, view.scale, view.scale);

}
void draw_torpedo_countdown(const torpedo_data& torpedo, long now){
    if(torpedo.state != TORPEDO_COUNTDOWN){
        return;
    }
    long remaining = torpedo.countdown_playback.end_time - now;
    int seconds = (remaining + 999) / 1000;

    fill_circle(rgba_color(0, 0, 0, 80), torpedo.pos.x, torpedo.pos.y, torpedo.radius - 4);
    draw_centered_text(to_string(seconds), COLOR_WHITE, torpedo.pos.x, torpedo.pos.y - 9, 20);
}
void draw_torpedo(const torpedo_data& torpedo, long now, bool debug){
    if(torpedo.state == TORPEDO_EXPLODING){
        draw_torpedo_explosion(torpedo, now);
    }
    else{
        draw_torpedo_field(torpedo, now);
        draw_torpedo_body(torpedo, now);
        draw_torpedo_countdown(torpedo, now);
    }

    if(debug){
        draw_circle(COLOR_BLACK, torpedo.pos, torpedo.explosion_impulse.range);
        draw_circle(COLOR_BLACK, torpedo.pos, torpedo.trigger_radius);
    }
}
void draw_torpedos(const game_data& game){
    for(int i = 0; i < game.round.torpedoes.items.length(); i++){
        draw_torpedo(game.round.torpedoes.items[i], game.round.clock.time_passed_ms, game.debug.show_collision);
    }
}

//-------------------- Floating score rendering --------------------

void draw_floating_scores(const round_data& round){
    long now = round.clock.time_passed_ms;
    for(int i = 0; i < round.floating_scores.length(); i++){
        const floating_score_data& float_score = round.floating_scores[i];
        long elapsed = now - float_score.start_ms;
        double progress = (double)elapsed / float_score.duration_ms;

        int alpha = 255 * (1 - progress);

        color text_color = rgba_color(255, 245, 120, alpha);
        draw_centered_text(
            "+" + to_string(float_score.score),
            text_color,
            float_score.pos.x,
            float_score.pos.y,
            20
        );
    }
}
