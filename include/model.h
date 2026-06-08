#ifndef MODEL_H
#define MODEL_H
#include "splashkit.h"
#include "dynamic_array.hpp"
#include "utilities.h"

const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;
const int HUD_HEIGHT = 50;

struct animation_playback{
    long start_time;
    long end_time;
    bool playing;
};

const int BALL_SIZE = 25;
const double BALL_DEFAULT_SPEED = 5;
const double BALL_MIN_BASE_SPEED = 4;
const double BALL_MAX_BASE_SPEED = 9;
const double BALL_SPEED_RECOVER = 1.01;

const double SPIN_AFFECT = 0.09;
const double MAX_SPIN = 4.0;
const double SPIN_DECAY = 0.95;

const long BALL_EAT_ANIMATION_MS = 500;
const long BALL_HURT_ANIMATION_MS = 1100;
const long BALL_STUNNED_ANIMATION_MS = 800;
const long BALL_BOOST_ANIMATION_MS = 400;
enum ball_animation_state{
    BALL_IDLE,
    BALL_EAT,
    BALL_HURT,
    BALL_STUNNED,
    BALL_BOOST
};

struct ball_animation_data{
    ball_animation_state state;
    animation_playback playback;
};

struct ball_data{
    bool active;
    point_2d pos;
    vector_2d velocity;
    double speed;
    double radius;
    double spin;

    ball_animation_data animation;
};



const double BAR_DEFAULT_LENGTH = 200.0;
const double BAR_MIN_LENGTH = 130.0;
const double BAR_MAX_LENGTH = 250.0;
const double BAR_THICKNESS = 15.0;
const double BAR_MARGIN = 10.0;

const double HIT_POSITION_AFFECT = 0.5;

enum bar_orientation{
    HORIZONTAL_BAR,
    VERTICAL_BAR
};
struct bar_data{
    rectangle bounds;
    bar_orientation orientation;
    double fixed_position;
    double speed;
    double previous_position;
};
struct bars_data{
    double length;
    double thickness;
    double margin;

    bar_data top;
    bar_data bottom;
    bar_data left;
    bar_data right;
};



const double PULSE_RADIUS = 150.0;
const double PULSE_FORCE = 1.5;
const long PULSE_COOLDOWN_MS = 500;
const long PULSE_DISPLAY_MS = 500;

enum impulse_falloff{
    NO_FALLOFF,
    DISTANCE_FALLOFF
};
struct impulse_data{
    point_2d center;
    double range;
    double force;
    impulse_falloff falloff;
};
struct water_pulse_data{
    bool active;
    impulse_data impulse;
    long cooldown_end_time;
    long display_end_time;
};



const int SCORE_FISH_RADIUS = 18;
const int ITEM_FISH_RADIUS = 25;

const double FISH_SPEED = 1.0;
const double FISH_FLEE_SPEED = 2.5;
const int FISH_FLEE_FRAMES = 30;
const double FISH_FEAR_RADIUS = 180.0;

const double FISH_TARGET_PUSH_RATE = 0.07;

const int FISH_MAX_TURN_ANGLE = 65;
const int FISH_DIRECTION_CHANGE_PERIOD = 15;
const int FISH_MIN_PAUSE_FRAMES = 20;
const int FISH_MAX_PAUSE_FRAMES = 60;


const int ADD_LIFE_REQUIRE_HIT = 3;
const int ADD_LIFE_SCORE = 100;

const int EXTEND_BAR_REQUIRE_HIT = 2;
const int EXTEND_BAR_SCORE = 50;
const int EXTEND_BAR_LENGTH = 50;
const long EXTEND_BAR_DURATION_MS = 10000;

const int SLOW_BALL_REQUIRE_HIT = 2;
const int SLOW_BALL_SCORE = 50;
const int SLOW_BALL_VALUE = 2;
const long SLOW_BALL_DURATION_MS = 15000;

