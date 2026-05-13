#pragma once
#include <SFML/Graphics.hpp>

class Bird {
public:
    Bird();

    void update(float dt);   // dt = delta time = время в секундах с последнего кадра
    void jump();
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect poluchit_ramku() const;
    void reset();
    void setSkin(int skinIndex);
    void setPhysics(float gravity, float jumpForce);

private:
    sf::Texture textures[3]; 
    sf::Sprite sprite;
    float skorost;
    float ugol;
    float animTimer;   // таймер анимации - увеличивается на dt каждый кадр - когда достигает 0.1 секунды — переключаем кадр
    int animFrame;  // текущий кадр анимации (0, 1 или 2).

    float GRAVITY = 900.0f;
    float JUMP_FORCE = -300.0f;
    const float MAX_UGOL = 90.0f;
    const float MIN_UGOL = -15.0f;
};
