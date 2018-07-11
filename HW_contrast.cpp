// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
//
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying
// difference by "contrast" and adding it back to 128. Shift result by
// adding "brightness" value.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016
//
void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
  int w = I1->width ();
  int h = I1->height();
  int total = w * h;

  // init lookup table
  int i, lut[MXGRAY];

  // apply contrast enhancement
  for(i=0; i<MXGRAY; ++i){
    double tran = (((i - 128) * contrast) + 128) + brightness;

  // clip so that values are in range [0,255]
    if(tran <0)
      tran = 0;
    if(tran > 255)
      tran = 255;

    // cast to int
    lut[i] = (int)tran;
  }

  // evaluate output
  int type;
  ChannelPtr<uchar> p1, p2, endd;
  for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
    IP_getChannel(I2, ch, p2, type);
    for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
  }
}
