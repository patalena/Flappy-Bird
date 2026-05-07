#include "Bird.h"
#include <algorithm>

Bird::Bird() : skorost(0.0f), ugol(0.0f), animTimer(0.0f), animFrame(1) {
    textures[0].loadFromFile("pinkbird-upflap.png");
    textures[1].loadFromFile("pinkbird-midflap.png");
    textures[2].loadFromFile("pinkbird-downflap.png");

    sprite.setTexture(textures[animFrame]); 

    sf::FloatRect b = sprite.getLocalBounds();  
    sprite.setOrigin(b.width / 2.0f, b.height / 2.0f); 
    sprite.setScale(1.5f, 1.5f);  
    sprite.setPosition(100.0f, 300.0f);  
}

void Bird::update(float dt) {
    skorost += GRAVITY * dt;  
    sprite.move(0, skorost * dt);  

    if (sprite.getPosition().y < 0) {  // птица выше верхнего края окна
        sprite.setPosition(sprite.getPosition().x, 0); 
        skorost = 0;  
    }

    if (skorost > 0) {
        // птица падает вниз
        ugol = std::min(skorost * 0.1f, MAX_UGOL); // угол (зависит от скорости)
    } else {
        // птица летит вверх
        ugol = std::max(skorost * 0.1f, MIN_UGOL);
    }
    sprite.setRotation(ugol);  // поворот спрайта на угол

    // анимация: перебираем три кадра
    animTimer += dt;  
    if (animTimer >= 0.1f) { 
        animTimer = 0.0f;
        animFrame = (animFrame + 1) % 3;
        sprite.setTexture(textures[animFrame]);  // новая текстура
    }
}

void Bird::jump() {
    skorost = JUMP_FORCE;
}


void Bird::draw(sf::RenderWindow& window) const {  
    window.draw(sprite);
}

sf::FloatRect Bird::poluchit_ramku() const {
    sf::FloatRect rect = sprite.getGlobalBounds(); 
    // уменьшение хитбокса
    rect.left += 5.0f;
    rect.top += 5.0f;
    rect.width -= 10.0f;
    rect.height -= 10.0f;
    return rect;
}

void Bird::reset() {
    sprite.setPosition(100.0f, 300.0f);
    sprite.setRotation(0.0f);  
    skorost = 0.0f;
    ugol = 0.0f;
    animTimer = 0.0f;
    animFrame = 1;
    sprite.setTexture(textures[animFrame]);
}
