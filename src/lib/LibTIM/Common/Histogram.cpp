#include <stdlib.h>
#include <fstream>
#include <map>
#include "Histogram.h"

using namespace std;

namespace LibTIM {

  /*
  template <class T,class T2>
  Histogram <T,T2> ::Histogram(Image <T> &im)
  {
    int dx=im.getSizeX();
    int dy=im.getSizeY();
    int dz=im.getSizeZ();
    int size=dx*dy*dz;
    for(int i=0; i< size; i++)
      data[im(i)]++;
  }
  */
//template <class T,class T2>
  Histogram::Histogram(Image <U8C3> const & img, int const* max, int const * size, int* channelIndex, int channelNumber, U8C3* background) {
    //cout<<"constructor"<<endl;
    int sx = img.getSizeX();
    int sy = img.getSizeY();
    //int sz=img.getSizeZ();
    //int size=dx*dy*dz;
    memcpy(this->size, size, 3);
    data = new map< int, map < int, map<int, float, less<int> >* >* >();

    switch (channelNumber) {

      case 1:
        (*data)[0] = new map < int, map<int, float, less<int> >* >();
        (*(*data)[0])[0] = new map<int, float, less<int> > ();
        break;

      case 2:
        (*data)[0] = new map < int, map<int, float, less<int> >* >();
        break;

      case 3:
        break;
    }

    for (int x = 0; x < sx; x++) {
      for (int y = 0; y < sy; y++) {
        U8C3 unit = img(x, y);
        if(background!=NULL)
          if(unit==*background)
            continue;
        int sortedunit[3] = {0};
        // reduire the number of size

        for (int c = 0; c < channelNumber; c++) {
          int val = unit[channelIndex[c]];
          sortedunit[c] = val * size[c] / max[c];
        }

        switch (channelNumber) {

          case 1:
            (*(*(*data)[0])[0])[sortedunit[0]]++;
            break;

          case 2:

            if ((*data)[0]->find(sortedunit[0]) == (*data)[0]->end()) {
              (*(*data)[0])[sortedunit[0]] = new map<int, float, less<int> >();
            }

            if ((*(*data)[0])[sortedunit[0]]->find(sortedunit[1]) == (*(*data)[0])[sortedunit[0]]->end()) {
              (*(*(*data)[0])[sortedunit[0]])[sortedunit[1]] = 0;
            }

            (*(*(*data)[0])[sortedunit[0]])[sortedunit[1]]++;
            break;

          case 3:

            if (data->find(sortedunit[0]) == data->end()) {
              (*data)[sortedunit[0]] = new map < int, map<int, float, less<int> >* >();
            }

            if ((*data)[sortedunit[0]]->find(sortedunit[1]) == (*data)[sortedunit[0]]->end()) {
              (*(*data)[sortedunit[0]])[sortedunit[1]] = new map<int, float, less<int> >();
            }

            if ((*(*data)[sortedunit[0]])[sortedunit[1]]->find(sortedunit[2]) == (*(*data)[sortedunit[0]])[sortedunit[1]]->end()) {
              (*(*(*data)[sortedunit[0]])[sortedunit[1]])[sortedunit[2]] = 0;
            }

            (*(*(*data)[sortedunit[0]])[sortedunit[1]])[sortedunit[2]]++;
            break;
        }
      }
    }
  }
  Histogram::~Histogram()
  {
    //cout<<"delete histo"<<endl;
    map< int, map < int, map<int, float, less<int> >* >* >::iterator it1;
    map < int, map<int, float, less<int> >* >::iterator it2;
    map<int, float, less<int> >::iterator it3;
    for(it1=data->begin(); it1!=data->end(); it1++){
      for(it2=(it1->second->begin()); it2!=(it1->second->end()); it2++){
        for(it3=(it2->second->begin()); it3!=(it2->second->end()); it3++){
          //delete it3->
        }
        delete it2->second;
      }
      delete it1->second;
    }
    delete data;
    //cout<<"end delete"<<endl;
  }


///Write histogram into file
//template <class T,class T2>
  void Histogram::write(ostream& file) {
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[0]; j++) {
        for (int k = 0; k < size[0]; k++) {
          file << (*this)(i, j, k) << " ";
        }
      }
    }
  }
  void Histogram::write(std::vector< double >& valueList)
  {
    //cout<<"write to all"<<endl;
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[0]; j++) {
        for (int k = 0; k < size[0]; k++) {
          valueList.push_back((*this)(i, j, k));
        }
      }
    }
    //cout<<"end write"<<endl;
  }

  void Histogram::write(ostream& file, int & colid) {
    for (int i = 0; i < size[0]; i++) {
      for (int j = 0; j < size[0]; j++) {
        for (int k = 0; k < size[0]; k++) {
          float a = (*this)(i, j, k);

          if (a != 0) {
            file << colid++ << ":" << (*this)(i, j, k) << " ";
          } else {
            colid++;
          }
        }
      }
    }
  }

  /*
  ///Write histogram into file
  template <class T,class T2>
  void Histogram <T,T2>::write(const char *filename)
  {
    ofstream outputFile(filename);
    if(!outputFile)
    {
      cerr << "Cannot open " << filename << "\n";
      exit(1);
    }
    typename HistoType::iterator it;

    for( it=data.begin(); it!=data.end(); it++)
      outputFile << (int)it->first << " " << it->second << "\n";
    outputFile.close();
  }
  */
  /*template <class T>
  map <T, int, less<T> >& Histogram <T>::getData(){
    return data;
  }
  */
//template <class T,class T2>

  void Histogram::normalize() {
    //cout<<"normalise"<<endl;
    float total = 0;
    /// calculate the total
     map< int, map < int, map<int, float, less<int> >* >* >::iterator it1;
     map< int, map<int, float, less<int> >* >::iterator it2;
     map<int, float, less<int> >::iterator it3;

    for (it1 = data->begin(); it1 != data->end(); it1++) {
      map < int, map<int, float, less<int> >* >* map2 = (*it1).second;

      for (it2 = map2->begin(); it2 != map2->end(); it2++) {
        map<int, float, less<int> >* map3 = (*it2).second;

        for (it3 = map3->begin(); it3 != map3->end(); it3++) {
          total += (*it3).second;
        }
      }
    }

    //* data;
    /// normalize
    for (it1 = data->begin(); it1 != data->end(); it1++) {
      map < int, map<int, float, less<int> >* >* map2 = (*it1).second;
      
      for (it2 = map2->begin(); it2 != map2->end(); it2++) {
        map<int, float, less<int> >* map3 = (*it2).second;
        
        for (it3 = map3->begin(); it3 != map3->end(); it3++) {
          (*it3).second=(*it3).second/total;
        }
      }
    }
  }

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
