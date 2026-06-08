#include "model.h"

double clamp_double(double value, double min_value, double max_value){
    if(value < min_value){
        return min_value;
    }

    if(value > max_value){
        return max_value;
    }

    return value;
}



ball_data new_ball(){
    ball_data ball;
    ball.active = true;
    ball.pos = {WINDOW_WIDTH / 2, (WINDOW_HEIGHT + HUD_HEIGHT) / 2};
    double angle = rnd(0, 359);
    ball.speed = BALL_DEFAULT_SPEED;
    ball.velocity = vector_from_angle(angle, ball.speed);
    ball.radius = BALL_SIZE;
    ball.spin = 0;

    ball.animation.state = BALL_IDLE;
    ball.animation.playback = {0, 0, true};

    return ball;
}



bar_data new_bar(bar_orientation orientation, double fixed_position, double length, double thickness){
    bar_data bar;
    bar.orientation = orientation;
    bar.fixed_position = fixed_position;
    bar.speed = 0;
    if(orientation == HORIZONTAL_BAR){
        bar.bounds = {
            (WINDOW_WIDTH - length) / 2,
            fixed_position,
            length,
            thickness
        };

        bar.previous_position = bar.bounds.x;
    }
    else{
        bar.bounds = {
            fixed_position,
            (WINDOW_HEIGHT + HUD_HEIGHT - length) / 2,
            thickness,
            length
        };

        bar.previous_position = bar.bounds.y;
    }
    return bar;
}

bars_data new_bars(){
    bars_data bars;
    bars.length = BAR_DEFAULT_LENGTH;
    bars.thickness = BAR_THICKNESS;
    bars.margin = BAR_MARGIN;

    bars.top = new_bar(HORIZONTAL_BAR, BAR_MARGIN + HUD_HEIGHT, bars.length, bars.thickness);
    bars.bottom = new_bar(HORIZONTAL_BAR, WINDOW_HEIGHT - BAR_MARGIN - BAR_THICKNESS, bars.length, bars.thickness);
    bars.left = new_bar(VERTICAL_BAR, BAR_MARGIN, bars.length, bars.thickness);
    bars.right = new_bar(VERTICAL_BAR, WINDOW_WIDTH - BAR_MARGIN - BAR_THICKNESS, bars.length, bars.thickness);

    return bars;
}



water_pulse_data new_water_pulse(){
    water_pulse_data water_pulse;
    water_pulse.active = true;
    water_pulse.cooldown_end_time = 0;
    water_pulse.display_end_time = 0;

    water_pulse.impulse.falloff = NO_FALLOFF;
    water_pulse.impulse.force = PULSE_FORCE;
    water_pulse.impulse.range = PULSE_RADIUS;

    return water_pulse;
}



side_data random_side_info(){
    side_data side;
    int choice = rnd(0, 3);
    if(choice == 0){
        side.spawn = TOP_SIDE;
        side.opposite = BOTTOM_SIDE;
        side.left = RIGHT_SIDE;
        side.right = LEFT_SIDE;
    }
    else if(choice == 1){
        side.spawn = BOTTOM_SIDE;
        side.opposite = TOP_SIDE;
        side.left = LEFT_SIDE;
        side.right = RIGHT_SIDE;
    }
    else if(choice == 2){
        side.spawn = LEFT_SIDE;
        side.opposite = RIGHT_SIDE;
        side.left = TOP_SIDE;
        side.right = BOTTOM_SIDE;
    }
    else{
        side.spawn = RIGHT_SIDE;
        side.opposite = LEFT_SIDE;
        side.left = BOTTOM_SIDE;
        side.right = TOP_SIDE;
    }
    return side;
}

screen_side random_exit_side(const side_data& side_set){

    screen_side exit_side;
    int choice = rnd(1, 100);
    if(choice >= 1 && choice < 51){
        exit_side = side_set.opposite;
    }
    else if(choice >= 51 && choice < 71){
        exit_side = side_set.left;
    }
    else if (choice >= 71 && choice < 91){
        exit_side = side_set.right;
    }
    else{
        exit_side = side_set.spawn;
    }
    return exit_side;
}

