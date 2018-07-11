// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_gammaCorrect:
//
// Gamma correct image I1. Output is in I2.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016
//
void
HW_gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

  // initialize lookup table
  int i, lut[MXGRAY];

  // apply gamma correction : OutIm = 255 * (InIm/255)^(1/γ)
  for(i = 0; i < MXGRAY; ++i){
    lut[i] = (int) (255 * pow(((double)i/255),1/gamma));
}

  // evaluate output
  int type;
  ChannelPtr<uchar> p1, p2, endd;
  for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
    IP_getChannel(I2, ch, p2, type);
    for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
  }
}
