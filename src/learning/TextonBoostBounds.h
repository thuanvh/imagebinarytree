// a version of Shotton textonboost C#

#ifndef TEXTON_BOOST_BOUNDS_H
#define TEXTON_BOOST_BOUNDS_H

#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <iostream>
#include "TextonBoost.h"
using namespace std;

namespace TextonBoostNS {
  /**
   * @brief Size
   */
  class Size {
  public:
    Size() {}
    Size(int w, int h) {
      Width = w;
      Height = h;
    };
    int Width;
    int Height;
  };
  /**
   * @brief Point
   */
  class Point {
  public:
    Point(){X=Y=0;}
    Point(int x, int y) {
      X = x;
      Y = y;
    }
    int X;
    int Y;
  };
  /**
   * @brief PointF
   */
  class PointF {
  public:
    PointF(){}
    PointF(float x, float y) {
      X = x;
      Y = y;
    }
    float X;
    float Y;
  };
  
  class BoundsEnumerator;
  /**
   * @brief Bounds is a rectangle. This is a set of points.
   */
  class Bounds { // : IEnumerable<Point>
  private:
    
    int minX, minY, maxX, maxY;
    
  public:
    bool nonEmpty;
    /**
     * @brief Get centre point of bounds
     */
    PointF GetCentre() {
      return PointF((maxX + minX) / 2.f, (maxY + minY) / 2.f);
    }
    /**
     * @brief Get 4 corners of the bounds
     */
    Point* GetCorners() {
      Point *corners = new Point[4];
      corners[0] = Point(minX, minY);
      corners[1] = Point(minX, maxY);
      corners[2] = Point(maxX, maxY);
      corners[3] = Point(maxX, minY);
      //return Point[4] = { Point(minX, minY), Point(minX, maxY), Point(maxX, maxY), Point(maxX, minY) };
    }
    /**
     * @brief Get rectangle which is the intersection with another bounds
     * @param with The intersecting rectangle
     */
    Bounds& IntersectWith(Bounds& with) {
      Bounds* ret = new Bounds();
      if (!this->nonEmpty || !with.nonEmpty)
        ret->nonEmpty = false;
      else {
        ret->minX = max(this->MinX(), with.MinX());
        ret->minY = max(this->MinY(), with.MinY());
        ret->maxX = min(this->MaxX(), with.MaxX());
        ret->maxY = min(this->MaxY(), with.MaxY());
        if (ret->MaxX() < ret->MinX() || ret->MaxY() < ret->MinY()) {
          ret->nonEmpty = false;
        } else
          ret->nonEmpty = true;
      }
      return *ret;
    }
    /**
     * @brief Point minimal
     */
    Point MinPoint() {
      return Point(MinX(), MinY());
    }
    /**
     * @brief Point maximal
     */
    Point MaxPoint() {
      return Point(MaxX(), MaxY());
    }
    /**
     * @brief MinX
     */
    int MinX() {
      return minX;
    }
    
    int MinY() {
      return minY;
    }
    
    int MaxX() {
      return maxX;
    }
    
    int MaxY() {
      return maxY;
    }
    
    int Width() {
      return max(0, MaxX() - MinX() + 1);
    }
    
    int Height() {
      return max(0, MaxY() - MinY() + 1);
    }
    
    Size size() {
      return Size(Width(), Height());
    }
    /**
     * @brief Check is not empty by checking the value of width and height
     */
    bool CheckNonEmpty() {
      return Width() > 0 && Height() > 0;
    }
    /**
     * @brief Shrink the bound to the new size
     */
    Bounds ShrinkBy(int size) {
      return Bounds(minX + size, minY + size, maxX - size, maxY - size);
    }
    /**
     * @brief Get a bound at a position relative with a point
     */
    Bounds OffsetBy(Point p) {
      return Bounds(minX + p.X, minY + p.Y, maxX + p.X, maxY + p.Y);
    }
    /**
     * @brief Get a bound at a position relative with a point float
     */
    Bounds OffsetBy(PointF p) {
      return Bounds(minX + (int) p.X, minY + (int) p.Y, maxX + (int) ceil(p.X), maxY + (int) ceil(p.Y));
    }
    /// Constructor
    Bounds(){}
    // INCLUSIVE COORDS
    Bounds(int minX, int minY, int maxX, int maxY) {
      this->minX = minX;
      this->minY = minY;
      this->maxX = maxX;
      this->maxY = maxY;
      this->nonEmpty = true;
      this->nonEmpty = CheckNonEmpty();
    }
    