point_2d random_point_on_side(screen_side side, double offset){
    point_2d point;
    if(side == TOP_SIDE){
        point = {(double)rnd(0, WINDOW_WIDTH), -offset};
    }
    else if(side == BOTTOM_SIDE){
        point = {(double)rnd(0, WINDOW_WIDTH), WINDOW_HEIGHT + offset};
    }
    else if(side == LEFT_SIDE){
        point = {-offset, (double)rnd(HUD_HEIGHT, WINDOW_HEIGHT)};
    }
    else{
        point = {WINDOW_WIDTH + offset, (double)rnd(HUD_HEIGHT, WINDOW_HEIGHT)};
    }
    return point;
}

vector_2d vector_from_to(point_2d from, point_2d to, double magnitude){
    if(from == to){
        return {0, 0};
    }

    vector_2d direction;
    direction.x = to.x - from.x;
    direction.y = to.y - from.y;
    direction = unit_vector(direction);

    vector_2d velocity;
    velocity.x = direction.x * magnitude;
    velocity.y = direction.y * magnitude;

    return velocity;

}

point_2d random_point_inside_screen(int margin){
    point_2d point;
    point = {
        (double)rnd(margin, WINDOW_WIDTH - margin),
        (double)rnd(margin + HUD_HEIGHT, WINDOW_HEIGHT - margin)
    };
    return point;
}

item_info score_item(){
    return {NO_ITEM, ITEM_NONE, 0, 10, 0, 0};
}
item_info add_life_item(){
    item_info item;
    item = {
        ADD_LIFE,
        INSTANT_ITEM,
        ADD_LIFE_REQUIRE_HIT,
        ADD_LIFE_SCORE,
        1,
        0
    };
    return item;
}
item_info extend_bar_item(){
    item_info item;
    item = {
        EXTEND_BAR,
        TIMED_ITEM,
        EXTEND_BAR_REQUIRE_HIT,
        EXTEND_BAR_SCORE,
        EXTEND_BAR_LENGTH,
        EXTEND_BAR_DURATION_MS
    };
    return item;
}
item_info slow_ball_item(){
    item_info item;
    item = {
        SLOW_BALL,
        TIMED_ITEM,
        SLOW_BALL_REQUIRE_HIT,
        SLOW_BALL_SCORE,
        SLOW_BALL_VALUE,
        SLOW_BALL_DURATION_MS
    };
    return item;
}

item_info get_random_item(){
    item_info item;
    int choice = rnd(1, 100);

    if(choice >= 1 && choice < 46){
        item = extend_bar_item();
    }
    else if(choice >= 46 && choice < 91){
        item = slow_ball_item();
    }
    else{
        item = add_life_item();
    }

    return item;
}

fish_data new_fish_with_item(item_info item){
    fish_data fish;
    side_data side = random_side_info();

    fish.active = true;

    fish.state = FISH_SWIMMING;
    fish.route = FISH_TO_TARGET;
    fish.item = item;

    fish.speed = FISH_SPEED;
    fish.velocity = vector_from_to(fish.pos, fish.target_point, fish.speed);
    fish.facing_left = fish.velocity.x < 0;
    fish.radius = fish.item.name == NO_ITEM ? SCORE_FISH_RADIUS : ITEM_FISH_RADIUS;

    fish.pos = random_point_on_side(side.spawn, fish.radius);
    fish.target_point = random_point_inside_screen(fish.radius * 4);
    fish.target_points_remaining = item.required_hits;
    fish.exit_point = random_point_on_side(random_exit_side(side), fish.radius);

    fish.fear_source = {0, 0};
    fish.flee_speed = FISH_FLEE_SPEED;
    fish.flee_radius = FISH_FEAR_RADIUS;

    fish.pause_timer = 0;
    fish.flee_timer = 0;
    fish.direction_change_timer = FISH_DIRECTION_CHANGE_PERIOD;

    fish.hit_count = 0;

    fish.animation.state = FISH_ANIM_SWIM;
    fish.animation.playback = {0, 0, true};
    fish.pending_remove = false;
    fish.reward_given = false;
        
    return fish;
}

