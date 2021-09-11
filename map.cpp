#include "map.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

//typedef std::pair<std::string, std::string> pair;
static std::map<char, std::string> m_tokens;

/// TILES MEMBER FUNCTIONS ////////////////////////////////////////
void Tiles::SetHoryzontal(){m_tiles = "###XXX###";};
void Tiles::SetVertical(){  m_tiles = "#X##X##X#";};
void Tiles::SetTRight(){    m_tiles = "#X##XX#X#";};
void Tiles::SetTLeft(){     m_tiles = "#X#XX##X#";};
void Tiles::SetTUp(){       m_tiles = "#X#XXX###";};
void Tiles::SetTDown(){     m_tiles = "###XXX#X#";};
void Tiles::SetTopLeft(){   m_tiles = "####XX#X#";};
void Tiles::SetTopRight(){  m_tiles = "###XX##X#";};
void Tiles::SetBottomLeft(){m_tiles = "#X##XX###";};
void Tiles::SetBottomRight(){m_tiles = "#X#XX####";};
void Tiles::MakeEnd(){m_tiles[4] = 'Z';};
void Tiles::PlaceTocken(char i){ m_tiles[4] = ('A' + i);};
Tiles::Tiles()
{
    m_tokens['A'] = "bag.png";
    m_tokens['B'] = "book.png";
    m_tokens['C'] = "brain.png";
    m_tokens['D'] = "fruit.png";
    m_tokens['E'] = "garlic.png";
    m_tokens['F'] = "ginger.png";
    m_tokens['G'] = "grapes.png";
    m_tokens['H'] = "hat.png";
}
Tiles::~Tiles(){}
void Tiles::Set(TileType type)
{
    switch(type){
        case Horyzontal:
            SetHoryzontal();
        break;
        case Vertical:
            SetVertical();
        break;
        case TRight:
            SetTRight();
        break;
        case TLeft:
            SetTLeft();
        break;
        case TUp:
            SetTUp();
        break;
        case TDown:
            SetTDown();
        break;
        case TopLeft:
            SetTopLeft();
        break;
        case TopRight:
            SetTopRight();
        break;
        case BottomLeft:
            SetBottomLeft();
        break;
        case BottomRight:
            SetBottomRight();
        break;
        default:
            SetTopLeft();
    }
}
std::string Tiles::GetTiles(){return m_tiles;};
char Tiles::GetTile(int x, int y)
{
    if (x < 3 && y < 3)
        return m_tiles[ (3 * y) + x];
    else return '?';
}

/// MAP MEMBER FUNCTIONS //////////////////////////////////////////
/// @brief Default contructor
GameMap::GameMap()
{
    m_width     = 9;                ///< number of regions in the width direction.
    m_height    = 9;                ///< number of regions in the height direction.
    m_totalwidth    = m_width*3;    ///< numer of tiles on width.
    m_totalheight   = m_height*3;   ///< number of tiles on height.
    m_pxWidth   = m_width * 16;     ///< Width of the map in pixels.
    m_pxHeight  = m_height *16;     ///< height of the map in piels.
    
    CreateTiles();
    CreateMap();
}

/// @brief destructor
GameMap::~GameMap()
{
    delete [] m_tiles;
}

/// @brief creates all the tiles in all the regions. a region
/// is a 3 y 3 tiled area.
void GameMap::CreateTiles()
{
    srand(time(nullptr));
    
    m_tiles = new Tiles[m_width * m_height];

    for (int i = 0; i < m_width * m_height; i++){
        m_tiles[i].Set( (TileType)(rand() % 10));
        if (rand() % 100 < 20){
            m_tiles[i].PlaceTocken(rand()%m_tokens.size());
        }
    }
    m_tiles[(m_width * m_height) -1].MakeEnd();
}

