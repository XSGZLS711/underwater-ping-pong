#include "controller_internal.h"

//-------------------- Fish movement and spawning --------------------

animation_rule fish_animation_rule_for_state(fish_animation_state state){
    switch(state){
        case FISH_ANIM_CAPTURED: return {4, 480, false};
        case FISH_ANIM_EXPLODED: return {3, 600, false};
        case FISH_ANIM_HURT: return {2, 600, false};
        case FISH_ANIM_FLEE: return {1, 0, true};
        case FISH_ANIM_SWIM: return {0, 0, true};
        default: return {0, 0, true};
    }
}

void try_play_fish_animation(fish_animation_data& animation, fish_animation_state state, long now){
    animation_rule current_rule = fish_animation_rule_for_state(animation.state);
    animation_rule new_rule = fish_animation_rule_for_state(state);

    bool current_finished = !animation.playback.playing || now > animation.playback.end_time;
    bool can_replace = current_finished || new_rule.priority >= current_rule.priority;

    if(can_replace){
        animation.state = state;
        animation.playback.start_time = now;
        animation.playback.playing = true;

        if(new_rule.loop){
            animation.playback.end_time = 0;
        }
        else{
            animation.playback.end_time = now + new_rule.duration_ms;
        }
    }
}
//到达target_point后更新路线
void update_fish_route(fish_data& fish){
    fish.target_points_remaining--;
    if(fish.target_points_remaining > 0){
        fish.target_point = random_point_inside_screen(fish.radius * 4);
    }
    else{
        fish.route = FISH_TO_EXIT;
        fish.target_point = fish.exit_point;
    }

    fish.state = FISH_PAUSED;
    fish.pause_timer = rnd(FISH_MIN_PAUSE_FRAMES, FISH_MAX_PAUSE_FRAMES);
    fish.velocity = {0, 0};
}
//将鱼拉向目标点
void steer_fish_toward_target(fish_data& fish){
    vector_2d target_velocity = vector_from_to(fish.pos, fish.target_point, fish.speed);
    fish.velocity.x = target_velocity.x * FISH_TARGET_PUSH_RATE + fish.velocity.x * (1 - FISH_TARGET_PUSH_RATE);
    fish.velocity.y = target_velocity.y * FISH_TARGET_PUSH_RATE + fish.velocity.y * (1 - FISH_TARGET_PUSH_RATE);
    
    vector_2d direction = unit_vector(fish.velocity);
    fish.velocity.x = direction.x * fish.speed;
    fish.velocity.y = direction.y * fish.speed;
}
//随机轻微偏转鱼的运动方向
void random_turn_fish(fish_data& fish){
    fish.direction_change_timer--;
    if(fish.direction_change_timer <= 0){
        int angle = rnd(-FISH_MAX_TURN_ANGLE, FISH_MAX_TURN_ANGLE);
        int new_angle = angle + vector_angle(fish.velocity);
        fish.velocity = vector_from_angle(new_angle, fish.speed);

        fish.direction_change_timer = FISH_DIRECTION_CHANGE_PERIOD;
    }
}

void update_fish_facing(fish_data& fish){
    if(abs(fish.velocity.x) > 0.1){
        fish.facing_left = fish.velocity.x < 0;
    }
}

void update_fish_pause(fish_data& fish){
    fish.pause_timer--;
    if(fish.pause_timer <= 0){
        fish.state = FISH_SWIMMING;
        fish.velocity = vector_from_to(fish.pos, fish.target_point, fish.speed);
        update_fish_facing(fish);
    }
}

void start_fish_flee(fish_data& fish, point_2d fear_source, long now){
    fish.state = FISH_FLEEING;
    fish.flee_timer = FISH_FLEE_FRAMES;
    fish.velocity = vector_from_to(fear_source, fish.pos, fish.flee_speed);

    try_play_fish_animation(fish.animation, FISH_ANIM_FLEE, now);
}

void update_fish_flee(fish_data& fish, long now){
    fish.flee_timer--;

    fish.pos.x += fish.velocity.x;
    fish.pos.y += fish.velocity.y;

    update_fish_facing(fish);
    if(fish.flee_timer <= 0){
        fish.state = FISH_SWIMMING;
        fish.velocity = vector_from_to(fish.pos, fish.target_point, fish.speed);
        fish.direction_change_timer = FISH_DIRECTION_CHANGE_PERIOD;
        try_play_fish_animation(fish.animation, FISH_ANIM_SWIM, now);
    }
}

void update_fish_animation_state(fish_data& fish, long now){
    if(fish.pending_remove){
        return;
    }
    if(fish.animation.playback.end_time == 0){
        return;
    }
    if(now < fish.animation.playback.end_time){
        return;
    }
    if(fish.state == FISH_FLEEING){
        try_play_fish_animation(fish.animation, FISH_ANIM_FLEE, now);
    }
    else{
        try_play_fish_animation(fish.animation, FISH_ANIM_SWIM, now);
    }
}

void update_fish(fish_data& fish, long now){
    if(!fish.active){
        return;
    }

    if(fish.pending_remove){
        return;
    }
    update_fish_animation_state(fish, now);

    if(fish.state == FISH_FLEEING){
        update_fish_flee(fish, now);
        return;
    }
    if(fish.state == FISH_PAUSED){
        update_fish_pause(fish);
        return;
    }
    //如果到达target_point
    if(fish.route == FISH_TO_TARGET && point_point_distance(fish.pos, fish.target_point) < fish.radius){
        update_fish_route(fish);
        return;
    }
    
    steer_fish_toward_target(fish);

    random_turn_fish(fish);

    fish.pos.x += fish.velocity.x;
    fish.pos.y += fish.velocity.y;
    update_fish_facing(fish);
}

