#include "Images.h"
#include "map.h"

//////////////////////////////////////////////////
void drawlevel() {
 uint8_t x;
 uint8_t y;
 for(y = 0; y <4 ; y++) {
  switch(y)
  {
    case 0:
    {
      for ( x = 0; x < WORLD_WIDTH; x++) {
      switch(world[y][x])
      {
      case BLOCK:
      Sprites::drawOverwrite(x * TILE_SIZE, 12, tiles, world[y][x]);
      break;
      case AIR:
      break;
      }
    }
  }
  break;
//////
 case 1:
    {
      for ( x = 0; x < WORLD_WIDTH; x++) {
      switch(world[y][x])
      {
      case BLOCK:
      Sprites::drawOverwrite(x * TILE_SIZE, 28, tiles, world[y][x]);
      break;
      case AIR:
      break;
      }
    }
  }
  break;
///////
 case 2:
    {
      for ( x = 0; x < WORLD_WIDTH; x++) {
      switch(world[y][x])
      {
      case BLOCK:
      Sprites::drawOverwrite(x * TILE_SIZE, 44, tiles, world[y][x]);
      break;
      case AIR:
      break;
      }
    }
  }
  break;
/////
 case 3:
    {
      for ( x = 0; x < WORLD_WIDTH; x++) {
      switch(world[y][x])
      {
      case BLOCK:
      Sprites::drawOverwrite(x * TILE_SIZE, 60, tiles, world[y][x]);
      break;
      case AIR:
      break;
      }
    }
  }
  break;
//////
  }
 }
}


////////////////////////////////////////the end of drawlevel()////////////////////////////////
/// isSolid() function
bool isSolid(uint8_t tileIndex){
   switch(tileIndex){
      case AIR:
       return false;
      case BLOCK:
       return true;
      default:
       return false;
           }
     }
//////////////////////
// isPointInTile() function
bool isPointInTile(uint8_t x, uint8_t y) {
       if(y ==12){
            return isSolid(world[0][x/TILE_SIZE]);}
       if(y == 28) {
           return isSolid(world[1][x/TILE_SIZE]);}
       if( y == 44){
           return isSolid(world[2][x/TILE_SIZE]);}
       if( y == 60){
          return isSolid(world[3][x/TILE_SIZE]);}
        else
           return false;        
       }
