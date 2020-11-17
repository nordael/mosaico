
#ifndef ___MYLIBPPM_H_
#define ___MYLIBPPM_H_

#define RAW_PPM '6'
#define PLAIN_PPM '3'
#define MAXCOLOR 256



typedef struct pix{
    unsigned char rcolor;
    unsigned char gcolor;
    unsigned char bcolor;
} pixelType;

typedef struct img{
    unsigned char mgNum[2];         //the tile magic number
    pixelType domPixel;             //the tile dominant color. Only set for tiles images, otherwise is negative
    int col,row;                    //number of columns and row of the image
    unsigned char mxval;                     //max value of pixel color
    pixelType *tlPixels;            //an array of row x col pixels
} imgModelType;

//returns a pointer to an empty imgModelType
imgModelType *initImgModel();

//allocates memory for pixels of a given imgModelType
void initImgModelPixels( long col, long row, imgModelType *img );

//computes the dominant color of a single ppm image
void dominantColor( imgModelType *ppmimg );

//read the ppm image from path filename ( mode != 0) ou standard input (mode == 0) ignoring path
void read_ppmImg( char* filename, imgModelType *img, int mode );


//write the file into the filename path the entity imgModelType
void write_ppmImg( char* filename, imgModelType *img, int mode );

//this method computes the redmean of the dominant color of the images
double colorDiff( imgModelType *img1, imgModelType *img2 );

//computes the square root of square mean of each pixel components
void dominantColor( imgModelType *ppmimg );

//deallocates the memory for imgModelType
void deleteImgModel( imgModelType *img );


#endif