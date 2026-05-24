#include "Pipe.h"


sf::Texture Pipe::texture;
bool Pipe::textureLoaded = false; 

Pipe::Pipe(): proshla(false), prosvet(150.0f) {
    if (!textureLoaded) {
        texture.loadFromFile("pipe-green.png");
        texture.setSmooth(false); 
        textureLoaded = true;
    }
    
    nizhnyaya_truba.setTexture(texture);
    verhnyaya_truba.setTexture(texture);
}

void Pipe::update(float dt) {
    verhnyaya_truba.move(-SKOROST * dt, 0);
    nizhnyaya_truba.move(-SKOROST * dt, 0);
}

void Pipe::draw(sf::RenderWindow& window) const {
    window.draw(verhnyaya_truba);
    window.draw(nizhnyaya_truba);
}

void Pipe::spawn(float x, float gap_center_y, float groundY) {
    float half_gap = prosvet / 2.0f; 

    float min_gap = half_gap + 10.0f;         
    float max_gap = groundY - half_gap - 10.0f; 

    if (gap_center_y < min_gap) gap_center_y = min_gap;
    if (gap_center_y > max_gap) gap_center_y = max_gap;

    nizhnyaya_truba.setScale(1.0f, 1.0f);
    nizhnyaya_truba.setPosition(x, gap_center_y + half_gap); 

    verhnyaya_truba.setScale(1.0f, -1.0f);
    verhnyaya_truba.setPosition(x, gap_center_y - half_gap);

    proshla = false;
}


bool Pipe::uletela_za_ekran() const {
    return nizhnyaya_truba.getPosition().x + (float)texture.getSize().x < 0;
}

sf::FloatRect Pipe::verhnyaya_ramka() const {
    return verhnyaya_truba.getGlobalBounds();
}

sf::FloatRect Pipe::nizhnyaya_ramka() const {
    return nizhnyaya_truba.getGlobalBounds();
}

bool Pipe::get_proshla() const  { return proshla; }
void Pipe::set_proshla(bool p)  { proshla = p; }
float Pipe::getX() const        { return nizhnyaya_truba.getPosition().x; }
void Pipe::setGap(float gap) { prosvet = gap; }
void Pipe::setSpeed(float speed) { SKOROST = speed; }