enum fish_state{
    FISH_SWIMMING,
    FISH_PAUSED,
    FISH_FLEEING
};
enum fish_route{
    FISH_TO_TARGET,
    FISH_TO_EXIT
};
enum screen_side{
    TOP_SIDE,
    BOTTOM_SIDE,
    LEFT_SIDE,
    RIGHT_SIDE
};
enum item_name{
    NO_ITEM,
    EXTEND_BAR,
    ADD_LIFE,
    SLOW_BALL
};
enum item_type{
    ITEM_NONE,
    INSTANT_ITEM,
    TIMED_ITEM
};
struct item_info{
    item_name name;
    item_type type;
    int required_hits;
    int score;
    double effect_amount;
    long duration_ms;
};
struct side_data{
    screen_side spawn;
    screen_side opposite;
    screen_side left;
    screen_side right;
};

enum fish_animation_state{
    FISH_ANIM_SWIM,
    FISH_ANIM_FLEE,
    FISH_ANIM_HURT,
    FISH_ANIM_CAPTURED,
    FISH_ANIM_EXPLODED
};
struct fish_animation_data{
    fish_animation_state state;
    animation_playback playback;
};
struct fish_data{
    bool active;
    
    fish_state state;
    fish_route route;
    item_info item;

    point_2d pos;
    point_2d target_point;
    int target_points_remaining;
    point_2d exit_point;

    double speed;
    vector_2d velocity;
    bool facing_left;
    double radius;

    point_2d fear_source;
    double flee_speed;
    double flee_radius;

    int pause_timer;
    int flee_timer;
    int direction_change_timer;

    int hit_count;

    fish_animation_data animation;
    bool pending_remove;
    bool reward_given;
};
const int FISH_MAX_COUNT = 16;
const int ITEM_FISH_MAX_COUNT = 3;
const int ITEM_FISH_SPAWN_CHANCE = 20;

const long FISH_MIN_SPAWN_INTERVAL_MS = 500;
const long FISH_MAX_SPAWN_INTERVAL_MS = 2000;
struct fishes_data{
    dynamic_array<fish_data> items;
    int max_active_count;
    long next_spawn_time;
};



const int TORPEDO_MAX_RADIUS = 30;
const int TORPEDO_MIN_RADIUS = 25;
const double TORPEDO_PUSH_FORCE = 15.0;

const int TORPEDO_EXPLOSION_RANGE = 70;
const int TORPEDO_TRIGGER_RANGE = 90;

const int TORPEDO_FISH_HIT_POWER = 1;

const long TORPEDO_COUNTDOWN_MS = 3000;
const long TORPEDO_EXPLOSION_DISPLAY_MS = 500;
enum torpedo_state{
    TORPEDO_IDLE,
    TORPEDO_EXPLODING,
    TORPEDO_COUNTDOWN
};
struct torpedo_data{
    bool active;
    torpedo_state state;
    point_2d pos;
    double radius;
    double trigger_radius;

    impulse_data explosion_impulse;

    int damage;
    int fish_hit_power;

    animation_playback countdown_playback;
    animation_playback explosion_playback;
    int last_countdown_second;
};
const int TORPEDO_MAX_COUNT = 3;
const long TORPEDO_MIN_SPAWN_INTERVAL_MS = 3000;
const long TORPEDO_MAX_SPAWN_INTERVAL_MS = 7000;
const double TORPEDO_SAFE_SPAWN_DISTANCE = 100;
const double TORPEDO_SAFE_SPAWN_ANGLE = 90.0;
const int TORPEDO_SPAWN_ATTEMPTS = 20;
struct torpedoes_data{
    dynamic_array<torpedo_data> items;
    int max_active_count;
    long next_spawn_time;
};



const int PLAYER_MAX_HEALTH = 3;
struct player_data{
    int score;
    int health;
};

struct profile_data{
    string name;
    int total_coins;
    int best_score;
    int games_played;
    int selected_character;
};
struct profiles_data{
    dynamic_array<profile_data> items;
    int current_index;
};

struct leaderboard_entry_data{
    string player_name;
    int score;
    long survival_time_ms;
};
const int LEADERBOARD_MAX_ENTRIES = 10;
struct leaderboard_data{
    dynamic_array<leaderboard_entry_data> entries;
};


