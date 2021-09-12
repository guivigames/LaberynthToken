#pragma onece
#ifndef MAP_H
#define MAP_H

#include <string>
#include "TextureManager.h"
#include <SFML/Graphics.hpp>

extern std::map<char, std::string> m_tokens;


enum TileType 
{
    Horyzontal = 0, 
    Vertical,
    TRight,
    TLeft,
    TUp,
    TDown,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};


/// @brief descriptor of tile.
class Tiles
{
public:
    sf::RenderTexture* m_renderTexture = nullptr;
    sf::Sprite m_sprite[2];
private:
    std::string m_tiles;
    void SetHoryzontal();
    void SetVertical();
    void SetTRight();
    void SetTLeft();
    void SetTUp();
    void SetTDown();
    void SetTopLeft();
    void SetTopRight();
    void SetBottomLeft();
    void SetBottomRight();
public:
    Tiles();
    ~Tiles();
    void Set(TileType type);
    std::string GetTiles();
    char GetTile(int x, int y);
    void MakeEnd();
    void RemoveToken();
    void PlaceToken(char i);
};


// Each icon/tile is 16 by 16 pixels. Each region will be 48 by 48 pixels.
// Making a map of 10 by 10 region will be a 480 by 480 piexels.

class GameMap
{
private:
    int m_width;
    int m_height;
    int m_pxWidth;
    int m_pxHeight;
    int m_totalwidth;
    int m_totalheight;
    uint8_t m_tokenCounter = 0;
    Tiles* m_tiles;
    std::string m_map;
    sf::Sprite m_MapStrip[900];

public:
    GameMap();
    ~GameMap();
    void CreateTiles();
    void CreateMap();
    int GetTotalWidth(){return m_totalwidth;};
    int GetTotalHeigt(){return m_totalheight;};
    int GetPxWidth(){ return m_pxWidth;};   ///< Return the width in pixels of the map.
    int GetPxHeight(){ return m_pxHeight;}; ///< Return the height in pixels of the map.
    int GetTokenCounter(void){ return m_tokenCounter;}; ///< Returns the number of tokens left.
    sf::Sprite& GetStripe(int x, int y);    ///< a strip in of a position x, y in the tile grid.
    char GetTile(int x, int y);         ///< get te tile as character.
    void SetTile(int x, int y, char c); ///< set the tile as character.
    void MoveTilesLeft(int x);          ///< Move the tiles to the left.
    void MoveTilesRight(int x);         ///< Move the tiles to the right.
    void MoveTilesDown(int i);          ///< Move the tiles down.
    void MoveTilesUp(int i);            ///< Move the tiles up.
};

#endif