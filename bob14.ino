#include <Arduboy2.h>
#include <ArduboyTones.h>
//////////////
#include <stdint.h>
#include "Images.h"
#include "map.h"
#include "sound.h"
#define BOB_GROUND_LEVEL          60
#define NUMBER_OF_BUBBLES 7
//////////////////////////////
//BeepPin1 beep; // class instacne for speaker pin 1
uint8_t count_bubble = 0;
/////////////////////////////////
////////////////적만들기시작/////////////////////////////////////////////////////////////////////////////////////////////

#define NUMBER_OF_OBSTACLES 4
#define OBSTACLE_LAUNCH_DELAY_MIN   90
#define OBSTACLE_LAUNCH_DELAY_MAX   200

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class GameStatus : uint8_t {
  Introduction,
  PlayGame,
  GameOver,
};
//////////////////
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
/////////////////////////////////////////////////////////////////
enum class Stance : uint8_t {
  R_Standing,
  R_Running1,
  R_Running2,
  R_Bubble,
  L_Running1,
  L_Running2,
  L_Bubble,
  Dead,
};


struct Bob {
  uint8_t x;
  uint8_t y;
  Stance stance;
  bool jumping;
  uint8_t jumpIndex;
  bool falling;
  uint8_t fallIndex;
  const uint8_t *image;
  const uint8_t *mask;
};

////////////
enum class BubbleType : uint8_t {
  R_Shoot,
  L_Shoot,
  Captured,
  L_Captured,
  Up_Bubble,
  Pop_Bubble,
};
//////////////////////////

//////////////////////////
struct Bubble{
  int8_t x;
  int8_t y;
  BubbleType b_type;
  bool shooting;  
  const uint8_t *image;
};

///////////////////////
//////////////////////////적만들기 ObstacleType//////////////////////////////////////////////////////////////////////////////////////////

enum class ObstacleType: uint8_t {
  Benzo1,
  Benzo2,
  Bonnie1,
  Bonnie2,
  Boa1,
  Boa2,
  Blubba1, 
  Blubba2,
  Count_AllObstacles = 7,
};

/////////////////////Obstacle struct///////////////////////////////////////////////////////////////////////////////////////////

