// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
// Written by: Christina Tsangouri and Aashna Shah, 2016
//
void
HW_histoMatch(ImagePtr I1, ImagePtr Ilut, ImagePtr I2)
{
  // initialize and declare variables
  int i, p, R;
  int left[MXGRAY], right[MXGRAY], lim[MXGRAY], indx[MXGRAY];
  int total, Hsum, Havg, h1[MXGRAY], *h2;
  unsigned char *in, *out;
  double scale;

  int width1 = I1 ->width();
  int height1 = I1 ->height();

  int width2 = I2 ->width();
  int height2 = I2 ->height();

  // total number of pixels in image
  total = height1 * width1;

  // init I2 dimensions and buffer
  width2 = width1;
  height2 = height1;
  IP_copyImageHeader(I1, I2);

  // clear histogram
  for(i=0; i<MXGRAY; i++) h1[i] = 0;

  // evaluate histogram
  int type1;
  ChannelPtr<uchar> p1, endd1;
  for(int ch = 0; IP_getChannel(I1, ch, p1, type1); ch++) {
     for(endd1 = p1 + total; p1<endd1;)
       h1[*p1++]++;
   }

  // target histogram
  int type2;
  ChannelPtr<int> p2;
  IP_getChannel(Ilut, 0, p2, type2);
       h2 = (int *)&p2[0];

  // normalize h2 to conform with dimensions of I1
  for(i=Havg=0; i<MXGRAY; i++){
      Havg += h2[i];
  }
  scale = (double) total / Havg;
  if(scale != 1)
  {
    for(i=0; i<MXGRAY; i++)
    {
      h2[i] *= scale;
    }
  }

  R = 0;
  Hsum = 0;

  /* evaluate remapping of all input gray levels;
  Each input gray value maps to an interval of valid output values.
  The endpoints of the intervals are left[] and right[] */
  for(i=0; i<MXGRAY; i++)
  {
    left[i] = indx[i] = R; // left end of interval
    lim[i] = h2[R] - Hsum;
    Hsum += h1[i]; // cumulative value for interval

    while(Hsum>h2[R] && R<MXGRAY-1)
    {
      // compute width of interval
      Hsum -= h2[R]; /* adjust Hsum as interval widens */
      R++; /* update */
    }
    right[i] = R; /* init right end of interval */
  }

  // clear h1 and reuse it below
  for(i=0; i<MXGRAY; i++)
  {
      h1[i] = 0;
  }

  int type3;
  ChannelPtr<uchar> p3,p4, endd3;
  for(int ch = 0; IP_getChannel(I1, ch, p3, type3); ch++)
    IP_getChannel(I2, ch, p4, type3);
      in = (uchar *)&p3[0];
      out = (uchar *)&p4[0];

      // visit all input pixels and make target histogram limit for each intensity value is enforced
      for(i=0; i<total; i++)
       {
          p = indx[in[i]];
          if(p == left[in[i]])
          {
            if(lim[in[i]]-- <= 0)
            {
              p = indx[in[i]] = MIN(p+1, MaxGray);
            }
            out[i] = p;
          }
          else if(p < right[in[i]])
          {
           if(h1[p] < h2[p])
           {out[i] = p;}
           else
           {out[i] = indx[in[i]] = MIN(p+1, MaxGray);}
          }
          else
            out[i] = p;
            h1[p]++;
     }
   }
