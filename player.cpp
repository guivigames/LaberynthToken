#include "player.hpp"
#include "TextureManager.h"

/// @brief PLayer contructor
Player::Player(sf::Vector2f _pos, sf::Vector2f _size)
{
    m_pos = _pos;
    m_size = _size;
    m_strip.setTexture( TextureManager::GetTexture("player.png"));
}

/// @brief player destructor
Player::~Player()
{}

/// @brief Get the strip of the player.
sf::Sprite& Player::GetStripe()
{
    return m_strip;
}

/// @brief set the player position.
void Player::SetPos(sf::Vector2f _pos)
{ 
    m_pos = _pos;
}

/// @brief set the player velocity.
void Player::SetVel(sf::Vector2f _vel)
{ 
    m_vel = _vel;
}

/// @brief Get the plauer position
sf::Vector2f Player::GetPos()
{
    return m_pos;
}

/// @brief Get the player velocity
sf::Vector2f Player::GetVel()
{
    return m_vel;
}