struct Obstacle {
  int8_t x;
  int8_t y;
  ObstacleType type;
  bool enabled;
  bool jumping;
  uint8_t jumpIndex;
  bool falling;
  uint8_t fallIndex;
  const uint8_t *image;  
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
//uint8_t groundX = 0;

Bob bob = {0, BOB_GROUND_LEVEL, Stance::R_Standing, false, false, false, false, bob_R_still, bob_R_still_mask };

uint8_t jumpCoords[] = { 3, 3, 2, 2, 2, 1, 1, 1, 1};

uint8_t fallCoords[] = { 3, 2, 1, 2, 2, 1, 2, 2, 1};

GameStatus gameStatus = GameStatus::Introduction;

////////////////////////////////////////////6개만 생성

Bubble bubbles[NUMBER_OF_BUBBLES]{
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
  {1,1,BubbleType::R_Shoot,false, bubble_R_shoot},
};

/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////적을 만들자////////////////

Obstacle obstacles[NUMBER_OF_OBSTACLES] = {
  {0,0, ObstacleType::Benzo1, false, false, false,false,false, benzo_1},
  {0,0, ObstacleType::Benzo1, false, false, false,false,false, benzo_1},
  {0,0, ObstacleType::Benzo1, false, false, false,false,false, benzo_1},
  {0,0, ObstacleType::Benzo1, false, false, false,false,false, benzo_1},
};

uint16_t obstacleLaunchCountdown = OBSTACLE_LAUNCH_DELAY_MIN;
//uint16_t obstacleDelayCountdown = OBSTACLE_LAUNCH_DELAY_MIN;

uint16_t point = 10;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t *bob_images[] = { bob_R_still,bob_R_running_1,bob_R_running_2,bob_R_bubble,bob_L_running_1,bob_L_running_2,bob_L_bubble,bob_dead };

const uint8_t *bob_masks[] = { bob_R_still_mask, bob_R_running_1_mask, bob_R_running_2_mask, bob_R_bubble_mask,  bob_L_running_1_mask, bob_L_running_2_mask, bob_L_bubble_mask, bob_dead_mask };

//////////////
const uint8_t *bubble_images[] = { bubble_R_shoot, bubble_L_shoot, bubble_R_captured, bubble_L_captured, bubble_pop };
/////////////////////////
///////////////////////// 적을 만들자//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t *obstacle_images[] = { benzo_1, benzo_2, bonnie_1, bonnie_2, boa_1, boa_2, blubba_1, blubba_2 };
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Setup the environment ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void setup() { 
  arduboy.boot();
  arduboy.setFrameRate(75);
  arduboy.initRandomSeed();
  arduboy.audio.begin();
//  beep.begin();
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Control the various states of the game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {
  // Pause here until it's time for the next frame ..
  if (!(arduboy.nextFrame())) return;
  //beep.timer();
  arduboy.pollButtons();
  switch (gameStatus) {
    case GameStatus::Introduction:
      introduction();
      break;
    case GameStatus::PlayGame:
      playGame();
      break;
    case GameStatus::GameOver:
      gameOver();
      break;
  }
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Reset everything ready for a new game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void initialiseGame() {
  bob.x = 1;
  bob.y = BOB_GROUND_LEVEL;
  bob.jumping = false;
  bob.falling = false;
  bob.stance = Stance::R_Standing;
 /////////////////////initialise bubbles
  count_bubble = 0;
  for (uint8_t i = 0; i < NUMBER_OF_BUBBLES; i++) {
    bubbles[i].shooting = false;
  }

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////적들을 초기화 시킴/////////////
    for (uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    obstacles[i].x = WIDTH;
    obstacles[i].y = 12;
    obstacles[i].enabled = false;
    obstacles[i].jumping=false;
    obstacles[i].falling=false;
    obstacles[i].jumpIndex =0;
    obstacles[i].fallIndex = 0;
    obstacles[i].type = ObstacleType::Benzo1;

  }
////////////////////////////////////////////////////////////////////////////////////////////////
}

/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the introduction ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void introduction() {
  arduboy.clear();
  initialiseGame();
  /////////////////////////
  arduboy.audio.on();
  if (!sound.playing()) sound.tones(score);
  ///////////////////////
  arduboy.setCursor(17, 12);
  arduboy.print(F("Press A to begin"));
  drawBob();
  arduboy.display();
  if (arduboy.pressed(A_BUTTON)) {
    sound.tones(shooting_bubble);
    arduboy.audio.off(); 
    /////////////////////
    gameStatus = GameStatus::PlayGame;
    bob.stance = Stance::R_Running1;
  }
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Display the 'Game Over' text if Bob has hit an obstacle ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void gameOver() {
  arduboy.clear();
  drawBob();
  arduboy.setCursor(40, 12);
  arduboy.print(F("Game Over"));
  arduboy.audio.off();
  updateBob();
  arduboy.display();
  if (arduboy.justPressed(A_BUTTON)) {
    initialiseGame();
    gameStatus = GameStatus::PlayGame;
    bob.stance = Stance::R_Running1;
  }
}


/* -----------------------------------------------------------------------------------------------------------------------------
 *  Run Bob, run.
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void playGame() {
  arduboy.clear();
  arduboy.setCursor(0, 0);
  arduboy.print(count_bubble);
  arduboy.audio.on();
  if(point <100) {memcpy( world, world1, sizeof(uint8_t)*32*4);}
  if(point >100) {
    arduboy.clear();
    memcpy( world1, world2, sizeof(uint8_t)*32*4);}
  drawlevel();
 // The player can only control Bob if he is running  on the ground ..
  if (!bob.jumping&&!bob.falling) {
    if (arduboy.justPressed(UP_BUTTON)) { bob.jumping = true; bob.jumpIndex = 0; }
    if (arduboy.justPressed(B_BUTTON))  {
          if (bob.stance == Stance::R_Running1 || bob.stance == Stance::R_Running2) {
                 bob.stance = Stance::R_Bubble;
                 //beep.tone(beep.freq(1000),50);
                 
               if (!sound.playing()) sound.tones(shooting_bubble);
               if(count_bubble < 6 ){               
                     count_bubble++;
                     bubbles[count_bubble].shooting=true;
                     bubbles[count_bubble].b_type = BubbleType::R_Shoot;
                     bubbles[count_bubble].x = bob.x+8;
                     bubbles[count_bubble].y = bob.y;
                     drawBubbles();
                    }
                    updateBubbles();
                 }
                 if(count_bubble==6){count_bubble=0;}



          if (bob.stance == Stance::L_Running1 || bob.stance == Stance::L_Running2) {
                 bob.stance = Stance::L_Bubble;
                 if (!sound.playing()) sound.tones(shooting_bubble);
                 //beep.tone(beep.freq(1000),50);
                     if(count_bubble < 6 ){                
                     count_bubble++;                  
                     bubbles[count_bubble].shooting=true;
                     bubbles[count_bubble].b_type = BubbleType::L_Shoot;
                     bubbles[count_bubble].x = bob.x-8;
                     bubbles[count_bubble].y = bob.y;
                     drawBubbles();
                    }                
                    updateBubbles();      
                 }
                 if(count_bubble==6){count_bubble=0;}
               }

    if (arduboy.pressed(LEFT_BUTTON) && bob.x > 0 && !bob.falling )  {
       bob.stance = Stance::L_Running1;
       if(!isPointInTile(bob.x, bob.y)&&!isPointInTile(bob.x+8,bob.y)){bob.falling=true; bob.fallIndex = 0;}
       else
       bob.x--;
       }

    if (arduboy.pressed(RIGHT_BUTTON) && bob.x < 120&& !bob.falling )         {
        bob.stance = Stance::R_Running1;
        if(!isPointInTile(bob.x, bob.y)&& !isPointInTile(bob.x+8, bob.y)){bob.falling=true; bob.fallIndex =0;}

        else
        bob.x++;
        }

    // If the player has not pressed the B button (or continued to hold it down)
    // and Bob is bubbling, then return him to an running position ..

    if (arduboy.notPressed(B_BUTTON) && bob.stance == Stance::R_Bubble) {
      bob.stance = Stance::R_Running1;
    }


    if (arduboy.notPressed(B_BUTTON) && bob.stance == Stance::L_Bubble) {
      bob.stance = Stance::L_Running1;
    }
  }

//////////////////////////////////////////////////////////////////////////////////////////

// Should we launch another obstacle?

 
  --obstacleLaunchCountdown;
 
  if (obstacleLaunchCountdown == 0) {
    for (uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
      if (!obstacles[i].enabled) { 
        launchObstacle(i); 
        ////////////////////////////////////////////////////////////////////////////////////////여기서 부터.............................................................
        break;
      }
    }
    obstacleLaunchCountdown = random(OBSTACLE_LAUNCH_DELAY_MIN, OBSTACLE_LAUNCH_DELAY_MAX);           
  }
///////////////////////////////////////////////
  // Has Steve collided with anything?/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (collision()) {

    bob.jumping = false;
    bob.jumpIndex = 0;
    
    if (bob.stance <= Stance::R_Running2) {
      bob.stance = Stance::Dead;
    }
    else {
      bob.y = BOB_GROUND_LEVEL;
      bob.stance = Stance::Dead;
    }
    gameStatus = GameStatus::GameOver;

  }
  else {
////////////////////////////////////////
    updateBob();
    drawBob();
    updateBubbles();
    drawBubbles();
//////////////////////////////////적을만들자
    updateObstacles();
    drawObstacles();
//////////////////////////////////////////////////////////////////////////////////////////////////
    arduboy.display();

  }
}


void updateBob() {

  // Is Bob jumping ?

  if (bob.jumping) {
    bob.y = bob.y-jumpCoords[bob.jumpIndex];
    bob.jumpIndex++;
    if (bob.jumpIndex == sizeof(jumpCoords)) {
      //bob.y = BOB_GROUND_LEVEL;
       bob.jumping = false;
       bob.jumpIndex = 0;
       if(!isPointInTile(bob.x, bob.y)){
        bob.falling = true;  
        }  
    }
    else
     bob.falling=false;
  }
 // is Bob falling?

if (bob.falling){
    bob.y = bob.y+fallCoords[bob.fallIndex];
    bob.fallIndex++;
      if(bob.fallIndex == sizeof(fallCoords)) {
          if(isPointInTile(bob.x, bob.y)) {
            bob.falling = false;
            bob.fallIndex = 0;}
          else
            bob.fallIndex = 0;
          }
}

//

 else {

    // Swap the image to simulate running ..
    if (arduboy.everyXFrames(3)) {
      switch (bob.stance) {
       case Stance::R_Running1:
         bob.stance = Stance::R_Running2;
          break;
        case Stance::R_Running2:
          bob.stance = Stance::R_Running1;
        break;
        case Stance::L_Running1:
          bob.stance = Stance::L_Running2;
          break;
        case Stance::L_Running2:
          bob.stance = Stance::L_Running1;
          break;      
        default:
          break;
      }
    }
  }
}





/* -----------------------------------------------------------------------------------------------------------------------------
 *  Render Bob.
 *
 *  The standing and jumping and bubbling images are rendered relative to the ground, so the image height is subtracted from the current Y
 *  position to determine an upper top position.
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void drawBob() {
  uint8_t imageIndex = static_cast<uint8_t>(bob.stance);
  bob.image = bob_images[imageIndex];
  bob.mask = bob_masks[imageIndex];
  Sprites::drawExternalMask(bob.x, bob.y - getImageHeight(bob.image), bob.image, bob.mask, 0, 0);
}

//////////////////////////////////////// Render Bubble

void updateBubbles() {
  for (uint8_t i = 0; i < NUMBER_OF_BUBBLES; i++) {
    if (bubbles[i].shooting == true) {
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////4/13

      switch (bubbles[i].b_type) {
        case BubbleType::R_Shoot: 
          bubbles[i].x++;
          break;
        case BubbleType::L_Shoot:
          bubbles[i].x--;
          break;
       case BubbleType::Captured:    
          bubbles[i].y--;
          break;
      }
 ///////////////////////////////////////////////////////////////////////////////////////////////////////

      // Has the bubble moved out of view ?
        if (bubbles[i].x > 128 || bubbles[i].x<0||bubbles[i].y<0 ) {
          bubbles[i].shooting = false;
          if(count_bubble >0){count_bubble--;}
          }
    }
   
  }
}


///////////////////////// drawing bubble

void drawBubbles() {
   for (uint8_t i = 0; i < NUMBER_OF_BUBBLES; i++) {    
      if (bubbles[i].shooting == true) {
        uint8_t imageIndex = static_cast<uint8_t>(bubbles[i].b_type);
        bubbles[i].image = bubble_images[imageIndex];
        Sprites::drawOverwrite(bubbles[i].x, bubbles[i].y - getImageHeight(bubbles[i].image), bubbles[i].image, 0);      
                                        }
      if(capturing(i)){
       uint8_t imageIndex = static_cast<uint8_t>(bubbles[i].b_type);
        bubbles[i].image = bubble_images[imageIndex];
        Sprites::drawOverwrite(bubbles[i].x, bubbles[i].y - getImageHeight(bubbles[i].image), bubbles[i].image, 0);      
                                        }                                   
      }
    }
  

 //////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /* -----------------------------------------------------------------------------------------------------------------------------
 *  Update the position of any visible obstacles ..
 *
 *  If the obstacle has completely off screen, then disable it.  Pterodactyls move 1 pixel per update whereas cacti move one
 *  pixel every second iteration.
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void updateObstacles() {
  for (uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {   
    if (obstacles[i].enabled == true) {     
      switch (obstacles[i].type) {
        case ObstacleType::Benzo1:
        case ObstacleType::Benzo2:
         if (arduboy.everyXFrames(6)) {
            if (obstacles[i].type == ObstacleType::Benzo1) { 
                obstacles[i].type = ObstacleType::Benzo2;
            }
            else {
              obstacles[i].type = ObstacleType::Benzo1;
            }
          }
////////////////////////////////////////////////////
        //--obstacleDelayCountdown;
        if(obstacles[i].x < bob.x && !obstacles[i].falling && !obstacles[i].jumping){ if(bob.x%2==1){obstacles[i].x++;}}
        if(obstacles[i].x > bob.x && !obstacles[i].falling && !obstacles[i].jumping){ if(bob.x%2==1){obstacles[i].x--;}}
        //obstacleDelayCountdown=random(OBSTACLE_LAUNCH_DELAY_MIN, OBSTACLE_LAUNCH_DELAY_MAX);
        if(!isPointInTile(obstacles[i].x, obstacles[i].y)&&!isPointInTile(obstacles[i].x+8, obstacles[i].y)&& !obstacles[i].jumping){
          obstacles[i].falling=true;                
          obstacles[i].y = obstacles[i].y+fallCoords[obstacles[i].fallIndex];
          obstacles[i].fallIndex++;
          if(obstacles[i].fallIndex == sizeof(fallCoords)) {
             if(isPointInTile(obstacles[i].x, obstacles[i].y)) {
                   obstacles[i].falling = false;
                   obstacles[i].fallIndex = 0;
                  }
              else
                    obstacles[i].fallIndex = 0;
              }
         }
/////////////////////////////////////////////////////////////
 // bob 이 적 위에 있으면

        if(arduboy.pressed(LEFT_BUTTON)&& obstacles[i].y > bob.y && !obstacles[i].falling ) { 
          obstacles[i].jumping = true;
          obstacles[i].y = obstacles[i].y-jumpCoords[obstacles[i].jumpIndex];
          obstacles[i].jumpIndex++;
           if(obstacles[i].jumpIndex == sizeof(jumpCoords)) {
             if(isPointInTile(obstacles[i].x, obstacles[i].y)) {
                obstacles[i].jumping=false;
                obstacles[i].jumpIndex = 0;
                }
                else
                obstacles[i].jumpIndex =0;
             }
             
        }
   
      
////////////////////////////////////////////////////////////

          break;



        case ObstacleType::Bonnie1:
        case ObstacleType::Bonnie2:
       
          if (arduboy.everyXFrames(6)) {
            if (obstacles[i].type == ObstacleType::Bonnie1) { 
              obstacles[i].type = ObstacleType::Bonnie2;
            }
            else {
              obstacles[i].type = ObstacleType::Bonnie1;

            }

          }

          obstacles[i].x--;

          break;

        case ObstacleType::Boa1:
        case ObstacleType::Boa2:       
          if (arduboy.everyXFrames(6)) {
            if (obstacles[i].type == ObstacleType::Boa1) { 
              obstacles[i].type = ObstacleType::Boa2;
            }
            else {
              obstacles[i].type = ObstacleType::Boa1;
            }

          }



          obstacles[i].x--;

          break;

        case ObstacleType::Blubba1:
        case ObstacleType::Blubba2:        
          if (arduboy.everyXFrames(6)) {

            if (obstacles[i].type == ObstacleType::Blubba1) { 

              obstacles[i].type = ObstacleType::Blubba2;

            }

            else {

              obstacles[i].type = ObstacleType::Blubba1;

            }

          }

          obstacles[i].x--;

          break;
      
      }

      



      // Has the obstacle moved out of view ?
      if (obstacles[i].x < -getImageWidth(obstacles[i].image)) {
        obstacles[i].enabled = false; 

      }


    }

    

  }

  

}





/* -----------------------------------------------------------------------------------------------------------------------------

 *  Render any visible obstacles on the screen ..

 * -----------------------------------------------------------------------------------------------------------------------------

 */

void drawObstacles() {



  for (uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {

    

    if (obstacles[i].enabled == true) {



      uint8_t imageIndex = static_cast<uint8_t>(obstacles[i].type);

      obstacles[i].image = obstacle_images[imageIndex];

      Sprites::drawOverwrite(obstacles[i].x, obstacles[i].y - getImageHeight(obstacles[i].image), obstacles[i].image, 0);      



    }

    

  }

  

}



//////////

/* -----------------------------------------------------------------------------------------------------------------------------

 *  Launch a new obstacle ..

 * -----------------------------------------------------------------------------------------------------------------------------

 */

void launchObstacle(uint8_t obstacleNumber) {


  // Randomly pick an obstacle ..  
  ObstacleType randomUpper = ObstacleType::Benzo1;
  switch (point) {
    case 0 ... 99: 
      randomUpper = ObstacleType::Benzo1;
      break;
    case 100 ... 199: 
      randomUpper = ObstacleType::Bonnie1;
      break;
    case 200 ... 299: 
      randomUpper = ObstacleType::Boa1;
      break;
    default:
      randomUpper = ObstacleType::Count_AllObstacles;
      break;
  }

  uint8_t randomLowerVal = static_cast<uint8_t>(ObstacleType::Benzo1);
  uint8_t randomUpperVal = static_cast<uint8_t>(randomUpper);
  uint8_t raddomObstacle = random(randomLowerVal, randomUpperVal + 1);

  ObstacleType type = static_cast<ObstacleType>(raddomObstacle);

  // Launch the obstacle ..

  obstacles[obstacleNumber].type = type;
  obstacles[obstacleNumber].enabled = true;
  obstacles[obstacleNumber].x = WIDTH - 1;
  if(type == ObstacleType::Benzo1) {
     obstacles[obstacleNumber].y = 12;
     }
  if(type == ObstacleType::Bonnie1) {
     obstacles[obstacleNumber].y = 28;
    }
  if(type == ObstacleType::Boa1){
     obstacles[obstacleNumber].y =44;
   }
  if(type == ObstacleType::Blubba1){
    obstacles[obstacleNumber].y = 60;
  }  
}
