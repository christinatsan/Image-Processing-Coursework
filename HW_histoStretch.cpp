// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoStretch:
//
// Apply histogram stretching to I1. Output is in I2.
// Stretch intensity values between t1 and t2 to fill the range [0,255].
//
// Written by: Christina Tsangouri and Aashna Shah, 2016
//
void
HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
  // Declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

  // init lookup table
  int i, lut[MXGRAY];
  for(i = 0; i < MXGRAY; i++){
    double tran = (double)(i-t1)/(t2-t1) * 255;

  // clip so that values are in range of [0,255]
    if(tran < 0)
      tran = 0;
    if(tran > 255)
      tran = 255;
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
