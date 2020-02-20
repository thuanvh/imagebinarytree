#ifndef Histogram_h
#define Histogram_h

#include "Image.h"
#include <map>
#include <iostream>

using namespace std;

namespace LibTIM {

  /** \defgroup Histogram
   \ingroup DataStructures
  **/

  /*@{*/

///Container for histograms
  /**
   Structure describing an histogram.
   Histogram can be constructed from an Image
  **/

//template <class T, class float>

  class Histogram {
    //typedef std::map <T, T2, std::less<T> > HistoType;

  public:
    ///Constructs an histogram from image im
    //Histogram(Image <T> &im);
    int channelNumber;
    int size[3];
    Histogram(Image <U8C3> const & img, int const* max, int const * size, int* channelIndex, int channelNumber, U8C3* background);
    /**
     * normalize the histogram
     */
    void normalize();
    ///Write histogram to disk
    /**
     *    Histogram is writed in a text file (xmgrace format)
    **/
    //void write(const char *filename);
    void write(ostream& file);
    void write(ostream& file, int & colid);
    void write(vector<double>& valueList);
    /**
     * get data reference
     **/
    /*float& operator()(int const & z) {
      return (*this)(0, 0, z);
    }*/

    const float operator()(int const & z) const {
      return (*this)(0, 0, z);
    }


    /*float& operator()(int const & x, int const & y, int const & z) {
      cout<<"get1 "<<x<<","<<y<<","<<z<<endl;
      if (data->find(x) == data->end())
        (*data)[x] = new  std::map < int, std::map<int, float, std::less<int> >* >();

      //return 0;
      else
        if ((*data)[x]->find(y) == (*data)[x]->end())
          (*(*data)[x])[y] = new  std::map<int, float, std::less<int> >();

      //return 0;
        else
          if ((*(*data)[x])[y]->find(z) == (*(*data)[x])[y]->end())
            (*(*(*data)[x])[y])[z] = 0;

      //return 0;
      return (*(*(*data)[x])[y])[z];
    };*/

    const float operator()(int const & x, int const & y, int const & z) const {
      //cout<<"get2 "<<x<<","<<y<<","<<z<<endl;
      if (data->find(x) == data->end())
        return 0;
      else
        if ((*data)[x]->find(y) == (*data)[x]->end())
          return 0;
        else
          if ((*(*data)[x])[y]->find(z) == (*(*data)[x])[y]->end())
            return 0;

      return (*(*(*data)[x])[y])[z];
    }

    /*float& operator()(int const & x, int const & y) {
      return (*this)(0, x, y);
    }*/

    const float operator()(int const & x, int const & y) const {
      return (*this)(0, x, y);
    }

    ~Histogram();
    //HistoType& getData();

  private:
    //HistoType data;
    std::map< int, std::map < int, std::map<int, float, std::less<int> >* >* >* data;
  };

  /*@}*/

}


#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
