#include <iostream>
#include <lib/LibTIM/Common/Image.h>
#include <algo/SIFT.h>
#include <algo/convertImage.h>
#include <time.h>
using namespace std;
using namespace LibTIM;

void DrawLine(Image<U8C3>& image, int r1, int c1, int r2, int c2);
void CombineImagesVertically(Image<U8C3>& im1, Image<U8C3>& im2, Image<U8C3>& result);
void DrawPoint(Image<U8C3>& image, vector<SIFTPoint*>& v);
void DrawPoint(Image<U8C3>& image, int r1, int c1);
  
int main(int argc,char** argv){
  srand(0);
  string a=argv[1];
  string b=argv[2];

  Image<U8C3> imga;
  Image<U8C3>::load(a.c_str(),imga);
  Image<U8> imggraya(imga.getSize());
  cvtRGB2Gray(imga,imggraya);

  Image<U8C3> imgb;
  Image<U8C3>::load(b.c_str(),imgb);
  Image<U8> imggrayb(imgb.getSize());
  cvtRGB2Gray(imgb,imggrayb);

  SIFTAdapter sift;
  vector<SIFTPoint*> pointListA;
  vector<SIFTPoint*> pointListB;
  sift.getSiftPoint(imggraya,pointListA);
  sift.getSiftPoint(imggrayb,pointListB);
    
  int rows = imga.getSizeY() + imgb.getSizeY();
  int cols = max(imga.getSizeX(), imgb.getSizeX());


  int sizeb=pointListB.size();
  double indexb[sizeb];
  sift.matching(pointListA,pointListB,indexb,0.3);

  int sizea=pointListA.size();
  double indexa[sizea];
  sift.matching(pointListB,pointListA,indexa,0.3);
  

  DrawPoint(imga,pointListA);
  DrawPoint(imgb,pointListB);
  
  Image<U8C3> imgc(cols,rows, 1);
  imgc.fill(0);
  CombineImagesVertically(imga,imgb,imgc);

  for(int i=0; i<sizeb; i++){
    SIFTPoint* pb=pointListB[i];
    int aindex=indexb[i];
    if(aindex!=-1){
      if(indexa[aindex]==i){
        SIFTPoint* pa=pointListA[aindex];
        DrawLine(imgc,pa->y,pa->x,pb->y+imga.getSizeY(),pb->x);        
      }
    }
  }
  imgc.save("siftout.jpg");
  
  return 0;
}
void DrawPoint(Image<U8C3>& image, vector<SIFTPoint*>& v){
  for(int i=0; i<v.size(); i++){
    DrawPoint(image,v[i]->y,v[i]->x);
  }
}
void DrawPoint(Image<U8C3>& image, int r1, int c1){
  image(c1,r1)=255;
}

/* Draw a white line from (r1,c1) to (r2,c2) on the image.  Both points
 *  must lie within the image.
 */
void DrawLine(Image<U8C3>& image, int r1, int c1, int r2, int c2)
{
  int i, dr, dc, temp;
  
  if (r1 == r2 && c1 == c2)  /* Line of zero length. */
    return;
  U8C3 color;
  for(int j=0; j<2; j++)
    color[j]=rand()%256;
  /* Is line more horizontal than vertical? */
  if (abs(r2 - r1) < abs(c2 - c1)) {
    
    /* Put points in increasing order by column. */
    if (c1 > c2) {
      temp = r1; r1 = r2; r2 = temp;
      temp = c1; c1 = c2; c2 = temp;
    }
    dr = r2 - r1;
    dc = c2 - c1;
    for (i = c1; i <= c2; i++){
      for(int j=0; j<2; j++)
        image(i,r1 + (i - c1) * dr / dc)[j] = color[j];
    }
      
    
  } else {
    
    if (r1 > r2) {
      temp = r1; r1 = r2; r2 = temp;
      temp = c1; c1 = c2; c2 = temp;
    }
    dr = r2 - r1;
    dc = c2 - c1;
    for (i = r1; i <= r2; i++)
      for(int j=0; j<2; j++)
        image(c1 + (i - r1) * dc / dr,i)[j] = color[j];
  }
}


/* Return a new image that contains the two images with im1 above im2.
 */
void CombineImagesVertically(Image<U8C3>& im1, Image<U8C3>& im2, Image<U8C3>& result)
{
  
  

  int r,c;
  /* Set all pixels to 0,5, so that blank regions are grey. */
  for (r = 0; r < result.getSizeY(); r++)
    for (c = 0; c < result.getSizeX(); c++)
      result(c,r) = 0;
    
    /* Copy images into result. */
    for (r = 0; r < im1.getSizeY(); r++)
      for (c = 0; c < im1.getSizeX(); c++)
        result(c,r) = im1(c,r);
      for (r = 0; r < im2.getSizeY(); r++)
        for (c = 0; c < im2.getSizeX(); c++)
          result(c,r + im1.getSizeY()) = im2(c,r);
        
        
}