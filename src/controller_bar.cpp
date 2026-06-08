#include "controller_internal.h"

//-------------------- Bar update and collision --------------------

void update_bar(bar_data& bar, double length, double thickness, double margin){
    if(bar.orientation == HORIZONTAL_BAR){
        bar.bounds.width = length;
        bar.bounds.height = thickness;
        bar.bounds.y = bar.fixed_position;
        //限制bar在屏幕内
        if(mouse_x() - bar.bounds.width / 2 <= margin){
            bar.bounds.x = margin;
        }
        else if(mouse_x() + bar.bounds.width / 2 >= WINDOW_WIDTH - margin){
            bar.bounds.x = WINDOW_WIDTH - bar.bounds.width - margin;
        }
        else{
            bar.bounds.x = mouse_x() - bar.bounds.width / 2;
        }

        bar.speed = bar.bounds.x - bar.previous_position;
        bar.previous_position = bar.bounds.x;

    }
    else{
        bar.bounds.width = thickness;
        bar.bounds.height = length;
        bar.bounds.x = bar.fixed_position;
        //限制bar在屏幕内
        if(mouse_y() - bar.bounds.height / 2 <= margin + HUD_HEIGHT){
            bar.bounds.y = margin + HUD_HEIGHT;
        }
        else if(mouse_y() + bar.bounds.height / 2 >= WINDOW_HEIGHT - margin){
            bar.bounds.y = WINDOW_HEIGHT - bar.bounds.height - margin;
        }
        else{
            bar.bounds.y = mouse_y() - bar.bounds.height / 2;
        }
        bar.speed = bar.bounds.y - bar.previous_position;
        bar.previous_position = bar.bounds.y;
    }
}

void update_bars(bars_data& bars){
    update_bar(bars.top, bars.length, bars.thickness, bars.margin);
    update_bar(bars.bottom, bars.length, bars.thickness, bars.margin);
    update_bar(bars.left, bars.length, bars.thickness, bars.margin);
    update_bar(bars.right, bars.length, bars.thickness, bars.margin);

}



bool ball_hits_bar(const ball_data& ball, const bar_data &bar){
    double closest_x;
    double closest_y;

    if(ball.pos.x < bar.bounds.x){
        closest_x = bar.bounds.x;
    }
    else if(ball.pos.x > bar.bounds.x + bar.bounds.width){
        closest_x = bar.bounds.x + bar.bounds.width;
    }
    else{
        closest_x = ball.pos.x;
    }

    if(ball.pos.y < bar.bounds.y){
        closest_y = bar.bounds.y;
    }
    else if(ball.pos.y > bar.bounds.y + bar.bounds.height){
        closest_y = bar.bounds.y + bar.bounds.height;
    }
    else{
        closest_y = ball.pos.y;
    }

    double x_distance = ball.pos.x - closest_x;
    double y_distance = ball.pos.y - closest_y;
    //sqrt(x^2 + y^2) <= r^2
    return x_distance * x_distance + y_distance * y_distance <= ball.radius * ball.radius;
}

vector_2d reflect_velocity(vector_2d v0, vector_2d normal){
    double dot = v0.x * normal.x + v0.y * normal.y;
    vector_2d v1;
    v1.x = v0.x - 2 * dot * normal.x;
    v1.y = v0.y - 2 * dot * normal.y;
    return v1;
}

vector_2d bar_velocity(const bar_data& bar){
    if(bar.orientation == HORIZONTAL_BAR){
        return {bar.speed, 0};
    }
    else{
        return {0, bar.speed};
    }
}

void reflect_ball_from_bar(ball_data& ball, const bar_data& bar, vector_2d normal){
    vector_2d bar_v = bar_velocity(bar);
    //小球相对于杆的速度
    vector_2d relative_velocity = {ball.velocity.x - bar_v.x, ball.velocity.y - bar_v.y};
    //球相对速度在法线方向的分量
    double approach_speed = relative_velocity.x * normal.x + relative_velocity.y * normal.y;
    //小于0说明小球正在靠近碰撞面
    if(approach_speed < 0){
        vector_2d reflect_v = reflect_velocity(relative_velocity, normal);

        ball.velocity.x = reflect_v.x + bar_v.x;
        ball.velocity.y = reflect_v.y + bar_v.y;
    }
}

void limit_spin(ball_data& ball){
    if(ball.spin > MAX_SPIN){
        ball.spin = MAX_SPIN;
    }
    else if(ball.spin < -MAX_SPIN){
        ball.spin = -MAX_SPIN;
    }
}

void apply_bar_effect_on_spin(ball_data& ball, const bar_data& bar, vector_2d normal){
    if(abs(bar.speed) <= 2){
        return;
    }

    if(bar.orientation == HORIZONTAL_BAR){
        ball.spin += normal.y * bar.speed * SPIN_AFFECT;
    }
    else{
        ball.spin -= normal.x * bar.speed * SPIN_AFFECT;
    }
    limit_spin(ball);
}

