/////////////capturing bubble// caputuring.ino



bool capturing(uint8_t j){  

       for(uint8_t i=0; i < NUMBER_OF_OBSTACLES; i++){

       if(obstacles[i].enabled ==true) {

         Rect bubRect = Rect{

                bubbles[j].x,

                bubbles[j].y-getImageHeight(bubbles[j].image),

                getImageWidth(bubbles[j].image),

                getImageHeight(bubbles[j].image)

                };



        Rect obsRect = Rect{

                obstacles[i].x,

                obstacles[i].y- getImageHeight(obstacles[i].image),

                getImageWidth(obstacles[i].image),

                getImageHeight(obstacles[i].image)

               };



        if(arduboy.collide(bubRect, obsRect)){

            obstacles[i].enabled=false;
            bubbles[j].b_type = BubbleType::Captured;

            point=point+10;

            return true;

            }

      }

  }

  return false;

}
