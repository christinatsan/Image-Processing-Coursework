void gammaCorrect(ImagePtr, double, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.

// Written by: Christina Tsangouri and Aashna Shah, 2016

void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
  // declare and initialize variables
  IP_copyImageHeader(I1, I2);
  ImagePtr gammaImg;
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;
  int error;
  uchar *out_image;

  gammaCorrect(I1,gamma,gammaImg);

  // allocate space for circular buffer to maximum needed for JJN (3)
  short *buffer[3];
  buffer[0] = new short[w+4];
  buffer[1] = new short[w+4];
  buffer[2] = new short[w+4];

  //allocate space for rows pointing past padding
  short *row0 = buffer[0] + 2;
  short *row1 = buffer[1] + 2;
  short *row2 = buffer[2] + 2;

  //initialize weights for each algorithm

  double floyd[4];

  floyd[0] = 7/(double)16;
  floyd[1] = 5/(double)16;
  floyd[2] = 3/(double)16;
  floyd[3] = 1/(double)16;

  double jarvis[4];

  jarvis[0] = 7/(double)48;
  jarvis[1] = 5/(double)48;
  jarvis[2] = 3/(double)48;
  jarvis[3] = 1/(double)48;

  int type;
  ChannelPtr<uchar>p1,p2,endd;
  int thr = MaxGray >> 1;
  for(int ch=0; IP_getChannel(gammaImg,ch,p1,type); ch++){
    IP_getChannel(I2,ch,p2,type);

    //Floyd-Steinberg
    if(method == 0){

        // copy pixels from input image to first row 0 in buffer
        for(int x=0; x<w; x++) {
          row0[x] = *p1++;
        }

        // replicate leftmost pixel with 1st pixel from input
        row0[-1] = row0[0];

        // replicate rightmost pixel with last pixel in row from input
        row0[w] = row0[w-1];

        // copy pixels from input image to first row 1 in buffer
        for(int x=0; x<w; x++){
          row1[x] = *p1++;
        }

        // replicate leftmost pixel with 1st pixel from input
        row1[-1] = row1[0];

        // replicate rightmost pixel with last pixel in row from input
        row1[w] = row1[w-1];
    }

      //Jarvis-Judice-Ninke
      if(method == 1) {

            // copy pixels from input image to first row 0 in buffer
            for(int x=0; x<w; x++){
              row0[x] = *p1++;
            }

            // replicate leftmost pixel with 1st pixel from input
            row0[-2] = row0[-1] = row0[0];

            // replicate rightmost pixel with last pixel in row from input
            row0[w+1] = row0[w] = row0[w-1];

            // copy pixels from input image to first row 1 in buffer
            for(int x=0; x<w; x++){
               row1[x] = *p1++;
            }

            // replicate leftmost pixel with 1st pixel from input
            row1[-2] = row1[-1] = row1[0];

            // replicate rightmost pixel with last pixel in row from input
            row1[w+1] = row1[w] = row1[w-1];

            // copy pixels from input image to first row 2 in buffer
            for(int x=0; x<w; x++){
               row2[x] = *p1++;
            }

            // replicate leftmost pixel with 1st pixel from input
            row2[-2] = row2[-1] = row2[0];

            // replicate rightmost pixel with last pixel in row from input
            row2[w+1] = row2[w] = row2[w-1];

      }

      // iterate through each row in input image
      for(int y=0; y<h; y++){
        out_image = (uchar *)&p2[0];

        // if we are at an odd row or if serpentine is not checked do left to right
        if(!(y%2) || !serpentine){

          //iterate through each row
          for(int x=0; x<w; x++){

            // threshold
            if(row0[x] < thr)
              out_image[x] = 0;
            else out_image[x] = MaxGray;

            //find the error by subtracting the thresholded value from the original input for
            // each pixel
            int error = row0[x] - out_image[x];

            // add the errors to neighbors according to each algorithm

            //Floyd-Steinberg
            if(method == 0){

                // multiply error with weight and add to original pixel
                row0[x+1] += error * floyd[0];
                row1[x-1] += error * floyd[2];
                row1[x] += error * floyd[1];
                row1[x+1] += error * floyd[3];
              }

            //Jarvis-Judice-Ninke
            if(method == 1){

                // multiply error with weight and add to original pixel
                row0[x+1] += error * jarvis[0];
                row0[x+2] += error * jarvis[1];
                row1[x-2] += error * jarvis[2];
                row1[x-1] += error * jarvis[1];
                row1[x] += error * jarvis[0];
                row1[x+1] += error * jarvis[1];
                row1[x+2] += error * jarvis[2];
                row2[x-2] += error * jarvis[3];
                row2[x-1] += error * jarvis[2];
                row2[x] += error * jarvis[1];
                row2[x+1] += error * jarvis[2];
                row2[x+2] += error * jarvis[3];

              }
            }
          }

          // if serpentine then go right to left
          else{

            // iterate through each row from the last to first pixel
            for(int x=w-1; x>=0; x--){

              // threshold
              if(row0[x] < thr)
                out_image[x] = 0;
              else out_image[x] = MaxGray;

              //find the error by subtracting the thresholded value from the original input for
              // each pixel
              int error = row0[x] - out_image[x];

              //Floyd-Steinberg
              if(method == 0){

                  // multiply error with weight and add to original pixel
                  row1[x-1] += error * floyd[0];
                  row0[x+1] +=  error * floyd[2];
                  row1[x] += error * floyd[1];
                  row1[x-1] += error * floyd[3];
                }

                //Jarvis-Judice-Ninke
                if(method == 1){

                  // multiply error with weight and add to original pixel
                  row0[x-1] += error * jarvis[0];
                  row0[x-2] += error * jarvis[1];
                  row1[x+2] += error * jarvis[2];
                  row1[x+1] += error * jarvis[1];
                  row1[x] += error * jarvis[0];
                  row1[x-1] += error * jarvis[1];
                  row1[x-2] += error * jarvis[2];
                  row2[x+2] += error * jarvis[3];
                  row2[x+1] += error * jarvis[2];
                  row2[x] += error * jarvis[1];
                  row2[x-1] += error * jarvis[2];
                  row2[x-2] += error * jarvis[3];
                }
              }
            }

        // advance rows
        if(method == 0){

           //Floyd-Steinberg
            row0 = row1;
            row1 = buffer[y%2] + 2;
            if(y < h-2){

              // copy from next row from input to buffer
              for(int x=0; x<w; x++){
                row1[x] = *p1++;
              }
            }
          }

        //Jarvis-Judice-Ninke
        if(method == 1){
              row0 = row1;
              row1 = row2;
              row2 = buffer[y%3] + 2;

              if(y<h-3){
                // copy from next row from input to buffer
                for(int x=0; x<w; x++){
                   row2[x] = *p1++;
                 }
              }
            }
          p2+=w;
          }
        }

        //free memory
        for(int i=0; i<3; i++){
          delete [] buffer[i];

        }
      }


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gammaCorrect:
//
// Apply gamma correction to image I1.
// Save result in I2.
//
void
gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
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
