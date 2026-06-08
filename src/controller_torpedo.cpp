#include "controller_internal.h"

//-------------------- Torpedo update and explosion --------------------

double angle_difference(double angle1, double angle2){
    double difference = angle1 - angle2;

    if(difference < 0){
        difference = -difference;
    }
    if(difference > 180){
        difference = 360 - difference;
    }

    return difference;
}

bool safe_torpedo_position(point_2d point, const ball_data& ball){
    double torpedo_angle = vector_angle(vector_from_to(ball.pos, point, 1));
    double ball_angle = vector_angle(ball.velocity);
    bool safe_angle = angle_difference(ball_angle, torpedo_angle) >= TORPEDO_SAFE_SPAWN_ANGLE;
    
    double distance = point_point_distance(point, ball.pos);
    bool safe_distance = distance >= TORPEDO_SAFE_SPAWN_DISTANCE;

    return safe_angle && safe_distance;
}

point_2d random_torpedo_spawn_position(const ball_data& ball){
    point_2d point;

    for(int i = 0; i < TORPEDO_SPAWN_ATTEMPTS; i++){
        point = random_point_inside_screen(TORPEDO_MAX_RADIUS * 4);

        if(safe_torpedo_position(point, ball)){
            return point;
        }
    }
    return point;
}

bool ball_inside_range(const torpedo_data& torpedo, const ball_data& ball, double range){
    return circles_intersect(ball.pos.x, ball.pos.y, ball.radius, torpedo.pos.x, torpedo.pos.y, range);
}

bool fish_inside_explosion_range(const torpedo_data& torpedo, const fish_data& fish){
    return circles_intersect(fish.pos.x, fish.pos.y, fish.radius, torpedo.pos.x, torpedo.pos.y, torpedo.explosion_impulse.range);
}

void start_screen_shake(round_data& round, double strength, long duration_ms){
    long now = round.clock.time_passed_ms;

    round.screen_shake.start_ms = now;
    round.screen_shake.end_ms = now + duration_ms;
    round.screen_shake.strength = strength;
}
//小球及玩家受爆炸的影响
void apply_torpedo_explosion_to_ball(game_data& game, const torpedo_data& torpedo){
    if(!ball_inside_range(torpedo, game.round.ball, torpedo.explosion_impulse.range)){
        return;
    }
    game.round.player.health -= torpedo.damage;
    if(game.round.player.health <= 0){
        game.round.player.health = 0;
        return;
    }

    start_screen_shake(game.round, SCREEN_SHAKE_STRENGTH, SCREEN_SHATE_DURATION);

    apply_impulse_to_ball(game.round.ball, torpedo.explosion_impulse);

    if(vector_magnitude(game.round.ball.velocity) <= 3){
        try_play_ball_animation(game.round.ball, BALL_STUNNED, game.round.clock.time_passed_ms);
    }
    else{
        try_play_ball_animation(game.round.ball, BALL_HURT, game.round.clock.time_passed_ms);
    }
}

void apply_torpedo_explosion_to_fishes(round_data& round, const torpedo_data& torpedo){
    for(int i = round.fishes.items.length() - 1; i >= 0; i--){
        fish_data& fish = round.fishes.items[i];
        if(!fish_inside_explosion_range(torpedo, fish)){
            continue;
        }
        bool captured = apply_hit_to_fish(fish, torpedo.fish_hit_power);
        if(captured){
            capture_fish(round, i);
            try_play_fish_animation(fish.animation, FISH_ANIM_EXPLODED, round.clock.time_passed_ms);
        }
        else{
            try_play_fish_animation(fish.animation, FISH_ANIM_HURT, round.clock.time_passed_ms);
            play_sound_effect("fish_hurt", 0.5);
        }
    }
}

void torpedo_start_explosion(game_data& game, torpedo_data& torpedo){
    torpedo.state = TORPEDO_EXPLODING;
    torpedo.explosion_playback.start_time = game.round.clock.time_passed_ms;
    torpedo.explosion_playback.end_time = game.round.clock.time_passed_ms + TORPEDO_EXPLOSION_DISPLAY_MS;

    apply_torpedo_explosion_to_ball(game, torpedo);
    apply_torpedo_explosion_to_fishes(game.round, torpedo);

    play_sound_effect("torpedo_explode", 0.3);
}

int torpedo_countdown_second(const torpedo_data& torpedo, long now){
    long reamining = torpedo.countdown_playback.end_time - now;
    return (reamining + 999) / 1000;
}

void update_torpedo(game_data& game, torpedo_data& torpedo){
    long now = game.round.clock.time_passed_ms;
    //小球进入检测范围开始倒数
    if(torpedo.state == TORPEDO_IDLE && ball_inside_range(torpedo, game.round.ball, torpedo.trigger_radius)){
        torpedo.state = TORPEDO_COUNTDOWN;
        torpedo.countdown_playback.start_time = now;
        torpedo.countdown_playback.end_time = now + TORPEDO_COUNTDOWN_MS;

        torpedo.last_countdown_second = torpedo_countdown_second(torpedo, now);
        play_sound_effect("torpedo_beep", 0.4);
    }
    //小球碰到鱼雷就爆炸
    if(torpedo.state != TORPEDO_EXPLODING && ball_inside_range(torpedo, game.round.ball, torpedo.radius)){
        torpedo_start_explosion(game, torpedo);
    }
    //进入倒计时
    if(torpedo.state == TORPEDO_COUNTDOWN){
        int current_second = torpedo_countdown_second(torpedo, now);
        //倒计时数变化
        if(current_second > 0 && current_second != torpedo.last_countdown_second){
            torpedo.last_countdown_second = current_second;
            play_sound_effect("torpedo_beep", 0.4);
        }
        //倒计时结束爆炸
        if(now >= torpedo.countdown_playback.end_time){
            torpedo_start_explosion(game, torpedo);
        }
    }
}

void update_torpedoes(game_data& game){
    dynamic_array<torpedo_data>& items = game.round.torpedoes.items;
    long now = game.round.clock.time_passed_ms;

    for(int i = 0; i < items.length(); i++){
        update_torpedo(game, items[i]);
    }
    //删除已经爆炸完的鱼雷
    for(int i = items.length() - 1; i >= 0; i--){
        if(items[i].state == TORPEDO_EXPLODING && now > items[i].explosion_playback.end_time){
            items.remove(i);
        }
    }
    //鱼雷数量不够且间隔一定时间再生成鱼雷
    if(items.length() < game.round.torpedoes.max_active_count && now >= game.round.torpedoes.next_spawn_time){
        items.add(new_torpedo(random_torpedo_spawn_position(game.round.ball)));
        //随机生成间隔
        game.round.torpedoes.next_spawn_time = now + rnd(TORPEDO_MIN_SPAWN_INTERVAL_MS, TORPEDO_MAX_SPAWN_INTERVAL_MS);
    }
}
