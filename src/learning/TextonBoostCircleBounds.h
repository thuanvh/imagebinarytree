// a version of Shotton textonboost C#

#ifndef TEXTONBOOST_CIRCLE_BOUNDS_H
#define TEXTONBOOST_CIRCLE_BOUNDS_H

namespace TextonBoostNS {
  class PointF;
  /**
   * @brief Bound of circle
   */
  class CircleBounds {
  private:
    bool nonEmpty;
    float cX, cY, radius;

  public:
    PointF GetCentre() {
      return PointF(cX, cY);
    }

    float CentreX() {
      return cX;
    }

    float CentreY() {
      return cY;
    }

    float Radius() {
      return radius;
    }

    bool CheckNonEmpty() {
      return radius >= 0;
    }

    CircleBounds(float cX, float cY, float radius) {
      this->cX = cX;
      this->cY = cY;
      this->radius = radius;

      this->nonEmpty = true;
      this->nonEmpty = CheckNonEmpty();
    }

    bool Contains(Point p) {
      if (nonEmpty)
        return (p.Y - cY) * (p.Y - cY) + (p.X - cX) * (p.X - cX) <= radius * radius;
      else
        return false;
    }

    bool Contains(PointF pF) {
      if (nonEmpty)
        return (pF.Y - cY) * (pF.Y - cY) + (pF.X - cX) * (pF.X - cX) <= radius * radius;
      else
        return false;
    }

    string ToString() {
      stringstream strs;
      if (nonEmpty)
        strs << "[ (" << cX << ", " << cY << "), radius " << radius << "]";
      else
        strs << "[ Empty CircleBounds (negative radius) ]";
      return strs.str();
    }

    CircleBoundsEnumerator* GetEnumerator() {
      return new CircleBoundsEnumerator(this);
    }
  };
}
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
