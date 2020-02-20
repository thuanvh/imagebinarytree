// a version of Shotton textonboost C#

#ifndef TEXTONBOOSTCIRCLEBOUNDSENUMERATOR
#define TEXTONBOOSTCIRCLEBOUNDSENUMERATOR
#include <math.h>
class CircleBounds;
class CircleBoundsEnumerator { //: IEnumerator<Point>


private:
  CircleBounds* bounds;
  bool reset;
  Point p ;
  
public:
  ~CircleBoundsEnumerator() { }
  CircleBoundsEnumerator(CircleBounds* b) {
    bounds = b;
    //p= new Point();
    Reset();
  }
  
  
  
  void Reset() {
    reset = true;
  }
  
  
  Point Current() {
    return p;
  }
  
  bool FindInternalPoint() {
    while (true) {
      if (bounds->Contains(p))
        return true;
      else
        p.X++;
      if (p.X > (int) ceil(bounds->cX + bounds->radius)) {
        p.Y++;
        p.X = (int)(bounds->cX - bounds->radius);
      }
      if (p.Y > (int) ceil(bounds->cY + bounds->radius))
        return false;
    }
  }
  
  bool MoveNext() {
    if (!bounds->nonEmpty)
      return false;
    else
      if (reset) {
        reset = false;
        p.X = (int)(bounds->cX - bounds->radius);
        p.Y = (int)(bounds->cY - bounds->radius);
        return FindInternalPoint();
      } else {
        p.X++;
        return FindInternalPoint();
      }
  }
  
  
  
};
#endif