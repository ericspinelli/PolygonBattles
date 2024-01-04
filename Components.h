#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CCollision
{
public:
    float radius;
    CCollision(float r)
        : radius(r) {}
};

class CInput
{
public:
    bool up      = false;
    bool down    = false;
    bool left    = false;
    bool right   = false;
    bool shoot   = false;
    bool special = false;

    CInput() {}
};

class CLifespan
{
public:
    int remaining   = 0;
    int total       = 0;
    CLifespan(int total)
        : remaining(total), total(total) {}
};

class CLives
{
public:
    int remaining    = 0;
    int invulnerable = 0;
    int invulnMax    = 0;
    CLives(int total, int invFrames)
        : remaining(total), invulnMax(invFrames) {}
};

class CScore
{
public:
    int score = 0;
    CScore(int val) : score(val) {}
};

class CShape
{
public:
    sf::CircleShape circle;

    CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
        : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

class CText
{
public:
    sf::Text    text;

    CText(const std::string& input_text, sf::Font& font, Vec2 pos, int charSize, sf::Color color)
    {
        text.setString(input_text);
        text.setFont(font);
        text.setPosition(pos.x, pos.y);
        text.setCharacterSize(charSize);
        text.setFillColor(color);
    }
};

class CTransform
{
public:
    Vec2    pos         = { 0.0, 0.0 };
    Vec2    velocity    = { 0.0, 0.0 };
    float   angle       = 0.0;

    CTransform(const Vec2& p, const Vec2& v, float a)
        : pos(p), velocity(v), angle(a) {}
};