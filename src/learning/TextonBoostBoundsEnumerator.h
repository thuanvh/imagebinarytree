// a version of Shotton textonboost C#

#ifndef TEXTONBOOST_BOUNDS_ENUMERATOR_H
#define TEXTONBOOST_BOUNDS_ENUMERATOR_H
namespace TextonBoostNS {
  /**
   * @brief Enumerator of list of points
   */
  class BoundsEnumerator { //: IEnumerator<Point>
  public:
    void Dispose() { };

    Bounds* bounds;
    bool reset;
    Point p;

    BoundsEnumerator(Bounds* b) {
      bounds = b;
      reset = true;
      Reset();
    }



    void Reset() {
      reset = true;

    }


    Point Current() {
      return p;
    }
    /**
     * @brief Move to the next point. Travel by row by row in the bounds.
     * @return Is able to move 
     */
    bool MoveNext() {
      if (!bounds->nonEmpty)
        return false;
      else
        if (reset) {
          reset = false;
          p.X = bounds->MinX();
          p.Y = bounds->MinY();
          return true;
        } else {

          p.X++;
          if (p.X > bounds->MaxX()) {
            p.X = bounds->MinX();
            p.Y++;
          }
          return p.Y <= bounds->MaxY();
        }
    }


  };
}
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
