// a version of Shotton textonboost C#

#ifndef TEXTON_BOOST_BOOL_ARRAY_H
#define TEXTON_BOOST_BOOL_ARRAY_H
#include <sys/types.h>
namespace TextonBoostNS {
  /**
   * @brief Array of boolean value
   */
  class BoolArray {
  private:
    unsigned int* data;
    int nBits;
    int bpi;
  public:
    BoolArray(){}
    //static const int bpi;// = 8 * sizeof(uint);
    /**
     * @brief Constructor
     */
    BoolArray(int nBits) {
      bpi = 8 * sizeof(uint);
      this->nBits = nBits;
      data = new uint[nBits / bpi + 1];
    }
    /**
     * @brief get value at index
     */
    bool get(int index) {
      //if (index < 0 || index >= nBits)
      //    throw new IndexOutOfRangeException();
      return (data[index / bpi] & (1u << (index % bpi))) != 0;
    };
    /**
     * @brief set value at index
     */
    void set(int index, bool value) {
      //if (index < 0 || index >= nBits)
      //    throw new IndexOutOfRangeException();

      if (value)
        data[index / bpi] |= (1u << (index % bpi));
      else
        data[index / bpi] &= ~(1u << (index % bpi));
    }

  };


};
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
