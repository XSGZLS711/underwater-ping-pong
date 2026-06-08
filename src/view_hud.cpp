#include "view_internal.h"

//-------------------- HUD calculation helpers --------------------

double effect_remaining_ratio(const active_effect_data& effect, long now){
    long elapsed = now - effect.start_ms;
    if(elapsed >= effect.duration_ms){
        return 0;
    }
    return 1 - (double)elapsed / effect.duration_ms;
}
//用很多条很短的线段，拼出一个“圆形倒计时进度条
point_2d point_on_circle(double x, double y, double radius, double angle){
    double radians = angle * 3.1415926 / 180.0;
    return {
        x + cos(radians) * radius,
        y + sin(radians) * radius
    };
}

color countdown_color(double ratio){
    if(ratio <= 0.25){
        return rgba_color(255, 45, 35, 240);
    }
    if(ratio <= 0.50){
        return rgba_color(255, 220, 80, 230);
    }

    return rgba_color(124, 252, 0, 245);
}


//-------------------- HUD widgets --------------------
void draw_timer_ring(double x, double y, double ratio, double radius, color ring_color){
    //把圆一圈拆成56份
    int segments = 56;
    double angle_per_segment = 360.0 / segments;
    int active_segments = segments * ratio;

    for(int i = 0; i < active_segments; i++){
        double angle1 = -90 - i * angle_per_segment;
        double angle2 = -90 - (i + 1) * angle_per_segment;

        point_2d point1 = point_on_circle(x, y, radius, angle1);
        point_2d point2 = point_on_circle(x, y, radius, angle2);

        draw_line(ring_color, point1, point2, option_line_width(2));
    }
}

void draw_effect_countdown(const active_effect_data& effect, long now, double x, double y, double radius){
    double elapsed = now - effect.start_ms;
    long remaining = effect.duration_ms - elapsed;

    if(remaining > 0 && remaining <= 3000){
        int seconds = remaining / 1000 + 1;

        fill_circle(rgba_color(0, 0, 0, 100), x, y, radius);
        draw_centered_text(to_string(seconds), COLOR_WHITE, x, y - 10, 22);
    }
}


//-------------------- HUD sections --------------------
void draw_hud_background(){
    fill_rectangle(rgba_color(3, 22, 38, 210), 0, 0, WINDOW_WIDTH, HUD_HEIGHT);
}

void draw_health_icons(const round_data& round){
    double scale = 0.75;
    double x = 64 * scale / 2 + 7;
    double y = HUD_HEIGHT / 2;
    double space = 50;

    for(int i = 0; i < PLAYER_MAX_HEALTH; i++){
        int frame = 4;

        if(i < round.player.health){
            frame = loop_animation_frame(round.clock.time_passed_ms, i, ICON_HEALTH_ANIMATION);
        }

        draw_sheet_frame(ICON_HEALTH_SHEET, frame, x, y, scale, scale);
        x += space;
    }
}

void draw_active_effect_icons(const round_data& round){
    double scale = 0.8;
    double x = WINDOW_WIDTH - 85;
    double y = HUD_HEIGHT / 2;
    double space = 50;

    for(int i = 0; i < round.active_effects.length(); i++){
        active_effect_data effect = round.active_effects[i];
        long now = round.clock.time_passed_ms;
        if(now - effect.start_ms >= effect.duration_ms){
            continue;
        }
        //画图标
        draw_item_icon(effect.name, x, y, i, scale, now);
        //画圆环倒计时
        double radius = 20;
        double ratio = effect_remaining_ratio(effect, now);
        color ring_color = countdown_color(ratio);
        draw_timer_ring(x, y, ratio, radius, ring_color);
        //画数字倒数
        draw_effect_countdown(effect, now, x, y, radius);
        x -= space;
    }
}

void draw_pulse_icon(const round_data& round){
    double x = WINDOW_WIDTH - 30;
    double y = HUD_HEIGHT / 2;
    double radius = 16;
    long now = round.clock.time_passed_ms;

    bool ready = now >= round.water_pulse.cooldown_end_time;

    color outer = ready ? rgba_color(120, 240, 255, 180) : rgba_color(80, 120, 145, 140);
    color core = ready ? rgba_color(40, 190, 255, 240) : rgba_color(35, 75, 95, 210);

    fill_circle(rgba_color(20, 180, 255, ready ? 45 : 20), x, y, radius + 7);
    fill_circle(rgba_color(5, 35, 55, 210), x, y, radius + 2);
    fill_circle(core, x, y, radius);
    draw_circle(outer, x, y, radius + 2);

    // 高光
    fill_circle(rgba_color(255, 255, 255, ready ? 150 : 70), x - 5, y - 6, 4);

    // 冷却进度环
    if(!ready){
        long remaining = round.water_pulse.cooldown_end_time - now;
        double ratio = 1.0 - (double)remaining / PULSE_COOLDOWN_MS;
        clamp_double(ratio, 0, 1);

        draw_timer_ring(x, y, ratio, radius + 6, rgba_color(120, 230, 255, 230));
    }
}

void draw_score_text(const player_data& player){
    draw_text("Score: " + to_string(player.score), COLOR_WHITE, get_system_font(), BODY_FONT_SIZE, 200, HUD_HEIGHT / 2 - 10);
}

void draw_hud(const round_data& round){
    draw_hud_background();
    draw_health_icons(round);
    draw_active_effect_icons(round);
    draw_pulse_icon(round);
    draw_score_text(round.player);
}
