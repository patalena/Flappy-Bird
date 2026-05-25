#include "Bird.h"
#include <string>
#include <cmath>   
#include <algorithm>

static std::string getSkinPath(int skin, int frame) {
    const char* names[3] = { "yellowbird", "bluebird", "pinkbird" };
    const char* frames[3] = { "-upflap.png", "-midflap.png", "-downflap.png" };
    return std::string(names[skin]) + frames[frame];
}

Bird::Bird() : skorost(0.0f), ugol(0.0f), animTimer(0.0f), animFrame(1) {
    setSkin(1);
}
void Bird::setSkin(int skinIndex) {
    if (skinIndex < 0) skinIndex = 0;
    if (skinIndex > 2) skinIndex = 2;

    textures[0].loadFromFile(getSkinPath(skinIndex, 0));
    textures[1].loadFromFile(getSkinPath(skinIndex, 1));
    textures[2].loadFromFile(getSkinPath(skinIndex, 2));

    sprite.setTexture(textures[1]); //средний кадр

    sprite.setPosition(100.0f, 300.0f);
    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin(b.width / 2.0f, b.height / 2.0f); //центр спрайта
    sprite.setScale(1.5f, 1.5f);

    animFrame = 1;
    animTimer = 0.0f;
    sprite.setTexture(textures[animFrame]);
}

void Bird::update(float dt) {
    skorost += GRAVITY * dt;  
    sprite.move(0, skorost * dt);  

    if (sprite.getPosition().y < 0) { 
        sprite.setPosition(sprite.getPosition().x, 0); 
        skorost = 0;  
    }
    if (skorost > 0) {

        ugol = std::min(skorost * 0.1f, MAX_UGOL); 
    } else {

        ugol = std::max(skorost * 0.1f, MIN_UGOL);
    }
    sprite.setRotation(ugol);  

    animTimer += dt;  
    if (animTimer >= 0.1f) { 
        animTimer = 0.0f;
        animFrame = (animFrame + 1) % 3;   //циклический перебор
        sprite.setTexture(textures[animFrame]);  
    }
}

void Bird::jump() {
    skorost = JUMP_FORCE;
}


void Bird::draw(sf::RenderWindow& window) const {  
    window.draw(sprite);
}

sf::FloatRect Bird::poluchit_ramku() const {
    sf::FloatRect rect = sprite.getGlobalBounds();  //учитывает позицию, поворот, масштаб
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

void Bird::setPhysics(float gravity, float jumpForce) {
    GRAVITY = gravity;
    JUMP_FORCE = jumpForce;
}
