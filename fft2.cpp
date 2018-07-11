#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#define PI 3.14159265358979323846

typedef struct
{
	int len;
	float* real;
	float* imag;
}complexS, *complexP;

void initComplexP(complexP com, int height);
void readIn(char* inFile, complexP com);
void FFT(complexP in, bool dir, complexP out);
void writeOut(char* outFile, complexP com);
void printComplex(complexP p);

int main(){
    complexP input = new complexS; //malloc(sizeof(complexS));
    complexP output = new complexS;//malloc(sizeof(complexS));
	  readIn("dft_input.txt", input);
    FFT(input, 0, output);
    printComplex(output);

	return 0;
}

void initComplexP(complexP com, int height){
    //init
    if (com == NULL) {
        com = new complexS; //malloc(sizeof(complexS));
    }
    com->len = height;
    com->real = (float *) malloc(sizeof(float) * height);
    com->imag = (float *) malloc(sizeof(float) * height);
}

void readIn(char* inFile, complexP com){
	FILE *in = fopen(inFile, "r");

    //scan width and height
	int width;
	int height;
	fscanf(in, "%d %d", &width, &height);
    initComplexP(com, height);

    //read numbers
    for (int i = 0; i < height; i++) {
        fscanf(in, "%f %f", com->real + i, com->imag + i);
    }

	fclose(in);

	for(int i=0; i<com->len; i++){
		printf("%f\n",com->real[i]);
	}
}

void FFT(complexP in,bool dir, complexP out){

    initComplexP(out, in->len);

    int n = in->len;

  //  if(!poweroftwo(n)){
  //      DFT(in,dir,out);
  //      return;
//    }

    out->len = n;
    out->real = new float[out->len];
    out->imag = new float[out->len];
    int d;
    int direction = (dir) ? -1: 1;  // forward or inverse DFT

    if(n == 1) //bottom of tree, do nothing.
    {
        for(int i = 0; i<n; i++){
            out->real[i] = in->real[i];
            out->imag[i] = in->imag[i];
        }

    }else{
        complexP even, odd;
        even = new complexS;
        odd = new complexS;
        even->len = n/2;
        even->real = new float[even->len];
        even->imag = new float[even->len];
        odd->len = n/2;
        odd->real = new float[odd->len];
        odd->imag = new float[odd->len];
        //build even
        for(int i = 0; i < even->len; i++){
            even->real[i] = in->real[i*2];
            even->imag[i] = in->imag[i*2];
        }
        //build odd
        for(int i = 0; i < odd->len; i++){
            odd->real[i] = in->real[i*2+1];
            odd->imag[i] = in->imag[i*2+1];
        }
        complexS f_even,f_odd;
        FFT(even,dir,&f_even);
        FFT(odd,dir,&f_odd);
        int i;
        for(i = 0; i < n/2; i++){
            float w_imag;
            float w_real;
            w_imag = sin(direction* -2*PI*i/(n));
            w_real = cos(direction* -2*PI*i/(n));

            out->real[i] = w_real*f_odd.real[i] - w_imag* f_odd.imag[i] + f_even.real[i];
            out->imag[i] = w_real*f_odd.imag[i] + w_imag*f_odd.real[i] + f_even.imag[i];
            out->real[i+n/2] = f_even.real[i] - w_real*f_odd.real[i] - w_imag* f_odd.imag[i];
            out->imag[i+n/2] = f_even.imag[i] - w_real*f_odd.imag[i] + w_imag*f_odd.real[i];
        }

        for (int i = 0; i < n; i++) {
            if(dir) {
            out->real[i] = out->real[i];  // synthesis
            out->imag[i] = out->imag[i];
            } else {
            out->real[i] = out->real[i]/2;  // analysis
            out->imag[i] = out->imag[i]/2;
            }
        }
    }
}


void writeOut(char* outFile, complexP com){
    FILE *out = fopen(outFile, "w");

    //write header
    fprintf(out, "2 %d\n", com->len);

    //write real and complex
    for (int i = 0 ; i < com->len; i++) {
        fprintf(out, "%f %f\n", com->real[i], com->imag[i]);
    }

    fclose(out);
}


void printComplex(complexP p){
        for (int i = 0; i < p->len; i++) {
            printf("real %f imag %f \n", p->real[i], p->imag[i]);
        }
}
