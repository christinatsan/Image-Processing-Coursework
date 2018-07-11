extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016

void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
  int total = w*h;
  unsigned char *in_image, *out_image, *blur_image;

  // blur input image
  ImagePtr blurredI1;
  HW_blur(I1,size,size,blurredI1);

  int type;
  ChannelPtr<uchar> p1, p2, p3;
  for(int ch=0; IP_getChannel(I1,ch,p1,type); ch++){
    IP_getChannel(I2,ch,p2,type);
    IP_getChannel(blurredI1,ch,p3,type);
    in_image = (uchar *)&p1[0];
    out_image = (uchar *)&p2[0];
    blur_image = (uchar *)&p3[0];

    for(int i=0; i<total; i++){

    // subtract blurred image from input image
    double sub = in_image[i] - blur_image[i];

    // multiply difference by factor
    double mult = sub*factor;

    // add product to input image
    double temp = in_image[i] + mult;

    // clip values
    if(temp > 255)
      temp = 255;
    if(temp < 0)
      temp = 0;

    // set new value to output pixel
    out_image[i] = (unsigned char) temp;
    }
  }
}
