#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <memory>
#include "TextureManager.h"
#include "map.h"
#include "player.hpp"
#include <SFML/Audio.hpp>

///< good old globals.
TextureManager g_textureManager;

const float fps = 60;
const float period = 1000000/fps;
const int width = 800;
const int height = 600;
const float radius = 8;
const float fRadius = 0.5;

///< My definitions of SFML vector arithmetic.
template <typename T>
sf::Vector2<T> operator*(const sf::Vector2<T>& left, const sf::Vector2<T>& right)
{
    T X = left.x * right.x;
    T Y = left.y * right.y;
    return sf::Vector2<T>(X,Y);
}
template <typename T>
sf::Vector2<T> operator*(const sf::Vector2<T>& left, const T right)
{
    T X = left.x * right;
    T Y = left.y * right;
    return sf::Vector2<T>(X,Y);
}
template <typename T>
sf::Vector2i floor(const sf::Vector2<T>& vector)
{
    int X = floor(vector.x);
    int Y = floor(vector.y);
    return sf::Vector2i(X,Y);
}

///< Algorithms for object collition.
// CIRCLE/RECTANGLE
bool circleRect(float cx, float cy, float rad, float rx, float ry, float rw, float rh) 
{
  // temporary variables to set edges for testing
  float testX = cx;
  float testY = cy;

  // which edge is closest?
  if (cx < rx)         testX = rx;      // test left edge
  else if (cx > rx+rw) testX = rx+rw;   // right edge
  if (cy < ry)         testY = ry;      // top edge
  else if (cy > ry+rh) testY = ry+rh;   // bottom edge

  // get distance from closest edges
  float distX = cx-testX;
  float distY = cy-testY;
  float distance = sqrt( (distX*distX) + (distY*distY) );

  // if the distance is less than the radius, collision!
  if (distance+0.01 < rad) {
    return true;
  }
  return false;
}
// CIRCLE/CIRCLE
bool circleCircle(float c1x, float c1y, float c1r, float c2x, float c2y, float c2r) {

  // get distance between the circle's centers
  // use the Pythagorean Theorem to compute the distance
  float distX = c1x - c2x;
  float distY = c1y - c2y;
  float distance = sqrt( (distX*distX) + (distY*distY) );

  // if the distance is less than the sum of the circle's
  // radii, the circles are touching!
  if (distance <= c1r+c2r) {
    return true;
  }
  return false;
}
// TRIANGLE/POINT
bool triPoint(float x1, float y1, float x2, float y2, float x3, float y3, float px, float py) {

  // get the area of the triangle
  float areaOrig = abs( (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1) );

  // get the area of 3 triangles made between the point
  // and the corners of the triangle
  float area1 =    abs( (x1-px)*(y2-py) - (x2-px)*(y1-py) );
  float area2 =    abs( (x2-px)*(y3-py) - (x3-px)*(y2-py) );
  float area3 =    abs( (x3-px)*(y1-py) - (x1-px)*(y3-py) );

  // if the sum of the three areas equals the original,
  // we're inside the triangle!
  if (area1 + area2 + area3 == areaOrig) {
    return true;
  }
  return false;
}

///< Object definitions.
//struct player
//{
//    sf::Vector2f m_vPos;
//    sf::Vector2f m_vVel;
//    float fRadius = 0.5;
//};