bool fish_off_screen(const fish_data& fish){
    double margin = fish.radius * 2;
    return fish.pos.x < -margin || fish.pos.x > WINDOW_WIDTH + margin ||
           fish.pos.y < -margin || fish.pos.y > WINDOW_HEIGHT + margin;
}

void update_fishes(round_data& round){
    dynamic_array<fish_data>& items = round.fishes.items;
    long now = round.clock.time_passed_ms;

    for(int i = 0; i < items.length(); i++){
        update_fish(items[i], now);
    }
    //移除消失或被捕获的鱼
    for(int i = items.length() - 1; i >= 0; i--){
        if(fish_off_screen(items[i]) || 
        (items[i].pending_remove && now >= items[i].animation.playback.end_time))
        {
            items.remove(i);
        }
    }
    //数量不足且间隔一定时间补充鱼
    if(items.length() < round.fishes.max_active_count && now >= round.fishes.next_spawn_time){
        int item_count = 0;
        for(int i = 0; i < items.length(); i++){
            if(items[i].item.name != NO_ITEM){
                item_count++;
            }
        }

        if(item_count < ITEM_FISH_MAX_COUNT && rnd(1, 100) <= ITEM_FISH_SPAWN_CHANCE){
            items.add(new_item_fish());
        }
        else{
            items.add(new_score_fish());
        }
        round.fishes.next_spawn_time = now + rnd(FISH_MIN_SPAWN_INTERVAL_MS, FISH_MAX_SPAWN_INTERVAL_MS);
    }
}


//-------------------- Fish collision and capture --------------------

void trigger_fish_panic(dynamic_array<fish_data>& fishes, point_2d fear_source, long now){
    for(int i = 0; i < fishes.length(); i++){
        if(point_point_distance(fishes[i].pos, fear_source) <= fishes[i].flee_radius){
            start_fish_flee(fishes[i], fear_source, now);
        }
    }
}

bool ball_hits_fish(const ball_data& ball, const fish_data& fish){
    return(circles_intersect(ball.pos.x, ball.pos.y, ball.radius, fish.pos.x, fish.pos.y, fish.radius));
}
//根据hit力道判断鱼是否被捕获
bool apply_hit_to_fish(fish_data& fish, int hit_power){
    bool captured = false;
    if(fish.item.required_hits == 0){
        captured = true;
    }
    else{
        fish.hit_count += hit_power;
        if(fish.hit_count >= fish.item.required_hits){
            captured = true;
        }
    }
    return captured;
}
//记录获得的道具效果
void apply_item_effect(round_data& round, const item_info& item){
    if(item.type == ITEM_NONE){
        round.stats.score_fish_captured++;
        return;
    }
    round.stats.item_fish_captured++;
    if(item.type == INSTANT_ITEM){
        if(item.name == ADD_LIFE){
            if(round.player.health < PLAYER_MAX_HEALTH){
                round.player.health++;
            }
        }
        return;
    }
    if(item.type == TIMED_ITEM){
        active_effect_data effect;
        effect.name = item.name;
        effect.start_ms = round.clock.time_passed_ms;
        effect.duration_ms = item.duration_ms;
        effect.amount = item.effect_amount;
        
        round.active_effects.add(effect);

        if(item.name == SLOW_BALL){
            round.ball.speed -= item.effect_amount;
            round.ball.speed = clamp_double(round.ball.speed, BALL_MIN_BASE_SPEED, BALL_MAX_BASE_SPEED);

            vector_2d direction = unit_vector(round.ball.velocity);
            round.ball.velocity.x = direction.x * round.ball.speed;
            round.ball.velocity.y = direction.y * round.ball.speed;
        }
    }
}

void ball_reflect_from_fish(ball_data& ball, fish_data& fish){
    vector_2d normal = vector_from_to(fish.pos, ball.pos, 1.0);
    ball.velocity = reflect_velocity(ball.velocity, normal);

    //将球从鱼中推出
    ball.pos.x = fish.pos.x + normal.x * (ball.radius + fish.radius);
    ball.pos.y = fish.pos.y + normal.y * (ball.radius + fish.radius);
}

void capture_fish(round_data& round, int fish_index){
    fish_data& fish = round.fishes.items[fish_index];
    point_2d capture_point = fish.pos;
    long now = round.clock.time_passed_ms;

    round.player.score += fish.item.score;
    //创建浮动分数对象
    round.floating_scores.add(new_floating_score(capture_point, fish.item.score, now));

    apply_item_effect(round, fish.item);
    trigger_fish_panic(round.fishes.items, capture_point, now);

    fish.velocity = {0, 0};
    fish.pending_remove = true;
    //音频
    if(fish.item.type == ITEM_NONE){
        play_sound_effect("eat_fish", 1.3);
    }
    else{
        play_sound_effect("item_get", 0.65);
    }
}

void handle_ball_fish_collision(round_data& round){
    for(int i = round.fishes.items.length() - 1; i >= 0; i--){
        fish_data& fish = round.fishes.items[i];
        //播放被捕获动画期间不参与碰撞
        if(fish.pending_remove){
            continue;
        }
        if(!fish.active || !ball_hits_fish(round.ball, fish)){
            continue;
        }
        bool captured = apply_hit_to_fish(fish, 1);
        long now = round.clock.time_passed_ms;
        if(captured){
            capture_fish(round, i);
            try_play_fish_animation(fish.animation, FISH_ANIM_CAPTURED, now);
            try_play_ball_animation(round.ball, BALL_EAT, now);
        }
        else{
            ball_reflect_from_fish(round.ball, fish);
        }
    }
}
