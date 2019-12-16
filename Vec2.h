
// Basic 2d vector
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
