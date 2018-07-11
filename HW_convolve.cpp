// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Apply image convolution on I1. Output is in I2.

// Written by: Christina Tsangouri and Aashna Shah, 2016

void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{

	// initialize values
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	uchar *out_image;

	int kernel_width = Ikernel->width();
	int kernel_height = Ikernel->height();

	if(kernel_width%2 == 0){
		return;
	}
	if(kernel_height%2 == 0){
		return;
	}

	// allocate space for buffer and row to maximum kernel size
	uchar *buffer[32], *row[32];
	int kernel_width2 = kernel_width / 2;
	int kernel_height2 = kernel_height / 2;

	//row points past padding
	for (int i = 0; i < kernel_height; i++) {
		buffer[i] = new uchar[w+kernel_width-1];
		row[i]  = buffer[i] + kernel_height2;
	}

	  int i, j;
    int type;
    ChannelPtr<uchar> p1, p2;
    ChannelPtr<float> pk, weight;

    IP_getChannel(Ikernel, 0, pk, type);
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
				out_image = (uchar *)&p2[0];

			 // copy 1st row from input to row 0 in buffer
        for(int x = 0; x < w; x++){
            row[0][x]  = *p1++;
        }

				// add padding to left side by replicating leftmost pixel
        for(int x = 1; x <= kernel_width2; x++)  {
            row[0][-x] = row[0][0];
        }

				// add padding to right side by replicating rightmost pixel
        for(int x = w; x < w+kernel_width2; x++) {
            row[0][x]  = row[0][w-1];
				}

			  // copy row 0 from row(0) to row(kernel_height/2)
				for(int i = 0; i <= kernel_height2; i++) {
				    for(j = 0; j < w+(kernel_height2-1); j++) {
				        buffer[i][j] = buffer[0][j];
				      }
				  }

			  // copy the rest of the rows into buffer
				for(int i=kernel_height2+1; i <kernel_height ; i++) {

						// copy each row from input to corresponding row in buffer
				    for(int x = 0; x < w; x++){
				        row[i][x] = *p1++;  // copy row
				    }

					  // add padding to left side by replicating leftmost pixel
				    for(int x = 1; x <= kernel_width2; x++) {
				        row[i][-x] = row[i][0];
				    }

						// add padding to right side by replicating rightmost pixel
				    for(int x = w; x < w+kernel_width2; x++){
				        row[i][x] = row[i][w-1];
				      }
				}

        int sum;
				int index = 0;
        for(int y = 0; y < h; y++) {
            for(int x = 0; x < w; x++) {

            	sum = 0;
            	weight = pk;

							// convolution
            	for(int i = 0; i < kernel_height; i++) {
            		for(int j = -kernel_width2; j <= kernel_width2; j++) {
            			sum += ((int) row[i][x+j] * *weight++);
            		}
            	}
            	out_image[index] = CLIP(sum, 0, MaxGray);
							index++;
            }

						//advance rows
            for(int i = 0; i < kernel_height-1; i++){
            	row[i] = row[i+1];
            }

            row[i] = buffer[y % kernel_height] + kernel_width2;

						// copy from next row from input to buffer
            if(y < h - kernel_height) {
            	for(int x = 0; x < w; x++) {
                    row[i][x]  = *p1++;
                }

							// add padding to left side by replicating leftmost pixel
            	for(int x = 1; x <= kernel_width2; x++){
                     row[i][-x] = row[i][0];
             }

						 // add padding to right side by replicating rightmost pixel
            	for(int x = w; x < w + kernel_width2; x++){
                    row[i][x]  = row[i][w-1];
                }
            }
        }
	}

	// delete memory
	for(int i = 0; i < kernel_height; i++){
		delete [] buffer[i];
    }
}
