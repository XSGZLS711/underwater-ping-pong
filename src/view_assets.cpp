#include "view_internal.h"

//-------------------- Resource loading --------------------

void load_sheet(const sheet_info& sheet){
    bitmap bmp = load_bitmap(sheet.bitmap_name, sheet.file_name);

    bitmap_set_cell_details(
        bmp,
        sheet.frame_width,
        sheet.frame_height,
        sheet.columns,
        sheet.rows,
        sheet.columns * sheet.rows
    );
}

void load_ui_sheets(){
    for(int i = 0; i < UI_SHEET_COUNT; i++){
        load_sheet(UI_SHEETS[i]);
    }
}

void load_ball_sheet(){
    load_sheet(PLAYER_SHARK_SHEET);
}

void load_bar_sheet(){
    load_sheet(BAR_SHEET);
}

void load_fish_sheets(){
    for(int i = 0; i < FISH_OBJECT_SHEET_COUNT; i++){
        load_sheet(FISH_OBJECT_SHEETS[i]);
    }
}

void load_torpedo_sheets(){
    load_sheet(TORPEDO_BODY_SHEET);
    load_sheet(TORPEDO_FIELD_SHEET);
    load_sheet(TORPEDO_EXPLOSION_SHEET);
}

void load_game_images(){
    load_ui_sheets();
    load_ball_sheet();
    load_bar_sheet();
    load_fish_sheets();
    load_torpedo_sheets();
} 

void load_game_sounds(){
    load_sound_effect("eat_fish", "eat_fish.wav");
    load_sound_effect("item_get", "item_get.wav");
    load_sound_effect("fish_hurt", "fish_hurt.wav");
    load_sound_effect("water_pulse", "water_pulse.wav");
    load_sound_effect("torpedo_beep", "torpedo_beep.wav");
    load_sound_effect("torpedo_explode", "torpedo_explode.wav");
    load_sound_effect("bar_hit", "bar_hit.wav");
    load_sound_effect("button_hover", "button_hover.wav");
    load_sound_effect("button_click", "button_click.wav");

    load_music("underwater_bgm", "underwater_bgm.mp3");
}