struct active_effect_data{
    item_name name;
    double amount;
    long start_ms;
    long duration_ms;
};
struct floating_score_data{
    point_2d pos;
    int score;
    long start_ms;
    long duration_ms;
};



struct round_clock_data{
    long time_passed_ms;
    long last_update_ms;
};



struct button_data{
    rectangle bounds;
    string text;
    bool hovered;
    bool clicked;
    bool was_hovered;
};
struct profile_select_menu_data{
    int first_visible_index;
    button_data create_button;
    button_data continue_button;
};
struct profile_create_menu_data{
    string typed_name;
    button_data create_button;
    button_data back_button;
};
struct main_menu_data{
    button_data start_button;
    button_data info_button;
    button_data left_character_button;
    button_data right_character_button;
    button_data unlock_button;
    button_data leaderboard_button;
    button_data quit_button;

    button_data switch_profile_button;
};
struct instructions_menu_data{
    button_data back_button;
};
struct pause_menu_data{
    button_data resume_button;
    button_data restart_button;
    button_data menu_button;
};
struct leaderboard_menu_data{
    button_data back_button;
};

struct game_over_menu_data{
    button_data restart_button;
    button_data menu_button;
};



//轮统计
struct round_stats_data{
    int score_fish_captured;
    int item_fish_captured;
    int torpedoes_triggered;
};
const int SCORE_PER_COIN = 10;
struct settlement_data{
    bool completed;
    int final_score;
    long survival_time_ms;
    int coins_earned;
    int score_fish_captured;
    int item_fish_captured;
};



const long DIFFICULTY_MAX_TIME_MS = 180000;
const long EFFECT_FADE_OUT_MS = 1500;
const double SCREEN_SHAKE_STRENGTH = 8.0;
const long FLOAT_SCORE_DURATION_MS = 800;
const long SCREEN_SHATE_DURATION = 500;
enum game_screen{
    PROFILE_SELECT_SCREEN,
    PROFILE_CREATE_SCREEN,
    MENU_SCREEN,
    INSTRUCTIONS_SCREEN,
    PLAYING_SCREEN,
    PAUSE_SCREEN,
    LEADERBOARD_SCREEN,
    GAME_OVER_SCREEN
};
struct screen_shake_data{
    long start_ms;
    long end_ms;
    double strength;
};
struct debug_data{
    bool on;
    bool show_collision;
    bool window_collision;
};
struct round_data{
    player_data player;

    ball_data ball;
    bars_data bars;
    water_pulse_data water_pulse;
    fishes_data fishes;
    torpedoes_data torpedoes;

    round_clock_data clock;
    dynamic_array<active_effect_data> active_effects;
    dynamic_array<floating_score_data> floating_scores;

    round_stats_data stats;
    settlement_data settlement;
    
    screen_shake_data screen_shake;
};
struct game_data{
    bool should_quit;
    game_screen current_screen;

    round_data round;

    profile_select_menu_data profile_select_menu;
    profile_create_menu_data profile_create_menu;
    main_menu_data main_menu;
    instructions_menu_data instructions_menu;
    pause_menu_data pause_menu;
    leaderboard_menu_data leaderboard_menu;
    game_over_menu_data game_over_menu;

    profiles_data profiles;
    leaderboard_data leaderboard;
    debug_data debug;
};
profile_data& current_profile(game_data& game);
const profile_data& current_profile(const game_data& game);

double clamp_double(double value, double min_value, double max_value);
vector_2d vector_from_to(point_2d from, point_2d to, double speed);
point_2d random_point_inside_screen(int margin);

ball_data new_ball();
bar_data new_bar(bar_orientation orientation, double fixed_position, double length, double thickness);
bars_data new_bars();
water_pulse_data new_water_pulse();
fish_data new_score_fish();
fish_data new_item_fish();
torpedo_data new_torpedo(point_2d point);
leaderboard_entry_data new_leaderboard_entry(const game_data& game);
floating_score_data new_floating_score(point_2d pos, int score, long now);
button_data new_button(double x, double y, double width, double height, string text);
profiles_data new_profiles();

round_data new_round();
game_data new_game();

#endif
