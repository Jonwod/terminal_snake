#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Vec2.h"


// It is assumed that a snake always has at least 3 segments
class Snake {
public:
    Snake() {
        _segments = std::vector<Vec2>{{2,0}, {1,0}, {0,0}};
    }

    enum MoveResult {ok, collision};

    MoveResult step() {
        Vec2 target = Vec2{(_segments[0].x + _direction.x) % COLS, (_segments[0].y + _direction.y) % LINES};
        if(target.x < 0) target.x = COLS + target.x;        // Feels like there might be a more concise way to do this?
        if(target.y < 0) target.y = LINES + target.y;
        for(Vec2& seg: _segments) {
            const Vec2 this_target = target;
            target = seg;
            seg = this_target;
        }
        
        for(int i = 1; i < _segments.size(); ++i) {
            if(head() == _segments[i]) {
                return collision;
            }
        }
        return ok;
    }
 
    void draw() {
        for(const Vec2& seg: _segments) {
            mvaddch(seg.y, seg.x, '@');  
        }
    }

    void set_input(char input) {
        if(input == 'w') {
            set_direction(Vec2{0,-1});
        }
        else if(input == 'd') {
            set_direction(Vec2{1,0});
        }
        else if(input == 's') {
            set_direction(Vec2{0, 1});
        }
        else if(input == 'a') {
            set_direction(Vec2{-1, 0});
        }
    }

    void set_direction(Vec2 new_direction) {
        if(new_direction != _direction.inverse())
            _direction = new_direction;
    }

    bool overlaps(const Vec2& point) const {
        for(auto seg: _segments){
            if(seg == point) {
                return true;
            }
        }
        return false;
    }

    inline const Vec2& head() const { return _segments[0]; }

    void grow() {
        const auto s = _segments.size();
        Vec2 grow_dir = _segments[s - 1] - _segments[s - 2];
        if(grow_dir.x != 0  &&  grow_dir.y != 0) {
            grow_dir.y = 0; // Prevent tail growing diagonally
        }
        const Vec2 new_seg = _segments[s-1] + grow_dir;
        _segments.push_back(new_seg);
    }
private:
    std::vector<Vec2> _segments;
    Vec2 _direction = {1, 0};
};


char get_input() {
    char input[80];
    getstr(input);
    char final_input = '\0';
    for(int i = 0; input[i]; ++i) {
        final_input = input[i];
    }
    return final_input;
}


Vec2 random_world_location() {
    return Vec2{rand() % COLS, rand() % LINES};
}


Vec2 random_food_location(const Snake& snake) {
    Vec2 location = random_world_location();
    while(snake.overlaps(location)){
        Vec2 location = random_world_location();
    }
    return location;
}


void draw_food(const Vec2& location) {
    mvaddch(location.y, location.x, '+');  
}


int main(){	
    // ~~~~ Ncurses setup ~~~~
    auto window = initscr();   // Start curses mode
    cbreak();                  // disable line buffering
    noecho();                  // Do not echo input to sceen
    nodelay(window, true);     // Prevents input functions from blocking (getch() and getstr())
    curs_set(0);               // Hide the cursor
    // ~~~~~~~~~~~~~~~~~~~~~~~

    srand(time(NULL));

    Snake snake;
    Vec2 food = random_food_location(snake);
    constexpr float step_rate = 10.f;
    bool live = true;

    while(live) {
        if(snake.step() == Snake::collision) { 
            live = false;
        }
        if(snake.head() == food) {
            snake.grow();
            food = random_food_location(snake);
        }
        clear();                // Clears the display
        snake.draw();
        draw_food(food);
        refresh();              // Updates the display
        snake.set_input(getch());
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.f / step_rate)));
    }

    // ~~~~~ Game over screen ~~~~~
    std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.f)));
    const std::string game_over = "game over";
    for(int i = 0; i < game_over.size(); ++i) {
        mvaddch(0, i, game_over[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(int(50.f)));
        refresh();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.f)));
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    endwin(); // End curses mode

    return 0;
}
