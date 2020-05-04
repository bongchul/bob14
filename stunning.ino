////stunning.ino

bool steve_capturing(uint8_t j){  
     if(steve.enabled){
         Rect bubRect = Rect{
                bubbles[j].x,
                bubbles[j].y-getImageHeight(bubbles[j].image),
                getImageWidth(bubbles[j].image),
                getImageHeight(bubbles[j].image)
                };

        Rect steveRect = Rect{
               steve.x,
               steve.y- getImageHeight(steve.image),
               getImageWidth(steve.image),
               getImageHeight(steve.image)
              };
        if(arduboy.collide(bubRect, steveRect)){
            bubbles[j].b_type = BubbleType::Up_Bubble;       
            //point=point+10;
            point++;
            return true;
            }
     }
  return false;
}
