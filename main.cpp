#include <ncurses.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct Vec2{
    inline void operator +=(const Vec2& B) {
        x += B.x;
        y += B.y;
    }

    inline bool operator == (const Vec2& B) const {
        return x == B.x && y == B.y;
    }

    inline bool operator != (const Vec2& B) const {
        return x != B.x  ||  y != B.y;
    }

    inline Vec2 operator +(const Vec2& B) const {
        return Vec2{x + B.x, y + B.y};
    }

    inline Vec2 operator -(const Vec2& B) const {
        return Vec2{x - B.x, y - B.y};
    }

    inline Vec2 inverse() const {
        return Vec2{-x, -y};
    }

    std::string str() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    int x = 0;
    int y = 0;
};


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
	auto window = initscr();   // Start curses mode
    cbreak();                  // disable line buffering
    noecho();                  // Do not echo input to sceen
    nodelay(window, true);     // Prevents input functions from blocking (getch() and getstr())
    srand(time(NULL));

    Snake snake;
    Vec2 food = random_food_location(snake);

    constexpr float step_rate = 4.f;
    bool live = true;
    while(live) {
        if(snake.step() == Snake::collision) { 
            live = false;
        }
        if(snake.head() == food) {
            snake.grow();
            food = random_food_location(snake);
        }
        clear();
        snake.draw();
        draw_food(food);
        refresh();
        char c = get_input();
        if(c) {
            snake.set_input(c);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.f / step_rate)));
    }

    printw("game over");

	endwin();			/* End curses mode		  */

	return 0;
}
