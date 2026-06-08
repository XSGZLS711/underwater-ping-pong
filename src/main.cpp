#include "model.h"
#include "view.h"
#include "controller.h"
#include "storage.h"

int main(){
    open_window("Underwater Ping Pong", WINDOW_WIDTH, WINDOW_HEIGHT);
    open_audio();

    
    game_data game = new_game();
    load_game_data(game);
    load_game_sounds();
    load_game_images();
    create_game_visuals();

    while(!quit_requested() && !game.should_quit){
        process_events();

        update_game(game);
        draw_game(game);
    }
    close_audio();

    return 0;
}
