///////////////////////////////////
bool collision(){
  for(uint8_t i =0; i<NUMBER_OF_OBSTACLES; i++){
    if(obstacles[i].enabled == true){
      Rect bobRect = Rect{ 
        bob.x,
        bob.y-getImageHeight(bob.image),
        getImageWidth(bob.image),
        getImageHeight(bob.image)
      };
      Rect obsRect = Rect{
        obstacles[i].x,
        obstacles[i].y-getImageHeight(obstacles[i].image),
        getImageWidth(obstacles[i].image),
        getImageHeight(obstacles[i].image)
      };
      if(arduboy.collide(bobRect, obsRect)){
        return true;
      }
                              
    }
  }
  return false;
}
