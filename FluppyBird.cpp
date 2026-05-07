#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Bird.h"
#include "Pipe.h"

enum class GameState { MENU, PLAYING, GAME_OVER };

static void drawScore(sf::RenderWindow& window, sf::Texture digitTextures[10], int scoreVal, float yPos) {
    std::string s = std::to_string(scoreVal);

    float totalW = 0;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        totalW += digitTextures[c - '0'].getSize().x * 2.0f;
    }

    float x = (400.0f - totalW) / 2.0f;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        int d = c - '0';
        sf::Sprite sp(digitTextures[d]);
        sp.setScale(2.0f, 2.0f);
        sp.setPosition(x, yPos);
        window.draw(sp);
        x += digitTextures[d].getSize().x * 2.0f;
    }
}

static void resetGame(Bird& bird, std::vector<Pipe>& pipes, float PIPE_DISTANCE, float groundY) { 
    bird.reset();
    float x = 500.0f;
    for (int i = 0; i < 4; i++) {
        float gap_y = 100.0f + static_cast<float>(rand() % 301);
        pipes[i].spawn(x, gap_y, groundY); 
        x += PIPE_DISTANCE;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 600), "Flappy Bird");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));

    // ===== ЗАГРУЗКА ТЕКСТУР =====
    sf::Texture bgTexture, baseTexture, messageTexture, gameoverTexture;
    sf::Texture digitTextures[10];

    bgTexture.loadFromFile("day.png");
    baseTexture.loadFromFile("base.png");
    messageTexture.loadFromFile("message.png");
    gameoverTexture.loadFromFile("gameover.png");
    for (int i = 0; i < 10; i++)
        digitTextures[i].loadFromFile(std::to_string(i) + ".png");

    // ===== НАСТРОЙКА СПРАЙТОВ =====

    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(400.0f / bgTexture.getSize().x,
        600.0f / bgTexture.getSize().y);

    float baseScale = 400.0f / (float)baseTexture.getSize().x;
    float baseHeight = baseTexture.getSize().y * baseScale;
    float groundY = 600.0f - baseHeight;
    sf::Sprite baseSprite(baseTexture);
    baseSprite.setScale(baseScale, baseScale);
    baseSprite.setPosition(0, groundY);

    sf::Sprite messageSprite(messageTexture);
    {
        sf::FloatRect b = messageSprite.getLocalBounds();
        messageSprite.setOrigin(b.width / 2.0f, b.height / 2.0f);
        messageSprite.setPosition(200.0f, 270.0f);
    }

    sf::Sprite gameoverSprite(gameoverTexture);
    {
        sf::FloatRect b = gameoverSprite.getLocalBounds();
        gameoverSprite.setOrigin(b.width / 2.0f, b.height / 2.0f);
        gameoverSprite.setPosition(200.0f, 200.0f);
    }

    // ===== ИГРОВЫЕ ОБЪЕКТЫ =====
    Bird bird;
    std::vector<Pipe> pipes;
    for (int i = 0; i < 4; i++) pipes.push_back(Pipe());

    const float PIPE_DISTANCE = 230.0f;
    const float PIPE_WIDTH = 52.0f;

    GameState state = GameState::MENU;
    int score = 0;

    // начальная расстановка труб
    float x = 500.0f;
    for (int i = 0; i < 4; i++) {
        float gap_y = 100.0f + static_cast<float>(rand() % 301);
        pipes[i].spawn(x, gap_y, groundY);
        x += PIPE_DISTANCE;
    }

    sf::Clock clock;

    // ===== ИГРОВОЙ ЦИКЛ =====
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Space)
            {
                if (state == GameState::MENU) {
                    state = GameState::PLAYING;
                    bird.jump();
                }
                else if (state == GameState::PLAYING) {
                    bird.jump();
                }
                else if (state == GameState::GAME_OVER) {
                    resetGame(bird, pipes, PIPE_DISTANCE, groundY);
                    score = 0;
                    state = GameState::MENU;
                }
            }
        }

        // ===== ОБНОВЛЕНИЕ ЛОГИКИ =====
        if (state == GameState::PLAYING) {
            bird.update(dt);
            for (size_t i = 0; i < pipes.size(); i++) {
                pipes[i].update(dt);
            }

            // переспавн труб
            float rightmost = -1000.0f;
            for (size_t i = 0; i < pipes.size(); i++) {
                if (pipes[i].getX() > rightmost) rightmost = pipes[i].getX();
            }

            if (rightmost < 400.0f) {
                for (size_t i = 0; i < pipes.size(); i++) {
                    if (pipes[i].uletela_za_ekran()) {
                        float gap_y = 100.0f + static_cast<float>(rand() % 301);
                        pipes[i].spawn(rightmost + PIPE_DISTANCE, gap_y, groundY);
                        break;
                    }
                }
            }

            // счёт
            for (size_t i = 0; i < pipes.size(); i++) {
                if (!pipes[i].get_proshla() &&
                    pipes[i].getX() + PIPE_WIDTH < bird.poluchit_ramku().left)
                {
                    pipes[i].set_proshla(true);
                    score++;
                }
            }

            // столкновения с трубами
            for (size_t i = 0; i < pipes.size(); i++) {
                if (bird.poluchit_ramku().intersects(pipes[i].verhnyaya_ramka()) ||
                    bird.poluchit_ramku().intersects(pipes[i].nizhnyaya_ramka()))
                {
                    state = GameState::GAME_OVER;
                }
            }

            // столкновение с землёй
            sf::FloatRect br = bird.poluchit_ramku();
            if (br.top + br.height > groundY)
                state = GameState::GAME_OVER;
        }

        // ===== ОТРИСОВКА =====
        window.clear();

        window.draw(bgSprite);

        if (state != GameState::MENU) {
            for (size_t i = 0; i < pipes.size(); i++) {
                pipes[i].draw(window);
            }
        }

        bird.draw(window);
        window.draw(baseSprite);

        if (state == GameState::MENU) {
            window.draw(messageSprite);
        }
        else if (state == GameState::PLAYING) {
            drawScore(window, digitTextures, score, 40.0f);
        }
        else if (state == GameState::GAME_OVER) {
            window.draw(gameoverSprite);
            drawScore(window, digitTextures, score, 270.0f);
        }

        window.display();
    }

    return 0;
}