#include "Vec2.h"

Vec2::Vec2() {}

Vec2::Vec2(float x_in, float y_in)
        : x(x_in)
        , y(y_in)
    {}

float Vec2::length()
{
    return sqrt((x*x) + (y*y));
}

void Vec2::normalize()
{
    float len = length();
    x /= len;
    y /= len;
}

// Returns the distance from self to rhs
float Vec2::dist(const Vec2& rhs) const
{
    Vec2 diff = Vec2(rhs.x - x, rhs.y - y);
    return sqrtf((diff.x * diff.x) + (diff.y * diff.y));
}

Vec2 Vec2::operator +(const Vec2& rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
Vec2 Vec2::operator -(const Vec2& rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
Vec2 Vec2::operator *(const float val) const { return Vec2(x * val, y * val); }
Vec2 Vec2::operator /(const float val) const { return Vec2(x / val, y / val); }

bool Vec2::operator ==(const Vec2& rhs) const { return ((x == rhs.x) && (y == rhs.y)); }
bool Vec2::operator !=(const Vec2& rhs) const { return ((x != rhs.x) || (y != rhs.y)); }

void Vec2::operator +=(const Vec2& rhs) { x += rhs.x; y += rhs.y; }
void Vec2::operator -=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; }
void Vec2::operator *=(const float val) { x *= val; y *= val; }
void Vec2::operator /=(const float val) { x /= val; y /= val; }

// For test purposes only
/*
void Vec2::print() const
{
    std::cout << "x = " << x << " y = " << y << std::endl;
}

int main(int arg, char * args[])
{
    Vec2 v1 = Vec2(3,4);
    Vec2 v2 = Vec2(5,6);
    Vec2 v3 = Vec2(3,4);

    Vec2 result;
    float val = 100.0;
    result = v1 + v2; result.print();
    result = v1 - v2; result.print();
    result = v1 * val; result.print();
    result = v2 * val; result.print();

    result = v1;
    result += v2; result.print();
    result -= v2; result.print();
    result *= val; result.print();
    result /= val; result.print();

    std::cout << (v1 == v2) << " " << (v1 == v3) << std::endl;
    std::cout << (v1 != v2) << " " << (v1 != v3) << std::endl;

    std::cout << "Length v1: " << v1.length() << std::endl;
    std::cout << "Length v2: " << v2.length() << std::endl;
    result = v1 + v2;
    std::cout << "Length result: " << result.length() << std::endl;

    std::cout << "v1" << std::endl;
    v1.print();
    std::cout << "v1 normalized" << std::endl;
    v1.normalize();
    v1.print();
    std::cout << v1.length() << std::endl;

    Vec2 b = Vec2(6,7);
    Vec2 a = Vec2(4,2);
    float dist = a.dist(b);
    std::cout << "Dist ab = " << dist << std::endl;
    dist = b.dist(a);
    std::cout << "Dist ba = " << dist << std::endl;

    return 0;
}*/
