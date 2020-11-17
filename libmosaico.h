
#ifndef __LIBMOSAICO_H
#define __LIBMOSAICO_H

#include "mylibppm.h"
#include "generic_list.h"


//reads the ppm image from input path (if itype!=0) or standard input (if itype == 0) 
imgModelType * parseInputImage( char* path, int itype );

//returns a pointer to a list of images to be used as tiles
TList *parseTiles( char* path, int ptype );

//builds the mosaic imagem given in a specified output mode using a list of tiles
void createMosaic( char *filename, imgModelType *img, TList *tiles, int mode );


#endif