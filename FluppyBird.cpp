#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include "Bird.h"
#include "Pipe.h"

enum class Difficulty { EASY, NORMAL, HARD };

struct GameSettings {
    float gravity;
    float jumpForce;
    float pipeGap;
    float pipeSpeed;
    const char* name;
};

const GameSettings SETTINGS[3] = {
    {650.0f, -240.0f, 190.0f, 120.0f, "EASY"},
    {900.0f, -300.0f, 150.0f, 160.0f, "NORMAL"},
    {1150.0f,-280.0f, 110.0f, 220.0f, "HARD"}
};

enum class GameState { MENU, PLAYING, GAME_OVER };


static void drawScore(sf::RenderWindow& window, sf::Texture digitTextures[10], int scoreVal, float yPos) {
    std::string s = std::to_string(scoreVal);

    float totalW = 0;
    for (size_t i = 0; i < s.size(); i++) {
        //общая ширина всех цифр 
        totalW += digitTextures[s[i] - '0'].getSize().x * 2.0f; //увеличиваем спрайт в два раза
    }

    float x = (400.0f - totalW) / 2.0f;  //центрирование
    for (size_t i = 0; i < s.size(); i++) {                                          
        sf::Sprite sp(digitTextures[s[i] - '0']);  //временный спрайт для тек цифры
        sp.setScale(2.0f, 2.0f);             
        sp.setPosition(x, yPos);             
        window.draw(sp);
        x += digitTextures[s[i] - '0'].getSize().x * 2.0f;
    }
}

