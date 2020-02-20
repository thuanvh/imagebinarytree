#include <QWidget>
#include <qpainter.h>
#include <QApplication>
#include <QMouseEvent>

#include "../ui/treeqtw.h"

/*
int main(int argc,char** argv) {
    QApplication app(argc, argv);
    OvenTimer foo;
    foo.show();
    return app.exec();

    return 0;
}
*/

// lower_bound/upper_bound example
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main () {
  double myints[] = {10,20,30,30,20,10,10,20};
  vector<double> v;
  for(int i=0; i<8; i++) v.push_back(myints[i]);//v[i]=myints[i];//(myints,myints+8);           // 10 20 30 30 20 10 10 20
  vector<double>::iterator low,up;
  
  sort (v.begin(), v.end());                // 10 10 10 20 20 20 30 30
  
  low=lower_bound (v.begin(), v.end(), 5); //          ^
  up= upper_bound (v.begin(), v.end(), 200); //                   ^
  
  cout << "lower_bound at position " << int(low- v.begin()) << endl;
  cout << "upper_bound at position " << int(up - v.begin()) << endl;
  
  return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
