#ifndef Types_h
#define Types_h

namespace LibTIM {
//Machine dependant typedefs

  typedef unsigned char U8;
  typedef signed char S8;

  typedef unsigned short U16;
  typedef signed short S16;

  typedef unsigned long  U32;
  typedef signed long    S32;

  typedef float F32;
//typedef double F64;

//Table
  template <class T, int N>

  struct Table {
    Table() {};

    Table(const Table &v) {
      for (int i = 0; i < N; i++)
        el[i] = v.el[i];
    }

    Table(int p) {
      for (int i = 0; i < N; i++)
        el[i] = p;
    }

    Table(int *vect) {
      for (int i = 0; i < N; i++)
        el[i] = vect[i];
    }

    T el[N];
    T &operator[](int i) {
      return el[i];
    }

    T operator=(T const & val) {
      for (int i = 0; i < N; i++)
        el[i] = val;

      return val;
    }

    Table<T, N>& operator+=(Table<T, N> const & val) {
      for (int i = 0; i < N; i++)
        el[i] += val.el[i];

      return *this;
    }
    bool operator==(Table<T, N> const & val) {
      for (int i = 0; i < N; i++)
        if(el[i]!=val.el[i])
          return false;
      
      return true;
    }
    bool operator==( int const & val) {
      for (int i = 0; i < N; i++)
        if(el[i]!=val)
          return false;
        
        return true;
    }
    bool operator!=( int const & val) {
      for (int i = 0; i < N; i++)
        if(el[i]==val)
          return false;
        
        return true;
    }
  };

//Type of RGB point
//typedef Table<U8, 3> RGB;
  typedef Table<U8, 3> U8C3;
  typedef unsigned char U8C1;

//Type of image size
  typedef unsigned short TSize;

//Type of point spacing
  typedef double TSpacing;

//Type of points coordinates
  typedef int TCoord;

//Type of label
  typedef unsigned long TLabel;

//Type of offset
  typedef  long TOffset;

  const float FLOAT_EPSILON = 0.0000000001f;
}

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
