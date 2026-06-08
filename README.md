Underwater Ping Pong

A 2D underwater arcade survival game built with C++ and SplashKit.

Features:
- Ball and bar physics
- Fish AI and item fish
- Torpedoes and explosions
- HUD, menus, animations, sound effects
- Local profiles and leaderboard

The bin folder is not included. Create it before building.

Build:
clang++ -Wall -std=c++20 -O2 -Iinclude src/*.cpp -o bin/game -l splashkit

Run:
./bin/game