#include "controller_internal.h"

//-------------------- Floating Score --------------------

void update_floating_scores(round_data& round){
    long now = round.clock.time_passed_ms;
    for(int i = round.floating_scores.length() - 1; i >= 0; i--){
        floating_score_data& float_score = round.floating_scores[i];
        //向上飘
        float_score.pos.y -= 0.6;

        if(now - float_score.start_ms >= float_score.duration_ms){
            round.floating_scores.remove(i);
        }

    }
}

//-------------------- Difficulty and item effects --------------------

double difficulty_progress(const round_data& round){
    double progress = (double)round.clock.time_passed_ms / DIFFICULTY_MAX_TIME_MS;
    return clamp_double(progress, 0, 1);
}

void update_difficulty(round_data& round){
    double progress = difficulty_progress(round);

    round.ball.speed = BALL_DEFAULT_SPEED + progress * (BALL_MAX_BASE_SPEED - BALL_DEFAULT_SPEED);
    round.bars.length = BAR_DEFAULT_LENGTH - progress * (BAR_DEFAULT_LENGTH - BAR_MIN_LENGTH);
    round.torpedoes.max_active_count = progress * TORPEDO_MAX_COUNT;
}



double item_effect_strength(const active_effect_data& effect, long elapsed_ms){
    if(elapsed_ms <= effect.duration_ms){
        return 1.0;
    }
    long fade_elapsed = elapsed_ms - effect.duration_ms;

    //效果到达持续时间后缓缓消散
    double fade_progress = (double)fade_elapsed / EFFECT_FADE_OUT_MS;
    return 1 - fade_progress;
}
//更新所有激活道具的效果
void update_item_effects(round_data& round){
    long now = round.clock.time_passed_ms;

    for(int i = round.active_effects.length() - 1; i >= 0; i--){
        active_effect_data& effect = round.active_effects[i];
        long elapsed_ms = now - effect.start_ms;

        if(elapsed_ms >= effect.duration_ms + EFFECT_FADE_OUT_MS){
            round.active_effects.remove(i);
            continue;
        }

        double strength = item_effect_strength(effect, elapsed_ms);
        if(effect.name == EXTEND_BAR){
            round.bars.length += effect.amount * strength;
        }
        else if(effect.name == SLOW_BALL){
            round.ball.speed -= effect.amount * strength;
        }

    }
    round.bars.length = clamp_double(round.bars.length, BAR_MIN_LENGTH, BAR_MAX_LENGTH);
    round.ball.speed = clamp_double(round.ball.speed, BALL_MIN_BASE_SPEED, BALL_MAX_BASE_SPEED);
}