fish_data new_score_fish(){
    return new_fish_with_item(score_item());
}

fish_data new_item_fish(){
    return new_fish_with_item(get_random_item());
}

fishes_data new_fishes(){
    fishes_data fishes;
    fishes.max_active_count = FISH_MAX_COUNT;
    fishes.next_spawn_time = rnd(FISH_MIN_SPAWN_INTERVAL_MS, FISH_MAX_SPAWN_INTERVAL_MS);
    return fishes;
}



torpedo_data new_torpedo(point_2d point){
    torpedo_data torpedo;

    torpedo.pos = point;
    torpedo.active = true;
    torpedo.state = TORPEDO_IDLE;

    torpedo.radius = (double)rnd(TORPEDO_MIN_RADIUS, TORPEDO_MAX_RADIUS);
    torpedo.trigger_radius = torpedo.radius + TORPEDO_TRIGGER_RANGE;

    torpedo.explosion_impulse.center = torpedo.pos;
    torpedo.explosion_impulse.range = torpedo.radius + TORPEDO_EXPLOSION_RANGE;
    torpedo.explosion_impulse.force = TORPEDO_PUSH_FORCE;
    torpedo.explosion_impulse.falloff = DISTANCE_FALLOFF;

    torpedo.damage = 1;
    torpedo.fish_hit_power = TORPEDO_FISH_HIT_POWER;

    torpedo.countdown_playback = {0, 0, true};
    torpedo.explosion_playback = {0, 0, true};
    
    torpedo.last_countdown_second = 0;

    return torpedo;
}

torpedoes_data new_torpedoes(){
    torpedoes_data torpedoes;
    torpedoes.max_active_count = 0;
    torpedoes.next_spawn_time = rnd(TORPEDO_MIN_SPAWN_INTERVAL_MS, TORPEDO_MAX_SPAWN_INTERVAL_MS);
    return torpedoes;
}



player_data new_player(){
    player_data player;
    player.score = 0;
    player.health = PLAYER_MAX_HEALTH;
    return player;
}

profile_data new_profile(){
    profile_data profile;
    profile.name = "Player";
    profile.best_score = 0;
    profile.total_coins = 0;
    profile.games_played = 0;
    profile.selected_character = 0;
    return profile;
}

profiles_data new_profiles(){
    profiles_data profiles;
    profiles.items.add(new_profile());
    profiles.current_index = 0;
    return profiles;
}

profile_data& current_profile(game_data& game){
    return game.profiles.items[game.profiles.current_index];
}

const profile_data& current_profile(const game_data& game){
    return game.profiles.items[game.profiles.current_index];
}



floating_score_data new_floating_score(point_2d pos, int score, long now){
    floating_score_data floating_score;
    floating_score.pos = pos;
    floating_score.score = score;
    floating_score.start_ms = now;
    floating_score.duration_ms = FLOAT_SCORE_DURATION_MS;
    return floating_score;
}



round_stats_data new_stats(){
    round_stats_data stats;
    stats.item_fish_captured = 0;
    stats.score_fish_captured = 0;
    stats.torpedoes_triggered = 0;
    return stats;
}

settlement_data new_settlement(){
    settlement_data settlement;
    settlement.completed = false;
    settlement.survival_time_ms = 0;
    settlement.final_score = 0;
    settlement.score_fish_captured = 0;
    settlement.item_fish_captured = 0;
    settlement.coins_earned = 0;
    return settlement;
}