//////////////////////////////
int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode( 800, 600), "Laberynth");
    sf::Text text;
    sf::Font font;
    font.loadFromFile("Pacifico.ttf");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::White);

    sf::Clock timer;
    sf::Clock gameTime;
    sf::Clock mouseRespond;

    GameMap *_map = new GameMap();
    sf::ConvexShape _arrows[4][5];
    Player* _playerOne;
    sf::Vector2f _scale = {16.0f, 16.0f};       ///< pixel size of each square.
    sf::Vector2f _mapOffest = {3.0, 3.0};      
    int co = 0; ///< gernaral purpose counter.
    int numarrows = 0;

    sf::SoundBuffer _buffer;
    if(!_buffer.loadFromFile("wall-crushing2.wav"))
        return -1;
    sf::Sound sound;
    sound.setBuffer(_buffer);

    ///< Set arrow positions.
    {
        co = 0;
        int pwHeight = _map->GetTotalHeigt();
        for (int i = 3; i < pwHeight; i+= 6){
            int row = i;
            int halfrow = 1.5;
            _arrows[0][co].setPointCount(3);
            _arrows[0][co].setPoint(0, sf::Vector2f( 1, _mapOffest.y+row)*_scale);
            _arrows[0][co].setPoint(1, sf::Vector2f( 2, _mapOffest.y+row+halfrow)*_scale);
            _arrows[0][co].setPoint(2, sf::Vector2f( 1, _mapOffest.y+row+(2*halfrow))* _scale);
            _arrows[0][co].setFillColor(sf::Color::Green);
            co++;
        }
        co = 0;
        for (int i = 3; i < pwHeight; i+= 6){
            int row = i;
            int halfrow = 1.5;
            _arrows[1][co].setPointCount(3);
            _arrows[1][co].setPoint(0, sf::Vector2f( 2 + _mapOffest.x + _map->GetTotalWidth(), _mapOffest.y+row)*_scale);
            _arrows[1][co].setPoint(1, sf::Vector2f( 1 + _mapOffest.x + _map->GetTotalWidth(), _mapOffest.y+row+halfrow)*_scale);
            _arrows[1][co].setPoint(2, sf::Vector2f( 2 + _mapOffest.x + _map->GetTotalWidth(), _mapOffest.y+row+(2*halfrow))* _scale);
            _arrows[1][co].setFillColor(sf::Color::Green);
            co++;
        }
        co = 0;
        int pwWidth = _map->GetTotalWidth();
        for (int i = 3; i < pwWidth; i+= 6){
            int col = i;
            int halfcol = 1.5;
            _arrows[2][co].setPointCount(3);
            _arrows[2][co].setPoint(0, sf::Vector2f( _mapOffest.x+col, 1)*_scale);
            _arrows[2][co].setPoint(1, sf::Vector2f( _mapOffest.x+col+halfcol, 2)*_scale);
            _arrows[2][co].setPoint(2, sf::Vector2f( _mapOffest.x+col+(2*halfcol), 1)* _scale);
            _arrows[2][co].setFillColor(sf::Color::Green);
            co++;
        }
        co = 0;
        for (int i = 3; i < pwWidth; i+= 6){
            int col = i;
            int halfcol = 1.5;
            _arrows[3][co].setPointCount(3);
            _arrows[3][co].setPoint(0, sf::Vector2f( _mapOffest.x + col, 2 + _mapOffest.y + _map->GetTotalHeigt())*_scale);
            _arrows[3][co].setPoint(1, sf::Vector2f(  _mapOffest.x+col+halfcol, 1 + _mapOffest.y + _map->GetTotalHeigt())*_scale);
            _arrows[3][co].setPoint(2, sf::Vector2f( _mapOffest.x+col+(2*halfcol),  2 + _mapOffest.y + _map->GetTotalHeigt())* _scale);
            _arrows[3][co].setFillColor(sf::Color::Green);
            co++;
        }
        numarrows = co;
    }
    
    ///< Player Initial position.
    _playerOne = new Player( sf::Vector2f( 1.0 + fRadius, 1.0 + fRadius), sf::Vector2f(1.0, 1.0));

    int score = 0;
    bool isGameOver = false;
    while (window.isOpen())
    {
        /// Event Haneling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ///< Make a new map if space bar is pressed.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            delete _map;
            _map = new GameMap();
            _playerOne->SetPos(sf::Vector2f(1.0 + fRadius, 1.0 + fRadius));
            isGameOver = false;
            score = 0;
            sf::sleep(sf::microseconds(500)); ///< Delay for GUI control.
        }

        if (!isGameOver){
            /// Control Player Oject
            sf::Time dt = timer.restart();  ///< Frame rate controller.
            _playerOne->SetVel({0.0, 0.0});
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) _playerOne->SetVel( _playerOne->GetVel() + sf::Vector2f( 0.0f, -1.0f));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) _playerOne->SetVel( _playerOne->GetVel() + sf::Vector2f( 0.0f, 1.0f));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _playerOne->SetVel( _playerOne->GetVel() + sf::Vector2f(-1.0f, 0.0f));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _playerOne->SetVel( _playerOne->GetVel() + sf::Vector2f( 1.0f, 0.0f));

            /// Where will object be
            sf::Vector2f vPotentilPos;// = _playerOne->GetPos() + (_playerOne->GetVel() * (5 * dt.asSeconds()));
            vPotentilPos.x = _playerOne->GetPos().x + (_playerOne->GetVel().x * (5 * dt.asSeconds()));
            vPotentilPos.y = _playerOne->GetPos().y + (_playerOne->GetVel().y * (5 * dt.asSeconds()));

            sf::Vector2i currentCell = floor(_playerOne->GetPos());
            sf::Vector2i targetCell = floor(vPotentilPos);
            sf::Vector2i vAreaTL = {std::max( std::min(currentCell.x, targetCell.x)-1, 0), 
                                    std::max( std::min(currentCell.y, targetCell.y)-1, 0)};
            sf::Vector2i vAreaBR = {std::min( std::max(currentCell.x, targetCell.x)+1, _map->GetTotalWidth()), 
                                    std::min( std::max(currentCell.y, targetCell.y)+1, _map->GetTotalHeigt())};
            sf::Vector2f vRayToNearest;
            sf::Vector2i vCell;

            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++){
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++){
                    char cr =_map->GetTile( vCell.x, vCell.y);
                    if (cr == '#'){
                        sf::Vector2f vNearestPoint;
                        vNearestPoint.x = std::max( (float)vCell.x, std::min(vPotentilPos.x, (float)vCell.x+1));
                        vNearestPoint.y = std::max( (float)vCell.y, std::min(vPotentilPos.y, (float)vCell.y+1));

                        sf::Vector2f vRayToNearest = vNearestPoint - vPotentilPos;
                        float fMag = pow(pow(vRayToNearest.x, 2)+pow(vRayToNearest.y, 2), 0.5);
                        float fOverlap = fRadius - fMag;
                        if (std::isnan(fOverlap)) fOverlap = 0;
                        if (fOverlap > 0)
                        {
                            vPotentilPos.x = vPotentilPos.x - (vRayToNearest.x/fMag)*fOverlap;
                            vPotentilPos.y = vPotentilPos.y - (vRayToNearest.y/fMag)*fOverlap;
                        }
                    }
                }
            }
            if (vPotentilPos.x > 0 && vPotentilPos.x < _map->GetTotalWidth())
                _playerOne->SetPos( {vPotentilPos.x, _playerOne->GetPos().y});// - _playerOne.fRadius;
            if (vPotentilPos.y > 0 && vPotentilPos.y < _map->GetTotalHeigt())
                _playerOne->SetPos( { _playerOne->GetPos().x, vPotentilPos.y});//- _playerOne.fRadius;

            if (circleRect(_playerOne->GetPos().x+0.5, _playerOne->GetPos().y+0.5, 0.5, 
            _map->GetTotalWidth()-1, _map->GetTotalHeigt()-1, 1, 1))
                isGameOver = true;
        }

        ///< Draw Everyting.
        window.clear();
        
        for (int y = 0; y < _map->GetTotalHeigt(); y++){
            for (int x = 0; x < _map->GetTotalWidth(); x++){
                auto& sprite = _map->GetStripe(x, y);
                sprite.setPosition(sf::Vector2f(x*_scale.x, y*_scale.y)+(_mapOffest*_scale)); // absolute position
                window.draw(sprite);
            }
        }

        /// Draw player
        {
            auto& sprite = _playerOne->GetStripe();
            sprite.setPosition( sf::Vector2f((_playerOne->GetPos().x - fRadius)*_scale.x, 
                                (_playerOne->GetPos().y - fRadius)*_scale.y) + (_mapOffest*_scale)); // absolute position
            window.draw(sprite);
        }

        // Get the current mouse position in the window.
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        // convert it to world coordinates.
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        // Draw Arrows and move the map if any arrow is activated.
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < numarrows; i++)
            {
                if (triPoint(_arrows[j][i].getPoint(0).x, _arrows[j][i].getPoint(0).y, _arrows[j][i].getPoint(1).x,
                _arrows[j][i].getPoint(1).y, _arrows[j][i].getPoint(2).x, _arrows[j][i].getPoint(2).y, worldPos.x, worldPos.y)){
                    _arrows[j][i].setFillColor(sf::Color::Red);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                    {
                        sf::Time dt = mouseRespond.getElapsedTime();  ///< Frame rate controller.
                        if (dt.asMilliseconds() > 250){
                            if (!isGameOver)
                                score++;        /// Increase the score.
                                sound.play();   /// Play the wall sound.
                            switch(j){
                            case 0:
                                _map->MoveTilesRight(i);
                                break;
                            case 1:
                                _map->MoveTilesLeft(i);
                                break;
                            case 2:
                                _map->MoveTilesDown(i);
                                break;
                            case 3:
                                _map->MoveTilesUp(i);
                                break;
                            }
                            mouseRespond.restart();
                        }
                    }
                }
                else 
                    _arrows[j][i].setFillColor(sf::Color::Green);
                window.draw(_arrows[j][i]);
            }

        /// Display text as per game state.
        if (!isGameOver){
            text.setString("Score: " + std::to_string(score));
            text.setPosition(width-250, 5);
            window.draw(text);
        }
        else {
            text.setString("Final Score: \r\n" + std::to_string(score) + "\r\nPress down \n\rspace bar\r\nto restart.");
            text.setPosition(width-250, 5);
            window.draw(text);
        }
        
        /// Display all the drawing in the screen.
        window.display();
        
    }
    return 0;
}

