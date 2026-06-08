#include "controller_internal.h"

//-------------------- Ball movement and animation --------------------

void handle_ball_window_collision(ball_data& ball, bool window_collision){
    if(!window_collision){
        return;
    }
    //小球从屏幕左侧反弹
    if(ball.pos.x - ball.radius <= 0){
        ball.pos.x = ball.radius;
        ball.velocity.x *= -1;
    }
    //小球从屏幕右侧反弹
    if(ball.pos.x + ball.radius >= WINDOW_WIDTH){
        ball.pos.x = WINDOW_WIDTH - ball.radius;
        ball.velocity.x *= -1;
    }
    //小球从屏幕上方反弹
    if(ball.pos.y - ball.radius <= HUD_HEIGHT){
        ball.pos.y = ball.radius + HUD_HEIGHT;
        ball.velocity.y *= -1;
    }
    //小球从屏幕下方反弹
    if(ball.pos.y + ball.radius >= WINDOW_HEIGHT){
        ball.pos.y = WINDOW_HEIGHT - ball.radius;
        ball.velocity.y *= -1;
    }
}

animation_rule ball_animation_rule_for(ball_animation_state state){
    switch(state){
        case BALL_STUNNED: return {4, BALL_STUNNED_ANIMATION_MS, false};
        case BALL_HURT: return {3, BALL_HURT_ANIMATION_MS, false};
        case BALL_EAT: return {2, BALL_EAT_ANIMATION_MS, false};
        case BALL_BOOST: return {1, BALL_BOOST_ANIMATION_MS, false};
        case BALL_IDLE:
        default: return {0, 0, true};
    }
}

void try_play_ball_animation(ball_data& ball, ball_animation_state state, long now){
    animation_rule current_rule = ball_animation_rule_for(ball.animation.state);
    animation_rule new_rule = ball_animation_rule_for(state);

    bool current_finished = ball.animation.state == BALL_IDLE || now >= ball.animation.playback.end_time;
    bool can_replace = current_finished || new_rule.priority >= current_rule.priority;
    if(can_replace){
        ball.animation.state = state;
        ball.animation.playback.start_time = now;
        ball.animation.playback.end_time = now + new_rule.duration_ms;
    }
}

void update_ball_animation(ball_data& ball, long now){
    if(ball.animation.state != BALL_IDLE && now >= ball.animation.playback.end_time){
        ball.animation.state = BALL_IDLE;
    }
}

void stabilise_ball_speed(ball_data &ball){
    double current_speed = vector_magnitude(ball.velocity);
    if(current_speed == 0){
        return;
    }
    vector_2d direction = unit_vector(ball.velocity);

    if(current_speed > ball.speed){
        double excess_speed = current_speed - ball.speed;

        double decay_rate = 0.01 + excess_speed * 0.01;
        decay_rate = clamp_double(decay_rate, 0.01, 0.20);

        current_speed -= excess_speed * decay_rate;
        if(current_speed < ball.speed){
            current_speed = ball.speed;
        }
    }
    else if(current_speed < ball.speed){
        current_speed *= BALL_SPEED_RECOVER;
        if(current_speed > ball.speed){
            current_speed = ball.speed;
        }
    }

    ball.velocity.x = direction.x * current_speed;
    ball.velocity.y = direction.y * current_speed;
}

void apply_spin(ball_data& ball){
    if(abs(ball.spin) < 0.1){
        ball.spin = 0;
        return;
    }
    double speed = vector_magnitude(ball.velocity);
    double angle = vector_angle(ball.velocity);
    angle += ball.spin;
    
    ball.velocity = vector_from_angle(angle, speed);

    ball.spin *= SPIN_DECAY;
}

void update_ball(ball_data& ball){
    apply_spin(ball);
    ball.pos.x += ball.velocity.x;
    ball.pos.y += ball.velocity.y;
    stabilise_ball_speed(ball);
}

bool ball_out_of_play_area(const ball_data& ball){
    return ball.pos.x + ball.radius < 0 ||
           ball.pos.x - ball.radius > WINDOW_WIDTH ||
           ball.pos.y + ball.radius < HUD_HEIGHT ||
           ball.pos.y - ball.radius > WINDOW_HEIGHT;
}

//-------------------- Impulse and water pulse --------------------

void apply_impulse_to_ball(ball_data& ball, const impulse_data& impulse){
    if(circles_intersect(ball.pos.x, ball.pos.y, ball.radius, impulse.center.x, impulse.center.y, impulse.range)){
        vector_2d direction = vector_from_to(impulse.center, ball.pos, 1.0);
        double force = impulse.force;

        //处理距离衰减情况
        if(impulse.falloff == DISTANCE_FALLOFF){
            double distance = point_point_distance(impulse.center, ball.pos);
            double effective_distance = distance - ball.radius;

            if(effective_distance < 0){
                effective_distance = 0;
            }

            double distance_radio = effective_distance / impulse.range;
            double strength = 1 - distance_radio;
            force *= strength * strength;   //让变化更显著
        }

        ball.velocity.x += force * direction.x;
        ball.velocity.y += force * direction.y;
    }
}

void handle_water_pulse(round_data& round){
    long now = round.clock.time_passed_ms;
    if(mouse_clicked(LEFT_BUTTON) && now >= round.water_pulse.cooldown_end_time){
        round.water_pulse.impulse.center = mouse_position();

        round.water_pulse.cooldown_end_time = now + PULSE_COOLDOWN_MS;
        round.water_pulse.display_end_time = now + PULSE_DISPLAY_MS;

        apply_impulse_to_ball(round.ball, round.water_pulse.impulse);
        play_sound_effect("water_pulse", 0.45);
    }
}