static void resetGame(Bird& bird, std::vector<Pipe>& pipes, float PIPE_DISTANCE, float groundY) { 
    bird.reset();
    float x = 500.0f; //стартовая позиция первой трубы (за пределами)
    for (int i = 0; i < 4; i++) {
        float gap_y = 100.0f + static_cast<float>(rand() % 301); //y от 100 до 400
        pipes[i].spawn(x, gap_y, groundY); 
        x += PIPE_DISTANCE;
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(400, 600), "Flappy Bird");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));


    // ЗАГРУЗКА ТЕКСТУР 
    sf::Texture bgTexture, baseTexture, messageTexture, gameoverTexture;
    sf::Texture digitTextures[10];

    bgTexture.loadFromFile("day.png");
    baseTexture.loadFromFile("base.png");
    messageTexture.loadFromFile("message.png");
    gameoverTexture.loadFromFile("gameover.png");
    for (int i = 0; i < 10; i++)
        digitTextures[i].loadFromFile(std::to_string(i) + ".png");

    sf::Font font;
    font.loadFromFile("PressStart2P-Regular.ttf");

    int highScore = 0;
    std::ifstream loadFile("highscore.txt");
    if (loadFile.is_open()) {
        loadFile >> highScore;   //чтение файла
        loadFile.close();
    }


    sf::Text bestLabel("BEST", font, 14);
    bestLabel.setFillColor(sf::Color(255, 255, 255));
    bestLabel.setOutlineColor(sf::Color(0, 0, 0));   //обводка
    bestLabel.setOutlineThickness(2.0f);    //толщина обводки
    bestLabel.setOrigin(bestLabel.getLocalBounds().width / 2.0f, 0.0f);   //центрирование
    bestLabel.setPosition(200.0f, 400.0f);

    sf::Text bestScoreText(std::to_string(highScore), font, 18);
    bestScoreText.setFillColor(sf::Color(255, 215, 0)); 
    bestScoreText.setOutlineColor(sf::Color::Black);
    bestScoreText.setOutlineThickness(2.0f);
    bestScoreText.setOrigin(bestScoreText.getLocalBounds().width / 2.0f, 0.0f);
    bestScoreText.setPosition(200.0f, 430.0f);

    sf::Text hintSkin("LEFT / RIGHT buttons for changing the skin", font, 8); //прижатие к левому верхнему углу с отступом 15px
    hintSkin.setFillColor(sf::Color::White);
    hintSkin.setOutlineColor(sf::Color::Black);
    hintSkin.setOutlineThickness(1.5f);
    hintSkin.setPosition(15.0f, 15.0f); 


    // НАСТРОЙКА СПРАЙТОВ 
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(400.0f / bgTexture.getSize().x, 600.0f / bgTexture.getSize().y); 

    float baseScale = 400.0f / (float)baseTexture.getSize().x; 
    float baseHeight = baseTexture.getSize().y * baseScale;
    float groundY = 600.0f - baseHeight; 

    sf::Sprite baseSprite(baseTexture);
    baseSprite.setScale(baseScale, baseScale);
    baseSprite.setPosition(0, groundY);

    sf::Sprite messageSprite(messageTexture);
    {
        sf::FloatRect b = messageSprite.getLocalBounds();
        messageSprite.setOrigin(b.width / 2.0f, b.height / 2.0f); //точка привязки - центр спрайта
        messageSprite.setPosition(200.0f, 270.0f);
    }

    sf::Sprite gameoverSprite(gameoverTexture);
    {
        sf::FloatRect b = gameoverSprite.getLocalBounds();
        gameoverSprite.setOrigin(b.width / 2.0f, b.height / 2.0f);
        gameoverSprite.setPosition(200.0f, 200.0f);
    }


    // ИГРОВЫЕ ОБЪЕКТЫ
    int selectedSkin = 1; 
    std::ifstream loadSkin("skin.txt");
    if (loadSkin.is_open()) {
        loadSkin >> selectedSkin;
        loadSkin.close();
    }
    if (selectedSkin < 0 || selectedSkin > 2) selectedSkin = 1;


    Difficulty currentDiff = Difficulty::NORMAL; 

    sf::Text diffText(SETTINGS[static_cast<int>(currentDiff)].name, font, 16);

    diffText.setFillColor(sf::Color::Yellow);
    diffText.setOutlineColor(sf::Color::Black);
    diffText.setOutlineThickness(1.5f);
    diffText.setOrigin(diffText.getLocalBounds().width / 2.0f, 0.0f);
    diffText.setPosition(100.0f, 500.0f);


    Bird bird;
    bird.setSkin(selectedSkin); 

    std::vector<Pipe> pipes;
    for (int i = 0; i < 4; i++)
        pipes.push_back(Pipe());

    const float PIPE_DISTANCE = 230.0f;
    const float PIPE_WIDTH = 52.0f;


    GameState state = GameState::MENU;  
    int score = 0;
    float x = 500.0f; 
    for (int i = 0; i < 4; i++) {
        float gap_y = 100.0f + static_cast<float>(rand() % 301);
        pipes[i].spawn(x, gap_y, groundY);
        x += PIPE_DISTANCE;
    }

    sf::Clock clock;

    // ИГРОВОЙ ЦИКЛ 
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds(); 

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) 
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
            {
                if (state == GameState::MENU) {
                    //физика под тек сложность
                    bird.setPhysics(SETTINGS[static_cast<int>(currentDiff)].gravity,
                        SETTINGS[static_cast<int>(currentDiff)].jumpForce);

                    for (auto& p : pipes) {
                        p.setGap(SETTINGS[static_cast<int>(currentDiff)].pipeGap);
                        p.setSpeed(SETTINGS[static_cast<int>(currentDiff)].pipeSpeed); 
                    }

                    float x = 500.0f;
                    for (int i = 0; i < 4; i++) {
                        float gap_y = 100.0f + static_cast<float>(rand() % 301);
                        pipes[i].spawn(x, gap_y, groundY);  
                        x += PIPE_DISTANCE;
                    }

                    state = GameState::PLAYING;  //переключение состояния
                    bird.jump();
                } else if (state == GameState::PLAYING) {
                    bird.jump();
                } else if (state == GameState::GAME_OVER) {
                    resetGame(bird, pipes, PIPE_DISTANCE, groundY);  //сброс
                    score = 0;
                    state = GameState::MENU;  //возврат в меню
                }
            }
            if (state == GameState::MENU && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    selectedSkin--;
                    if (selectedSkin < 0) selectedSkin = 2;  //циклический переход
                    bird.setSkin(selectedSkin);
                    std::ofstream saveSkin("skin.txt");
                    if (saveSkin.is_open()) { 
                        saveSkin << selectedSkin;  //перезаписываем
                        saveSkin.close();
                    }
                }
                if (event.key.code == sf::Keyboard::Right) {
                    selectedSkin++;
                    if (selectedSkin > 2) selectedSkin = 0;
                    bird.setSkin(selectedSkin);
                    std::ofstream saveSkin("skin.txt");
                    if (saveSkin.is_open()) { 
                        saveSkin << selectedSkin;
                        saveSkin.close(); 
}
                }
                   if (event.key.code == sf::Keyboard::Up) {
                       if (currentDiff == Difficulty::EASY) 
                           currentDiff = Difficulty::HARD; 
                       else currentDiff = static_cast<Difficulty>(static_cast<int>(currentDiff) - 1);
                       diffText.setString(SETTINGS[static_cast<int>(currentDiff)].name);
                   }
                   if (event.key.code == sf::Keyboard::Down) {
                       if (currentDiff == Difficulty::HARD) currentDiff = Difficulty::EASY;
                       else currentDiff = static_cast<Difficulty>(static_cast<int>(currentDiff) + 1);
                       diffText.setString(SETTINGS[static_cast<int>(currentDiff)].name);
                   }
               
            }
        }

        // ОБНОВЛЕНИЕ ЛОГИКИ 
        if (state == GameState::PLAYING) {
            bird.update(dt);
            for (size_t i = 0; i < pipes.size(); i++) {
                pipes[i].update(dt);
            }

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

            // СЧЁТ
            for (size_t i = 0; i < pipes.size(); i++) {
                if (!pipes[i].get_proshla() && pipes[i].getX() + PIPE_WIDTH < bird.poluchit_ramku().left)
                {
                    pipes[i].set_proshla(true);
                    score++;
                }
            }

            for (size_t i = 0; i < pipes.size(); i++) {
                if (bird.poluchit_ramku().intersects(pipes[i].verhnyaya_ramka()) ||
                    bird.poluchit_ramku().intersects(pipes[i].nizhnyaya_ramka()))
                {
                    state = GameState::GAME_OVER;

                    if (currentDiff != Difficulty::EASY && score > highScore) {
                        highScore = score;
                        bestScoreText.setString(std::to_string(highScore));
                        std::ofstream saveFile("highscore.txt");
                        if (saveFile.is_open()) { saveFile << highScore; saveFile.close(); }
                    }
                }
            }

            sf::FloatRect br = bird.poluchit_ramku();
            if (br.top + br.height > groundY) {
                state = GameState::GAME_OVER;

                if (currentDiff != Difficulty::EASY && score > highScore) {
                    highScore = score;
                    bestScoreText.setString(std::to_string(highScore));
                    std::ofstream saveFile("highscore.txt");
                    if (saveFile.is_open()) { saveFile << highScore; saveFile.close(); }
                }
            }
        }

        // ОТРИСОВКА
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
            window.draw(hintSkin);
            window.draw(diffText);
        } else if (state == GameState::PLAYING) {
            drawScore(window, digitTextures, score, 40.0f);
        } else if (state == GameState::GAME_OVER) {
            window.draw(gameoverSprite);
            drawScore(window, digitTextures, score, 270.0f);

            if (currentDiff != Difficulty::EASY) {
                window.draw(bestLabel);
                window.draw(bestScoreText);
            } else {

                sf::Text easyMsg("EASY MODE - NO RECORDS", font, 12);
                easyMsg.setFillColor(sf::Color::Black);
                easyMsg.setOrigin(easyMsg.getLocalBounds().width / 2, 0);
                easyMsg.setPosition(200.0f, 500.0f);
                window.draw(easyMsg);
            }
        }

        window.display();
    }
    return 0;
}