void apply_bar_hit_position_effect(ball_data& ball, const bar_data& bar, point_2d center){
    double speed = vector_magnitude(ball.velocity);
    vector_2d offset = {
        (ball.pos.x - center.x) / (bar.bounds.width / 2),
        (ball.pos.y - center.y) / (bar.bounds.height / 2)
    };
    
    if(bar.orientation == HORIZONTAL_BAR){
        if(offset.x < -1){
            offset.x = -1;
        }
        else if(offset.x > 1){
            offset.x = 1;
        }
        ball.velocity.x += offset.x * speed * HIT_POSITION_AFFECT;
    }
    else{
        if(offset.y < -1){
            offset.y = -1;
        }
        else if(offset.y > 1){
            offset.y = 1;
        }
        ball.velocity.y += offset.y * speed * HIT_POSITION_AFFECT;
    }

    vector_2d direction = unit_vector(ball.velocity);
    ball.velocity.x = direction.x * speed;
    ball.velocity.y = direction.y * speed;
}

void handle_ball_top_bottom_bar_collision(ball_data& ball, const bar_data& bar, double top, double bottom, point_2d center){
    vector_2d normal;
    //球在杆上方，从上方推出
    if(ball.pos.y < center.y){
        ball.pos.y = top - ball.radius;
        normal = {0, -1};
    }
    //球在杆下方，从下方推出
    else{
        ball.pos.y = bottom + ball.radius;
        normal = {0, 1};
    }
    reflect_ball_from_bar(ball, bar, normal);

    //正面碰撞
    if(bar.orientation == HORIZONTAL_BAR){
        apply_bar_hit_position_effect(ball, bar, center);
        apply_bar_effect_on_spin(ball, bar, normal);
    }
}

void handle_ball_left_right_bar_collision(ball_data& ball, const bar_data& bar, double left, double right, point_2d center){
    vector_2d normal;
    //球在杆左侧，从左侧推出
    if(ball.pos.x < center.x){
        ball.pos.x = left - ball.radius;
        normal = {-1, 0};
    }
    //球在杆右侧，从右侧推出
    else{
        ball.pos.x = right + ball.radius;
        normal = {1, 0};
    }
    reflect_ball_from_bar(ball, bar, normal);
    //正面碰撞
    if(bar.orientation == VERTICAL_BAR){
        apply_bar_hit_position_effect(ball, bar, center);
        apply_bar_effect_on_spin(ball, bar, normal);
    }
}

void handle_ball_corner_bar_collision(ball_data& ball, const bar_data& bar, double left, double right, double top, double bottom){
    point_2d corner;
    corner.x = clamp_double(ball.pos.x, left, right);
    corner.y = clamp_double(ball.pos.y, top, bottom);

    vector_2d normal = vector_from_to(corner, ball.pos, 1.0);
    //将小球沿法线方向推出
    ball.pos.x = corner.x + normal.x * ball.radius;
    ball.pos.y = corner.y + normal.y * ball.radius;
    //更新反弹后的速度矢量
    reflect_ball_from_bar(ball, bar, normal);
    //limit_ball_speed(ball);
}

bool handle_ball_bar_collision(ball_data &ball, const bar_data& bar, long now){
    if(!ball_hits_bar(ball,bar)){
        return false;
    }
    double speed_before = vector_magnitude(ball.velocity);

    double left = bar.bounds.x;
    double right = bar.bounds.x + bar.bounds.width;
    double top = bar.bounds.y;
    double bottom = bar.bounds.y + bar.bounds.height;
    point_2d center = {(left + right) / 2, (top + bottom) / 2};

    bool horizontal_inside = ball.pos.x > left && ball.pos.x < right;
    bool vertical_inside = ball.pos.y > top && ball.pos.y < bottom;
    //球心在杆的内部
    if(horizontal_inside && vertical_inside){
        double nearest_x = ball.pos.x - left;
        if(nearest_x > right - ball.pos.x){
            nearest_x = right - ball.pos.x;
        }
        double nearest_y = ball.pos.y - top;
        if(nearest_y > bottom - ball.pos.y){
            nearest_y = bottom - ball.pos.y;
        }
        //离水平方向更近一些，向上下推出
        if(nearest_x > nearest_y){
            handle_ball_top_bottom_bar_collision(ball, bar, top, bottom, center);
        }
        else{
            handle_ball_left_right_bar_collision(ball, bar, left, right, center);
        }
    }
    //球在杆上方或下方
    else if(horizontal_inside){
        handle_ball_top_bottom_bar_collision(ball, bar, top, bottom, center);
    }
    //球在杆左侧或右侧
    else if(vertical_inside){
        handle_ball_left_right_bar_collision(ball, bar, left, right, center);
    }
    //球在杆的角落
    else{
        handle_ball_corner_bar_collision(ball, bar, left, right, top, bottom);
    }
    double speed_after = vector_magnitude(ball.velocity);
    if(speed_after > speed_before + 2.0){
        try_play_ball_animation(ball, BALL_BOOST, now);
    }
    return true;
}

void handle_ball_bars_collision(round_data& round){
    long now = round.clock.time_passed_ms;
    bool hit_top = handle_ball_bar_collision(round.ball, round.bars.top, now);
    bool hit_bottom = handle_ball_bar_collision(round.ball, round.bars.bottom, now);
    bool hit_left = handle_ball_bar_collision(round.ball, round.bars.left, now);
    bool hit_right = handle_ball_bar_collision(round.ball, round.bars.right, now);

    if(hit_top || hit_bottom || hit_left || hit_right){
        play_sound_effect("bar_hit", 0.45);
    }
}