leaderboard_entry_data new_leaderboard_entry(const game_data& game){
    leaderboard_entry_data entry;
    entry.player_name = current_profile(game).name;
    entry.score = game.round.settlement.final_score;
    entry.survival_time_ms = game.round.settlement.survival_time_ms;
    return entry;
}



button_data new_button(double x, double y, double width, double height, string text){
    button_data button;
    button.bounds = {x, y, width, height};
    button.text = text;
    button.hovered = false;
    button.clicked = false;
    button.was_hovered = false;
    return button;
}

profile_select_menu_data new_profile_select_menu(){
    profile_select_menu_data menu;
    menu.first_visible_index = 0;

    menu.create_button = new_button(390, 470, 300, 50, "Create Profile");
    menu.continue_button = new_button(390, 535, 300, 55, "Continue");

    return menu;
}

profile_create_menu_data new_profile_create_menu(){
    profile_create_menu_data menu;
    menu.typed_name = "";
    menu.create_button = new_button(390, 380, 300, 55, "Create");
    menu.back_button = new_button(390, 450, 300, 50, "Back");
    return menu;
}


main_menu_data new_main_menu(){
    main_menu_data menu;
    menu.start_button = new_button(385, 505, 310, 64, "Start Game");
    menu.info_button = new_button(715, 512, 50, 50, "i");
    menu.left_character_button = new_button(280, 270, 70, 100, "<");
    menu.right_character_button = new_button(730, 270, 70, 100, ">");
    menu.unlock_button = new_button(430, 425, 220, 44, "Unlock 500");
    menu.leaderboard_button = new_button(820, 550, 190, 95, "Rank");
    menu.quit_button = new_button(955, 24, 80, 34, "Quit");
    menu.switch_profile_button = new_button(210, 58, 90, 28, "Switch");
    return menu;
}


instructions_menu_data new_instructions_menu(){
    instructions_menu_data menu;
    menu.back_button = new_button(430, 530, 220, 50, "Back");
    return menu;
}


pause_menu_data new_pause_menu(){
    pause_menu_data menu;
    menu.resume_button = new_button(430, 310, 220, 50, "Resume");
    menu.restart_button = new_button(430, 375, 220, 50, "Restart");
    menu.menu_button = new_button(430, 440, 220, 50, "Main Menu");
    return menu;
}


leaderboard_menu_data new_leaderboard_menu(){
    leaderboard_menu_data menu;
    menu.back_button = new_button(430, 620, 220, 50, "Back");
    return menu;
}


game_over_menu_data new_game_over_menu(){
    game_over_menu_data menu;
    menu.restart_button = new_button(430, 400, 220, 50, "Restart");
    menu.menu_button = new_button(430, 465, 220, 50, "Main Menu");
    return menu;
}


round_data new_round(){
    round_data round;
    round.player = new_player();
    
    round.ball = new_ball();
    round.bars = new_bars();
    round.water_pulse = new_water_pulse();
    round.fishes = new_fishes();
    round.torpedoes = new_torpedoes();

    round.clock.last_update_ms = current_ticks();
    round.clock.time_passed_ms = 0;

    round.stats = new_stats();
    round.settlement = new_settlement();

    round.screen_shake.start_ms = 0;
    round.screen_shake.end_ms = 0;
    round.screen_shake.strength = 0;
    return round;
}

game_data new_game(){
    game_data game;
    game.should_quit = false;
    game.current_screen = PROFILE_SELECT_SCREEN;

    game.round = new_round();

    game.profile_select_menu = new_profile_select_menu();
    game.profile_create_menu = new_profile_create_menu();
    game.main_menu = new_main_menu();
    game.instructions_menu = new_instructions_menu();
    game.pause_menu = new_pause_menu();
    game.leaderboard_menu = new_leaderboard_menu();
    game.game_over_menu = new_game_over_menu();

    game.profiles = new_profiles();

    game.debug.on = false;
    game.debug.show_collision = false;
    game.debug.window_collision = false;
    return game;
}
