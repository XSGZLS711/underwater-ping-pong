#ifndef VIEW_INTERNAL_H
#define VIEW_INTERNAL_H

#include "view.h"

//-------------------- View constants and view-only data --------------------

inline const int BUTTON_FONT_SIZE = 20;
inline const int TITLE_FONT_SIZE = 42;
inline const int BODY_FONT_SIZE = 18;

struct sheet_info{
    string bitmap_name;
    string file_name;
    double frame_width;
    double frame_height;
    int columns;
    int rows;
    int frame_count;
};

inline const sheet_info ICON_HEALTH_SHEET = {"health_sheet", "health_sheet.png", 64, 64, 5, 1, 5};
inline const sheet_info ICON_EXTEND_BAR_SHEET = {"extend_bar_sheet", "extend_bar_sheet.png", 64, 64, 4, 1, 4};
inline const sheet_info ICON_SLOW_BALL_SHEET = {"slow_ball_sheet", "slow_ball_sheet.png", 64, 64, 4, 1, 4};
inline const sheet_info ICON_BUBBLE_SHEET = {"item_bubble_sheet", "item_bubble_sheet.png", 64, 64, 4, 1, 4};
inline const sheet_info BACKGROUND_SHEET = {"background_sheet", "underwater_background_sheet.png", 1150, 750, 8, 1, 8};
inline const sheet_info UI_SHEETS[] = {
    ICON_HEALTH_SHEET,
    ICON_EXTEND_BAR_SHEET,
    ICON_SLOW_BALL_SHEET,
    ICON_BUBBLE_SHEET,
    BACKGROUND_SHEET
};
inline const int UI_SHEET_COUNT = sizeof(UI_SHEETS) / sizeof(UI_SHEETS[0]);

inline const sheet_info PLAYER_SHARK_SHEET = {"player_shark_sheet", "player_shark_sheet.png", 256, 256, 6, 5, 30};

inline const sheet_info BAR_SHEET = {"bar_sheet", "bar_sheet.png", 512, 96, 12, 1, 12};

inline const int TORPEDO_BODY_ROW_IDLE = 0;
inline const int TORPEDO_BODY_ROW_COUNTDOWN = 1;
inline const int TORPEDO_FIELD_ROW_TRIGGER = 0;
inline const int TORPEDO_FIELD_ROW_WARNING = 1;
inline const int TORPEDO_EXPLOSION_ROW = 0;

inline const sheet_info TORPEDO_BODY_SHEET = {"torpedo_body_sheet", "torpedo_body_sheet.png", 128, 128, 6, 2, 6};
inline const sheet_info TORPEDO_FIELD_SHEET = {"torpedo_field_sheet", "torpedo_field_sheet.png", 256, 256, 6, 2, 6};
inline const sheet_info TORPEDO_EXPLOSION_SHEET = {"torpedo_explosion_sheet", "torpedo_explosion_sheet.png", 256, 256, 6, 1, 6};

enum torpedo_part{
    TORPEDO_BODY,
    TORPEDO_EXPLOSION,
    TORPEDO_FIELD
};

struct torpedo_animation_info{
    torpedo_part part;
    torpedo_state state;
    sheet_info sheet;
    int row;
    long frame_ms;
    double scale;
    bool loop;
};

inline const torpedo_animation_info TORPEDO_ANIMATIONS[] = {
    {TORPEDO_BODY, TORPEDO_IDLE, TORPEDO_BODY_SHEET, TORPEDO_BODY_ROW_IDLE, 160, 0.55, true},
    {TORPEDO_BODY, TORPEDO_COUNTDOWN, TORPEDO_BODY_SHEET, TORPEDO_BODY_ROW_COUNTDOWN, 90, 0.55, true},
    {TORPEDO_EXPLOSION, TORPEDO_EXPLODING, TORPEDO_EXPLOSION_SHEET, TORPEDO_EXPLOSION_ROW, 80, 1.0, false},
    {TORPEDO_FIELD, TORPEDO_IDLE, TORPEDO_FIELD_SHEET, TORPEDO_FIELD_ROW_TRIGGER, 200, 1.0, true},
    {TORPEDO_FIELD, TORPEDO_COUNTDOWN, TORPEDO_FIELD_SHEET, TORPEDO_FIELD_ROW_WARNING, 200, 1.0, true}
};
inline const int TORPEDO_ANIMATION_COUNT = sizeof(TORPEDO_ANIMATIONS) / sizeof(TORPEDO_ANIMATIONS[0]);

