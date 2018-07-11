void blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016

void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();

  // if filter dimensions are even increment to next odd

  if((filterW % 2) == 0)
    filterW++;

  if((filterH % 2) == 0)
    filterH++;

  int type;
  ChannelPtr<uchar> p1, p2;
  for(int ch=0; IP_getChannel(I1,ch,p1,type); ch++){
    IP_getChannel(I2,ch,p2,type);

    // if the filter has a width greater than 1, then blur every row one by one
    if(filterW > 1){

      // iterate through each row
      for(int y=0; y<h; y++){

        // call blur1d function for each row
        blur1D(p1,w,1,filterW,p2);

        // increment to pixel in next row
        p1 += w;
        p2 += w;
      }
    // set the blurred output as our new input in order to blur vertically too if neccessary
    IP_getChannel(I2,ch,p1,type);
    }
    else
      IP_copyImage(I1,I2);

    // if the filter has a height greater than 1, then blur every column one by one
    if(filterH > 1){
      IP_getChannel(I2,ch,p2,type);

      //iterate through each column
      for(int x=0; x<w; x++){

        //call blur1d function for each column
        blur1D(p1,h,w,filterH,p2);

        // increment to pixel in next column
        p1 += 1;
        p2 += 1;
      }
    }
  }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur1D:
//
// Blur src with 1D box filter of width ww.
//
void
blur1D(ChannelPtr<uchar> src, int len, int stride, int ww, ChannelPtr<uchar> dst)
{
  // initializa variables
  uchar *imgPointer;
  uchar *start,*end;
  int total = 0;


  // allocate space for buffer
  uchar *buffer = new uchar[len+ww];

  // pointer to past padding

  imgPointer = buffer + ww/2;

  // copy from input row or column to buffer past padding
  for(int i=0; i<len; i++){
    imgPointer[i] = src[i*stride];
  }

  // start pointer is to begining of buffer & end pointer to end of buffer
  start = buffer;
  end = buffer + len + ww/2;

  // replicate leftmost and rightmost pixels for padding
  for(int i=0; i<ww/2; i++){
    start[i] = imgPointer[0];
    end[i] = imgPointer[len-1];
  }

  // sum all pixels in the buffer
  for(int i=0; i<ww-1; i++)
    total = total + buffer[i];

  int i = ww-1;
  for(int x=0; x<len; x++){
    total = total + buffer[i++];

    // find average and set equal to output pixel
    dst[x*stride] = total/ww;
    total = total - buffer[i-ww];
  }

  // free memory
  delete [] buffer;

}