/// @brief Makes the map on a single std::String of the generated tiles. 
/// This way it is much easier for searching later on.
void GameMap::CreateMap()
{
    m_map = "";

    for (int h = 0; h < m_height; h++){
        for (int y = 0; y < 3; y++){
            for (int w = 0; w < m_width; w++){
                for (int x = 0; x < 3; x++){
                    m_map += m_tiles[(h * m_width) + w].GetTile(x, y);
                    char cell = m_tiles[(h * m_width) + w].GetTile(x, y);
                    if ( cell >= 'A' &&  cell < ('A' + m_tokens.size())){
                        m_MapStrip[m_map.size()-1].setTexture( TextureManager::GetTexture( m_tokens[ cell]), true);//_texture.getTexture());
                        //printf("%c", cell);
                    }
                    else {
                        m_MapStrip[m_map.size()-1].setTexture(TextureManager::GetTexture("maps.png"));
                        if ( cell == '#'){
                            m_MapStrip[m_map.size()-1].setTextureRect(sf::IntRect(16, 16, 16, 16));
                            //printf("wall\r\n");
                        }
                        else if ( cell == 'X'){
                            m_MapStrip[m_map.size()-1].setTextureRect(sf::IntRect( 0, 32, 16, 16));
                            //printf("Path\r\n");
                        }
                        else if ( cell == 'Z'){
                            m_MapStrip[m_map.size()-1].setTextureRect(sf::IntRect(32, 0, 16, 16));
                            //printf("Gate\r\n");
                        }
                    }
                }
            }
        }
    }
}

/// @brief Gets the stripe of the tile for drawing
sf::Sprite&  GameMap::GetStripe(int x, int y)
{
    return m_MapStrip[ (y * m_totalwidth) + x];
}

/// @brief Gets the chracte of the specific tile
char GameMap::GetTile(int x, int y)
{
    if (x < m_totalwidth && x < m_totalheight)
    {
        return m_map[(y * m_totalwidth) + x];
    }
    else return '?';
}

/// @brief Move the specific row one section left
void GameMap::MoveTilesLeft(int x)
{
    int h = (2*x)+1;
    // store first tile in row.
    Tiles temp = m_tiles[(h * m_width)];
    for(int w = 0; w < m_width-1; w++){
        m_tiles[(h * m_width) + w] = m_tiles[(h * m_width) + (w + 1)];
        //std::cout << (h * m_width) + w << " ";
    }
    //std::cout << x << " " << h << std::endl;
    m_tiles[(h * m_width) + (m_width-1)] = temp;
    CreateMap();
}

/// @brief Move the specific row one section Right
void GameMap::MoveTilesRight(int x)
{
    int h = (2*x)+1;
    // store first tile in row.
    Tiles temp = m_tiles[(h * m_width) + (m_width-1)];
    for(int w = m_width-1; w > 0; w--){
        m_tiles[(h * m_width) + w] = m_tiles[(h * m_width) + (w - 1)];
        //std::cout << (h * m_width) + w << " ";
    }
    //std::cout << x << " " << h << std::endl;
    m_tiles[(h * m_width)] = temp;
    CreateMap();
}

/// @brief Move the specific row one section Down
void GameMap::MoveTilesDown(int i)
{
    int x = (2*i)+1;
    // store first tile in row.
    int l = ((m_height * m_width) - 1) - ((m_width-1)-x);
    Tiles last = m_tiles[l];
    for(int w = l; w > x; w -= m_width){
        m_tiles[w] = m_tiles[w - m_width];
        //std::cout << (h * m_width) + w << " ";
    }
    //std::cout << x << " " << h << std::endl;
    m_tiles[x] = last;
    CreateMap();
}

/// @brief Move the specific row one section Down
void GameMap::MoveTilesUp(int i)
{
    int x = (2*i)+1;
    // store first tile in row.
    int l = ((m_height * m_width) - 1) - ((m_width-1)-x);
    Tiles first = m_tiles[x];
    for(int w = x; w < l; w += m_width){
        m_tiles[w] = m_tiles[w + m_width];
        //std::cout << (h * m_width) + w << " ";
    }
    //std::cout << x << " " << h << std::endl;
    m_tiles[l] = first;
    CreateMap();
}

