#include <Arduboy2.h>
#include <ArduboyTones.h>
//////////////
#include <stdint.h>
#include "Images.h"
#include "map.h"
#include "sound.h"
#include "bubble.h"
#include "obstacles.h"
#define BOB_GROUND_LEVEL  60
//////////////////////////////
uint16_t point = 10;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class GameStatus : uint8_t {
  Introduction,
  PlayGame,
  GameOver,
  EndGame,
  HappyEnd,
};
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

///////////////최종 boss, final boss is steve. He is big and angry.
enum class steveStance : uint8_t {
  Running1,
  Running2,
  Stunned1,
  Stunned2,
  Dead1,
};

struct Steve {
  uint8_t x;
  uint8_t y;
  steveStance stance;
  bool enabled;
  bool stunned;
  uint8_t stunIndex;
  const uint8_t *image;
  const uint8_t *mask;
};
//////////////////////////////////////////////////////////////최종 boss 끝

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Bob bob = {0, BOB_GROUND_LEVEL, Stance::R_Standing, false, false, false, false, bob_R_still, bob_R_still_mask };
uint8_t jumpCoords[] = { 3, 3, 2, 2, 2, 1, 1, 1, 1};
uint8_t fallCoords[] = { 3, 2, 1, 2, 2, 1, 2, 2, 1};
uint8_t stunCoords[] = {3,3,3};
GameStatus gameStatus = GameStatus::Introduction;

uint16_t obstacleLaunchCountdown = OBSTACLE_LAUNCH_DELAY_MIN;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t *bob_images[] = { bob_R_still,bob_R_running_1,bob_R_running_2,bob_R_bubble,bob_L_running_1,bob_L_running_2,bob_L_bubble,bob_dead };
const uint8_t *bob_masks[] = { bob_R_still_mask, bob_R_running_1_mask, bob_R_running_2_mask, bob_R_bubble_mask,  bob_L_running_1_mask, bob_L_running_2_mask, bob_L_bubble_mask, bob_dead_mask };
//////////////
const uint8_t *bubble_images[] = { bubble_R_shoot, bubble_L_shoot, bubble_captured,  bubble_pop, bubble_up };

//////////////////////////////////////////////steve 초기화/////////////////////////////////////////////////////////////////////////////////
Steve steve = {136 , BOB_GROUND_LEVEL, steveStance::Running1, false, false, false, steve_running1, steve_running1_mask };
const uint8_t *steve_images[] = { steve_running1, steve_running2, steve_stunned1,steve_stunned2, steve_dead1 };
const uint8_t *steve_masks[] = { steve_running1_mask, steve_running2_mask, steve_stunned1_mask,steve_stunned2_mask, steve_dead1_mask };
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* -----------------------------------------------------------------------------------------------------------------------------
 *  Setup the environment ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void setup() {
  arduboy.boot();
  arduboy.setFrameRate(75);
  arduboy.initRandomSeed();
  arduboy.audio.begin();
}
/* -----------------------------------------------------------------------------------------------------------------------------
 *  Control the various states of the game ..
 * -----------------------------------------------------------------------------------------------------------------------------
 */
void loop() {
  // Pause here until it's time for the next frame ..
  if (!(arduboy.nextFrame())) return;
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
    case GameStatus::EndGame: 
      endGame();
      break;
    case GameStatus::HappyEnd:
      happyEnd();
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
    obstacles[i].jumping = false;
    obstacles[i].falling = false;
    obstacles[i].jumpIndex = 0;
    obstacles[i].fallIndex = 0;
    obstacles[i].type = ObstacleType::Benzo1;
  }
//////////////////////////steve를 초기화 시킴//////////////////////////////////////////////////////////////////////
  steve.x = 136;
  steve.y = BOB_GROUND_LEVEL;
  steve.enabled = false;
  steve.stunned = false;
  steve.stunIndex = 0;
  steve.stance = steveStance::Running1;