inline const sheet_info SCORE_FISH_SHEET = {"score_fish_sheet", "score_fish_sheet.png", 128, 128, 6, 5, 6};
inline const sheet_info ITEM_FISH_ADD_LIFE_SHEET = {"item_fish_add_life_sheet", "item_fish_add_life_sheet.png", 128, 128, 6, 5, 6};
inline const sheet_info ITEM_FISH_EXTEND_BAR_SHEET = {"item_fish_extend_bar_sheet", "item_fish_extend_bar_sheet.png", 192, 128, 6, 5, 6};
inline const sheet_info ITEM_FISH_SLOW_BALL_SHEET = {"item_fish_slow_ball_sheet", "item_fish_slow_ball_sheet.png", 128, 128, 6, 5, 6};
inline const sheet_info FISH_OBJECT_SHEETS[] = {
    SCORE_FISH_SHEET,
    ITEM_FISH_ADD_LIFE_SHEET,
    ITEM_FISH_EXTEND_BAR_SHEET,
    ITEM_FISH_SLOW_BALL_SHEET
};
inline const int FISH_OBJECT_SHEET_COUNT = sizeof(FISH_OBJECT_SHEETS) / sizeof(FISH_OBJECT_SHEETS[0]);

struct fish_animation_info{
    item_name item;
    fish_animation_state state;
    sheet_info sheet;
    int row;
    long frame_ms;
    double scale;
    bool loop;
};

inline const int FISH_ROW_SWIM = 0;
inline const int FISH_ROW_FLEE = 1;
inline const int FISH_ROW_HURT = 2;
inline const int FISH_ROW_CAPTURED = 3;
inline const int FISH_ROW_EXPLODED = 4;

inline const double SCORE_FISH_SCALE = 0.45;
inline const double ADD_LIFE_FISH_SCALE = 0.55;
inline const double EXTEND_BAR_FISH_SCALE = 0.50;
inline const double SLOW_BALL_FISH_SCALE = 0.53;

inline const fish_animation_info FISH_ANIMATIONS[] = {
    {NO_ITEM, FISH_ANIM_SWIM, SCORE_FISH_SHEET, FISH_ROW_SWIM, 120, SCORE_FISH_SCALE, true},
    {NO_ITEM, FISH_ANIM_FLEE, SCORE_FISH_SHEET, FISH_ROW_FLEE, 80, SCORE_FISH_SCALE, true},
    {NO_ITEM, FISH_ANIM_CAPTURED, SCORE_FISH_SHEET, FISH_ROW_CAPTURED, 80, SCORE_FISH_SCALE, false},
    {NO_ITEM, FISH_ANIM_EXPLODED, SCORE_FISH_SHEET, FISH_ROW_EXPLODED, 90, SCORE_FISH_SCALE, false},

    {ADD_LIFE, FISH_ANIM_SWIM, ITEM_FISH_ADD_LIFE_SHEET, FISH_ROW_SWIM, 120, ADD_LIFE_FISH_SCALE, true},
    {ADD_LIFE, FISH_ANIM_FLEE, ITEM_FISH_ADD_LIFE_SHEET, FISH_ROW_FLEE, 80, ADD_LIFE_FISH_SCALE, true},
    {ADD_LIFE, FISH_ANIM_HURT, ITEM_FISH_ADD_LIFE_SHEET, FISH_ROW_HURT, 90, ADD_LIFE_FISH_SCALE, false},
    {ADD_LIFE, FISH_ANIM_CAPTURED, ITEM_FISH_ADD_LIFE_SHEET, FISH_ROW_CAPTURED, 80, ADD_LIFE_FISH_SCALE, false},
    {ADD_LIFE, FISH_ANIM_EXPLODED, ITEM_FISH_ADD_LIFE_SHEET, FISH_ROW_EXPLODED, 90, ADD_LIFE_FISH_SCALE, false},

    {EXTEND_BAR, FISH_ANIM_SWIM, ITEM_FISH_EXTEND_BAR_SHEET, FISH_ROW_SWIM, 120, EXTEND_BAR_FISH_SCALE, true},
    {EXTEND_BAR, FISH_ANIM_FLEE, ITEM_FISH_EXTEND_BAR_SHEET, FISH_ROW_FLEE, 80, EXTEND_BAR_FISH_SCALE, true},
    {EXTEND_BAR, FISH_ANIM_HURT, ITEM_FISH_EXTEND_BAR_SHEET, FISH_ROW_HURT, 90, EXTEND_BAR_FISH_SCALE, false},
    {EXTEND_BAR, FISH_ANIM_CAPTURED, ITEM_FISH_EXTEND_BAR_SHEET, FISH_ROW_CAPTURED, 80, EXTEND_BAR_FISH_SCALE, false},
    {EXTEND_BAR, FISH_ANIM_EXPLODED, ITEM_FISH_EXTEND_BAR_SHEET, FISH_ROW_EXPLODED, 90, EXTEND_BAR_FISH_SCALE, false},

    {SLOW_BALL, FISH_ANIM_SWIM, ITEM_FISH_SLOW_BALL_SHEET, FISH_ROW_SWIM, 120, SLOW_BALL_FISH_SCALE, true},
    {SLOW_BALL, FISH_ANIM_FLEE, ITEM_FISH_SLOW_BALL_SHEET, FISH_ROW_FLEE, 80, SLOW_BALL_FISH_SCALE, true},
    {SLOW_BALL, FISH_ANIM_HURT, ITEM_FISH_SLOW_BALL_SHEET, FISH_ROW_HURT, 90, SLOW_BALL_FISH_SCALE, false},
    {SLOW_BALL, FISH_ANIM_CAPTURED, ITEM_FISH_SLOW_BALL_SHEET, FISH_ROW_CAPTURED, 80, SLOW_BALL_FISH_SCALE, false},
    {SLOW_BALL, FISH_ANIM_EXPLODED, ITEM_FISH_SLOW_BALL_SHEET, FISH_ROW_EXPLODED, 90, SLOW_BALL_FISH_SCALE, false}
};
inline const int FISH_ANIMATION_COUNT = sizeof(FISH_ANIMATIONS) / sizeof(FISH_ANIMATIONS[0]);

