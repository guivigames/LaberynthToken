#pragma onece
#ifndef PLAYER_H
#define PLAYER_H


#include <string>
#include <SFML/Graphics.hpp>


class Player
{
protected:
    sf::Vector2f m_pos;         //
    sf::Vector2f m_vel;         //
    sf::Vector2f m_size;        //
    sf::Sprite m_strip;         //
public:
    Player(sf::Vector2f _pos, sf::Vector2f _size);  //
    ~Player();                          //
    sf::Sprite& GetStripe();            //
    void SetPos(sf::Vector2f _pos);     //
    void SetVel(sf::Vector2f _vel);     //
    sf::Vector2f GetPos();              //
    sf::Vector2f GetVel();              //
    
};


#endif