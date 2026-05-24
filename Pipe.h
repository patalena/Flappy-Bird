#pragma once
#include <SFML/Graphics.hpp>

class Pipe {
public:
    Pipe();
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void spawn(float x, float gap_center_y, float groundY);
    bool uletela_za_ekran() const;
    sf::FloatRect verhnyaya_ramka() const;
    sf::FloatRect nizhnyaya_ramka() const;
    bool  get_proshla() const;
    void  set_proshla(bool prolet);
    void setGap(float gap);
    void setSpeed(float speed);
    float getX() const;



private:

    static sf::Texture texture;
    static bool textureLoaded;

    sf::Sprite verhnyaya_truba;   
    sf::Sprite nizhnyaya_truba;  

    bool proshla;
    float prosvet;
    float SKOROST = 160.0f;
    float getGap() const { return prosvet; }

};