struct loop_animation_info{
    sheet_info sheet;
    long cycle_ms;      //闪烁周期
    long sparkle_ms;    //闪烁总时间
    long frame_ms;      //每个frame的时间
    long interval_ms;   //icon之间闪烁间隔
};

inline const loop_animation_info ICON_HEALTH_ANIMATION = {ICON_HEALTH_SHEET, 4000, 800, 200, 220};
inline const loop_animation_info ICON_EXTEND_BAR_ANIMATION = {ICON_EXTEND_BAR_SHEET, 3600, 600, 150, 200};
inline const loop_animation_info ICON_SLOW_BALL_ANIMATION = {ICON_SLOW_BALL_SHEET, 3600, 600, 150, 200};
inline const loop_animation_info ICON_BUBBLE_ANIMATION = {ICON_BUBBLE_SHEET, 800, 800, 200, 200};
inline const loop_animation_info BACKGROUND_ANIMATION = {BACKGROUND_SHEET, 1600, 1600, 200, 0};
inline const loop_animation_info BAR_ANIMATION = {BAR_SHEET, 6000, 1920, 160, 0};
inline const loop_animation_info LOOP_ANIMATIONS[] = {
    ICON_HEALTH_ANIMATION,
    ICON_EXTEND_BAR_ANIMATION,
    ICON_SLOW_BALL_ANIMATION,
    ICON_BUBBLE_ANIMATION,
    BACKGROUND_ANIMATION,
    BAR_ANIMATION
};

struct animation_view_data{
    sheet_info sheet;
    int row;
    long frame_ms;      //每一个frame持续多久
    double scale;
    int frame_count;
    bool loop;
};

//-------------------- Shared view helpers --------------------

int loop_animation_frame(long time_ms, int icon_index, loop_animation_info animation);
int animation_frame(const animation_playback& playback, const animation_view_data& view, long now);
void draw_sheet_frame(const sheet_info& sheet, int frame, double center_x, double center_y, double scale_x, double scale_y, double angle = 0, bool flip_y = false);
void draw_idle_icon(const loop_animation_info& icon, double center_x, double center_y, int icon_index, double scale, long now, bool flip_y = false);
void draw_centered_text(const string& text, color text_color, double center_x, double y, int font_size);
void draw_item_icon(item_name name, double x, double y, int index, double scale, long now);
void draw_underwater_background(long now);
void draw_water_pulse(const round_data& round);
void draw_ball(const ball_data& ball, bool advance_animation, bool debug);
void draw_bars(const game_data& game);
void draw_fishes(const game_data& game);
void draw_torpedos(const game_data& game);
void draw_floating_scores(const round_data& round);
void draw_hud(const round_data& round);

void draw_profile_select(const game_data& game);
void draw_profile_create(const game_data& game);
void draw_menu(const game_data& game);
void draw_instructions(const game_data& game);
void draw_leaderboard(const game_data& game);
void draw_pause_overlay(const game_data& game);
void draw_game_over_overlay(const game_data& game);

#endif
