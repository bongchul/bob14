#pragma once
#define NUMBER_OF_BUBBLES 7
uint8_t count_bubble = 0;

enum class BubbleType : uint8_t {
  R_Shoot,
  L_Shoot,
  Captured,
  Up_Bubble,
  Pop_Bubble,
};

//////////////////////////
struct Bubble{
  int8_t x;
  int8_t y;
  BubbleType b_type;
  bool shooting;  
  const uint8_t *image;
};

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