    Bounds(Size s) {
      this->minX = 0;
      this->minY = 0;
      this->maxX = s.Width - 1;
      this->maxY = s.Height - 1;
      this->nonEmpty = true;
      this->nonEmpty = CheckNonEmpty();
    }
    
    Bounds(int width, int height) {
      this->minX = 0;
      this->minY = 0;
      this->maxX = width - 1;
      this->maxY = height - 1;
      this->nonEmpty = true;
      this->nonEmpty = CheckNonEmpty();
    }
    /*
    Bounds(Rectangle& r) {
      this->minX = r.X;
      this->minY = r.Y;
      this->maxX = r.X + r.Width - 1;
      this->maxY = r.Y + r.Height - 1;
      this->nonEmpty = true;
      this->nonEmpty = CheckNonEmpty();
    }*/
    /**
     * @brief Check if it contains a point
     */
    bool Contains(Point p) {
      if (nonEmpty)
        return minX <= p.X && p.X <= maxX && minY <= p.Y && p.Y <= maxY;
      else
        return false;
    }
    /**
     * @brief Check if it contains a pointF
     */
    bool Contains(PointF pF) {
      if (nonEmpty)
        return minX <= pF.X && pF.X <= maxX && minY <= pF.Y && pF.Y <= maxY;
      else
        return false;
    }
    /**
     * @brief Enlarge to contain a point
     */
    Bounds EnlargeInclude(Point p) {
      return EnlargeInclude(p.X, p.Y, p.X, p.Y);
    }
    /**
     * @brief Enlarge to include a point
     */
    Bounds EnlargeInclude(PointF p) {
      return EnlargeInclude((int) floor(p.X), (int) floor(p.Y), (int) ceil(p.X), (int) ceil(p.Y));
    }
    /**
     * @brief union of 2 bounds to a create a new bound which contains 2 bounds.
     */
    Bounds EnlargeInclude(int lX, int lY, int hX, int hY) {
      if (!nonEmpty)
        return  Bounds(lX, lY, hX, hY);
      else
        return Bounds(min(minX, lX), min(minY, lY), max(maxX, hX), max(maxY, hY));
    }
    /**
     * @brief Get string of class
     */
    string ToString() {
      stringstream strs;
      if (nonEmpty)
        strs<<"[ (" << minX << ", " << minY << "), ("<< maxX << ", "<< maxY << ")]";
      else
        strs<< "[ Empty Bounds ]";
      return strs.str();
    }
    /*
    BoundsEnumerator* GetEnumerator() {
      return new BoundsEnumerator(this);
    }*/
  };
  /**
   * @brief Class of boundsF
   */
  class BoundsF {
  private:
    bool isAssigned;
    float minX, minY, maxX, maxY;

  public:
    PointF GetCentre() {
      return PointF((maxX + minX) / 2.f, (maxY + minY) / 2.f);
    }

    float MinX() {
      return minX;
    }

    float MinY() {
      return minY;
    }

    float MaxX() {
      return maxX;
    }

    float MaxY() {
      return maxY;
    }

    float Width() {
      return maxX - minX + 1;
    }

    float Height() {
      return maxY - minY + 1;
    }

    // INCLUSIVE COORDS
    BoundsF(float minX, float minY, float maxX, float maxY) {
      this->minX = minX;
      this->minY = minY;
      this->maxX = maxX;
      this->maxY = maxY;
      this->isAssigned = true;
    }

    bool Contains(PointF p) {
      return minX <= p.X && p.X <= maxX && minY <= p.Y && p.Y <= maxY;
    }

    BoundsF EnlargeInclude(PointF p) {
      if (!isAssigned)
        return BoundsF(p.X, p.Y, p.X, p.Y);
      else
        return BoundsF(min(minX, p.X), min(minY, p.Y), max(maxX, p.X), max(maxY, p.Y));
    }

    string ToString() {
      stringstream strv;
      strv << "[ (" << minX << ", " << minY << "), (" << maxX << ", " << maxY << ")]";
      return strv.str();
    }
  };
}
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
