// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;
  float scale = (float) MXGRAY/levels;
  float bias = scale/2.0;

  // init lut
  int i, lut[MXGRAY];
  for(i = 0; i < MXGRAY; i++){
    lut[i] = scale * (int)(i/scale) + bias;
    std::cout<<"lut[1]" <<lut[i]<< "\n";
  }

  // evaluate output
  int type;
  ChannelPtr<uchar> p1, p2, endd;
  for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
    IP_getChannel(I2, ch, p2, type);
    int row = 0;
    int col = 0;

    for(endd = p1 + total; p1<endd;){
    // if dither is checked

      if(dither)
      {
        // calculate random jitter
        double random = rand();
        random = random/RAND_MAX;
        random = random * bias;

        int sign;

        // if else block to alternatively set sign to either + or -
        if (row % 2 == 0 && col % 2 == 0)
        {
          sign = 1;
        }
        else if(row %2 == 0 && col % 2 == 1)
        {
          sign = -1;
        }
        else if(row % 2 == 1 && col % 2 == 0)
        {
          sign = -1;
        }
        else
        {
          sign = 1;
        }

        // signed jitter value
        random = sign * random;
        int k = *p1++ + random;

        // clip so that values are in range [0,255]
        if(k < 0)
          k = 0;
        if(k > 255)
          k = 255;

        // evaluate output
        *p2++ = lut[k];

      // advance to next pixel for deciding sign
      if(col == (w -1))
      {
        col = 0;
        row++;
      }
      else
      {
        col++;
      }
    }
      // if dither is not checked
      else
      {*p2++ = lut[*p1++];}
    }
  }
}
