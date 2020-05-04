///////////////////////////////////
bool collision_steve(){
      Rect bobRect = Rect{ 
        bob.x,
        bob.y-getImageHeight(bob.image),
        getImageWidth(bob.image),
        getImageHeight(bob.image)
      };
      Rect steveRect = Rect{
        steve.x,
        steve.y-getImageHeight(steve.image),
        getImageWidth(steve.image),
        getImageHeight(steve.image)
      };
      if(arduboy.collide(bobRect, steveRect)){
        return true;
      }
                              
  return false;
}