/////////////////////////////////////////////////////////////////////////////
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
    point=10;
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
  arduboy.audio.on();
  switch(point){
    case 0 ... 99:
    arduboy.print(point);
    memcpy( world, world, sizeof(uint8_t)*32*4);
    drawlevel();
    break;
    case 100 ... 199:
    arduboy.print(point);
    memcpy( world, world2, sizeof(uint8_t)*32*4);
    drawlevel();
    break;
    case 200 ... 399:
    arduboy.print(point);
    memcpy( world, world3, sizeof(uint8_t)*32*4);
    drawlevel();
    break;
    case 400:
    arduboy.clear();
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
      gameStatus = GameStatus::EndGame;
      break;
  }
  //drawlevel();
 // The player can only control Bob if he is running  on the ground ..
  if (!bob.jumping&&!bob.falling) {
    if (arduboy.justPressed(UP_BUTTON)) { bob.jumping = true; bob.jumpIndex = 0; }
    if (arduboy.justPressed(B_BUTTON))  {
          if (bob.stance == Stance::R_Running1 || bob.stance == Stance::R_Running2) {
                 bob.stance = Stance::R_Bubble;                               
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
        updateObstacles();
        drawObstacles();
        break;
      }
      else
      if(obstacles[i].x==WIDTH-2){
      obstacles[i].type=ObstacleType::Bonnie1;
      }
      updateObstacles();
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
//////////////////////////////////적을만들자 Let's make obstacles
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
////////
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
 *  Render Bob
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
          bubbles[i].x=bubbles[i].x+2;
          break;
        case BubbleType::L_Shoot:
          bubbles[i].x=bubbles[i].x-2;
          break;
       case BubbleType::Captured:    
          bubbles[i].y--;
          break;
       case BubbleType::Up_Bubble:    
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

/////////////////// update bubbles's end
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
      if(steve_capturing(i)&&steve.enabled==true){       
        uint8_t imageIndex = static_cast<uint8_t>(bubbles[i].b_type);
        bubbles[i].image = bubble_images[imageIndex];
        Sprites::drawOverwrite(bubbles[i].x, bubbles[i].y - getImageHeight(bubbles[i].image), bubbles[i].image, 0);                                              
        steve.stunned=true;
        //steve_point=steve_point+100;                            
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
        if(obstacles[i].x < bob.x && !obstacles[i].falling && !obstacles[i].jumping ){  if(obstacles[i].x>1){ obstacles[i].x =obstacles[i].x+random(0,2);} else obstacles[i].x++;}
        if(obstacles[i].x > bob.x && !obstacles[i].falling && !obstacles[i].jumping ){  if(obstacles[i].x<129){obstacles[i].x =obstacles[i].x-random(0,2);} else obstacles[i].x--;}     
        if(!isPointInTile(obstacles[i].x, obstacles[i].y)&&!isPointInTile(obstacles[i].x+8, obstacles[i].y)&&!obstacles[i].jumping){
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
 // bob 이 적 위에 있으면 if bob is above of any obstacles
        if( bob.x-obstacles[i].x > 4 && obstacles[i].y > bob.y &&!obstacles[i].falling && obstacles[i].jumpIndex <= sizeof(jumpCoords)) {
          obstacles[i].jumping = true;
          obstacles[i].y = obstacles[i].y-jumpCoords[obstacles[i].jumpIndex];
          obstacles[i].jumpIndex++;
          delay(10);
           if(obstacles[i].jumpIndex == sizeof(jumpCoords)) {
             if(isPointInTile(obstacles[i].x, obstacles[i].y)) {
                obstacles[i].jumping=false;
                obstacles[i].jumpIndex = 0;
                }
                //else
                //obstacles[i].jumpIndex =0;
             if(!isPointInTile(obstacles[i].x,obstacles[i].y)){
                obstacles[i].falling=true;
                obstacles[i].y = obstacles[i].y+fallCoords[obstacles[i].fallIndex];
                obstacles[i].fallIndex++;
                 if(obstacles[i].fallIndex == sizeof(fallCoords)){
                  obstacles[i].falling=false;
                  obstacles[i].fallIndex =0;
                 }
             }
             //else
             //obstacles[i].jumpIndex =0;
             //obstacles[i].fallIndex =0;
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
          obstacles[i].x++;
          break;
      }
  
      // Has the obstacle moved out of view ?
      if (obstacles[i].x < -getImageWidth(obstacles[i].image)|| obstacles[i].x>129) {
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
  ObstacleType randomLower = ObstacleType::Benzo1;
  ObstacleType randomUpper = ObstacleType::Benzo2;
  
  switch (point) {
    case 0 ... 99:
      randomLower = ObstacleType::Benzo1;
      randomUpper = ObstacleType::Benzo2;
      break;
    case 100 ... 199:
      randomLower = ObstacleType::Bonnie1;
      randomUpper = ObstacleType::Bonnie2;
      break;
    case 200 ... 299:
      randomLower = ObstacleType::Bonnie1;
      randomUpper = ObstacleType::Boa2;
      break;
    default:
      randomLower = ObstacleType::Bonnie1;
      randomUpper = ObstacleType::Blubba2;
      break;
  }
  uint8_t randomLowerVal = static_cast<uint8_t>(randomLower);
  uint8_t randomUpperVal = static_cast<uint8_t>(randomUpper);
  uint8_t raddomObstacle = random(randomLowerVal, randomUpperVal);
  ObstacleType type = static_cast<ObstacleType>(raddomObstacle);
  // Launch the obstacle ..
  obstacles[obstacleNumber].type = type;
  obstacles[obstacleNumber].enabled = true;
  
  if(type == ObstacleType::Benzo1||type==ObstacleType::Benzo2) {
     switch(obstacleNumber%4){
      case 1 ... 3:
      obstacles[obstacleNumber].x = WIDTH-2;    
      obstacles[obstacleNumber].y = 12;
      break;
      case 0:
      obstacles[obstacleNumber].x = WIDTH-2;
      obstacles[obstacleNumber].y = 28;
      break;
     }
  }
  if(type == ObstacleType::Bonnie1||type==ObstacleType::Bonnie2) {
     switch(obstacleNumber%4){
      case 0:
      obstacles[obstacleNumber].y = 12;
      obstacles[obstacleNumber].x = WIDTH - 1;
      break;
      case 1:
      obstacles[obstacleNumber].y = 28;
      obstacles[obstacleNumber].x = WIDTH - 1;
      break;
      case 2:
      obstacles[obstacleNumber].y = 44;
      obstacles[obstacleNumber].x = WIDTH - 1;
      break;      
      case 3:
      obstacles[obstacleNumber].y = 60;
      obstacles[obstacleNumber].x = WIDTH - 1;
      break;   
     }
    }
  if(type == ObstacleType::Boa1 || type==ObstacleType::Boa2){
     obstacles[obstacleNumber].y =44;
     obstacles[obstacleNumber].x = WIDTH - 1;
   }
  if(type == ObstacleType::Blubba1||type==ObstacleType::Blubba2){
    switch(obstacleNumber%4){
      case 0:
      obstacles[obstacleNumber].x = 1;
      obstacles[obstacleNumber].y = 60;
      break;
      case 1 ... 3:
      obstacles[obstacleNumber].x = 1;
      obstacles[obstacleNumber].y = 28;
      break;
    }
  }  
}

////////////////////// End_game
/* -----------------------------------------------------------------------------------------------------------------------------
 *  boss와 싸우는 곳
 * -----------------------------------------------------------------------------------------------------------------------------
 */

void endGame() {
  arduboy.clear();
  arduboy.setCursor(0, 0);
  arduboy.print(point);
  arduboy.audio.on();
  memcpy( world, world1, sizeof(uint8_t)*32*4);
  switch(point){
    case 900 ... 999:
    steve.stance = steveStance::Stunned1;
    break;
    case 1000 ... 1100:
    steve.enabled=false;
    steve.x=120;
    steve.stance=steveStance::Dead1;
    gameStatus = GameStatus::HappyEnd;
    break;
  }
  drawlevel();
  //steve_point=600;
  steve.enabled=true;
  //if(steve.enabled)
    //      { drawSteve();}

 // The player can only control Bob if he is running  on the ground ..
  if (!bob.jumping&&!bob.falling) {
    if (arduboy.justPressed(UP_BUTTON)) { bob.jumping = true; bob.jumpIndex = 0; }
    if (arduboy.justPressed(B_BUTTON))  {
          if (bob.stance == Stance::R_Running1 || bob.stance == Stance::R_Running2) {
                 bob.stance = Stance::R_Bubble;                       
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
///////////////////////////////////////////////
  // Has Steve collided with anything?/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (collision_steve()) {
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
//////////////////////////////////적을만들자 Let's make obstacles
    updateSteve();
    drawSteve();
//////////////////////////////////////////////////////////////////////////////////////////////////
    arduboy.display();
  }
}

//////////////////////endGame 마지막
void happyEnd(){
  arduboy.clear();
  //drawBob();
  bob.x=0; bob.y=44;
  drawSteve();
  arduboy.setCursor(40, 12);
  arduboy.print(F("You win!!!"));
  arduboy.audio.off();
  //updateBob();
  updateSteve();
  arduboy.display();
  if (arduboy.justPressed(A_BUTTON)) {
    initialiseGame();
    point=10;
    gameStatus = GameStatus::PlayGame;
    bob.stance = Stance::R_Running1;
  }
}



///////////////////////////////////////////

void drawSteve(){
  uint8_t imageIndex = static_cast<uint8_t>(steve.stance);
  steve.image = steve_images[imageIndex];
  steve.mask = steve_masks[imageIndex];
  Sprites::drawExternalMask(steve.x, steve.y - getImageHeight(steve.image), steve.image, steve.mask, 0, 0); 
}



/////////////////////////////

void updateSteve() {

  

  if( point <900 && bob.y >15 && steve.x>4  )
  { steve.x=steve.x-random(0,2); 
  }
  else {
    // Swap the image to simulate running ..
    if (arduboy.everyXFrames(3)) {
      switch (steve.stance) {  
        case steveStance::Running1:
          steve.stance = steveStance::Running2;
          break;      
        case steveStance::Running2:
          steve.stance = steveStance::Running1;
          break;         
        default:
          break;      
      }
    }///////////////////////////////////
  }
  if( point<900 && bob.y < 15 && steve.x<124 )
  { steve.x++;}
  else {
    // Swap the image to simulate running ..
    if (arduboy.everyXFrames(3)) {
      switch (steve.stance) {  
        case steveStance::Running1:
          steve.stance = steveStance::Running2;
          break;      
        case steveStance::Running2:
          steve.stance = steveStance::Running1;
          break;
        default:
          break;      
      }
    }///////////////////////////////////
  }

 if( point > 900 && bob.y >15 && steve.x>4  )
  { steve.x=steve.x-random(0,2); 
  }
  else {
    // Swap the image to simulate running ..
    if (arduboy.everyXFrames(3)) {
      switch (steve.stance) {  
        case steveStance::Stunned1:
          steve.stance = steveStance::Stunned2;
          break;      
        case steveStance::Stunned2:
          steve.stance = steveStance::Stunned1;
          break;                 
        default:
          break;      
      }
    }///////////////////////////////////
  }
  if( point>900 && bob.y < 15 && steve.x<124 )
  { steve.x++;}
  else {
    // Swap the image to simulate running ..
    if (arduboy.everyXFrames(3)) {
      switch (steve.stance) {  
       case steveStance::Stunned1:
          steve.stance = steveStance::Stunned2;
          break;      
        case steveStance::Stunned2:
          steve.stance = steveStance::Stunned1;
          break;       
        default:
          break;      
      }
    }///////////////////////////////////
  }

}
