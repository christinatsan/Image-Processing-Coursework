#include <algorithm>
#include <vector>
using std::vector;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.

// Written by: Christina Tsangouri and Aashna Shah, 2016

void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;
  uchar *out_image;
  int median;

  // clamp to 9
  if(sz > 9)
    sz = 9;

  // if filter is even increment to next odd
  if(sz%2 == 0)
    sz++;

  // allocate space for buffer to maximum filter size 9
  uchar *buffer[9];

  for(int i=0; i<sz; i++){
    buffer[i] = new uchar[w+(sz-1)];
  }

  // allocate space for row to maximum filter size 9 - row points past padding
  uchar *row[9];
  for(int i=0; i<sz; i++){
    row[i] = buffer[i] + sz/2;
  }

  int type,i,j;
  std::vector<uchar> bufferV;
  ChannelPtr<uchar> p1,p2, endd;
  for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++){
    IP_getChannel(I2, ch, p2, type);
    out_image = (uchar *)&p2[0];

     // copy 1st row from input to row 0 in buffer
    for(int x = 0; x < w; x++){
      row[0][x] = *p1++;
    }

    // add padding to left side by replicating leftmost pixel
    for(int x = 1; x <= sz/2; x++){
      row[0][-x] = row[0][0];
    }

    // add padding to right side by replicating rightmost pixel
    for(int x = w; x < w+(sz/2); x++){
      row[0][x] = row[0][w-1];
    }

    // copy row 0 from row(0) to row(sz/2)
    for(int i = 0; i <= sz/2; i++) {
        for(j = 0; j < w+((sz/2)-1); j++) {
          buffer[i][j] = buffer[0][j];
        }
    }

    // copy the rest of the rows into buffer
    for(int i=(sz/2)+1; i < sz; i++) {

      // copy each row from input to corresponding row in buffer
        for(int x = 0; x < w; x++){
          row[i][x] = *p1++;
        }

        // add padding to left side by replicating leftmost pixel
        for(int x = 1; x <= sz/2; x++) {
          row[i][-x] = row[i][0];
        }

        // add padding to right side by replicating rightmost pixel
        for(int x = w; x < w+(sz/2); x++){
           row[i][x] = row[i][w-1];
         }
    }

    // iterate through each pixel in input image
    int index = 0;
    for(int y=0; y<h; y++){
      for(int x=0; x<w; x++){
        bufferV.clear();

        // copy neighborhood into buffer
        for(int i=0; i<sz; i++){
          for(int s=-(sz/2); s<=sz/2; s++){
            bufferV.push_back(row[i][x+s]);
          }
        }

        // sort buffer
        std::sort(bufferV.begin(), bufferV.end());

        // find the median
        median = bufferV[sz*sz/2];

        // copy median into corresponding output pixel
        out_image[index] =  median;
        index++;

      }

      // advance rows
      for (j = 0; j < sz - 1; j++) {
          row[j] = row[j+1];
      }

      row[j] = buffer[y%sz] + sz/2;

      if(y < h-sz){
          for(int x = 0; x < w; x++)
              row[j][x] = *p1++;
      }

    }

  }
}
