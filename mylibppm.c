
#include "mylibppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


imgModelType *initImgModel(){
    imgModelType *model;

    model = malloc( sizeof(imgModelType) );
    model->mgNum[0] = model->mgNum[1] = '\0';
    model->domPixel = (pixelType){0, 0, 0};
    model->col = 0;
    model->row = 0;
    model->mxval = 0;
    model->tlPixels = NULL;
    
    return model;
}

void initImgModelPixels( long col, long row, imgModelType *img ){
    
    img->tlPixels = malloc( sizeof(pixelType)*(col*row) );
}

static void write_plain( FILE * ofile, imgModelType* img ){
    int i, size;

    //printing the header of the file
    fprintf( ofile, "P%c\n", img->mgNum[1] );
    fprintf( ofile, "%d %d\n%d\n", img->col, img->row, img->mxval );

    //printinf the pixels array
    size = img->col*img->row;
    for( i = 0; i < size; i++ ){
        fprintf( ofile, "%d ", img->tlPixels[i].rcolor );
        fprintf( ofile, "%d ", img->tlPixels[i].gcolor );
        fprintf( ofile, "%d ", img->tlPixels[i].bcolor );
    }
    fclose(ofile);
}

static void write_raw( FILE *ofile, imgModelType *img ){
    
    //printing the header of the file
    fwrite( img->mgNum, sizeof( img->mgNum[0] ), 2, ofile );
    fprintf( ofile, "\n%d %d", img->col, img->row );
    fprintf( ofile, "\n%d\n", img->mxval );

    //print the pixels array 
    fwrite( img->tlPixels, sizeof( img->tlPixels[0] ), (img->row * img->col) , ofile  );
    fclose(ofile);
}

void write_ppmImg( char* filename, imgModelType *img, int mode ){
    FILE *ofile;

    if( mode != 0 )
        ofile = fopen( filename, "w" );
    else 
        ofile = stdout;

    if ( ofile == NULL ){
        fprintf( stderr, "*ERRO AO ABRIR ARQUIVO DE SAIDA*\n");
        fclose( ofile );
        exit(1); 
    }
    //shift between raw and plain write mode
    if( img->mgNum[1] == RAW_PPM )
         write_raw( ofile, img );
    else if( img->mgNum[1] == PLAIN_PPM )
        write_plain( ofile, img );
    else {
        fprintf( stderr, "ARQUIVO NAO SUPORTADO: %s\n", filename );
        exit(1);
    }
    
    fclose( ofile );
}

static void readHeader(  FILE* ifile, imgModelType *img ){
    unsigned char dummy;
    int rc_read, mxv_read;

    rc_read = 0, mxv_read = 0;
    dummy = fgetc(ifile);
    while ( isspace( dummy ) || ( !rc_read && !mxv_read ) ){
        dummy = fgetc(ifile);
        
        if( isspace(dummy) )
            continue; //continue reading stream until find a non-space charater
        else
            ungetc( dummy, ifile ); //return character to stream and points to it

        if( dummy == '#' ){
            for( ; dummy != '\0' && dummy != '\n'; ) //ignore comments
                dummy = fgetc(ifile);
            
            continue;
        }
        //read once the values for width and height
        if( !rc_read ) {
            fscanf( ifile, "%d %d", &img->col, &img->row );
            rc_read = 1;
            dummy = fgetc(ifile);
            continue;
        }
        //read once the max value for pixels. value between 0-255
        if( !mxv_read ) {
            fscanf( ifile, "%d", &mxv_read );
            img->mxval = mxv_read;
            dummy = fgetc(ifile);
        }
    }
}

static void read_plain(  FILE* ifile, imgModelType *img  ){    
    int buffer[3], i;

    readHeader(ifile, img);
    initImgModelPixels( img->col, img->row, img );

    i = 0;
    while( !feof(ifile) && i < ( img->col*img->row ) ){
        fscanf(ifile, "%d %d %d", 
            buffer, 
            (buffer+1), 
            (buffer+2)
        );
        img->tlPixels[i].rcolor = buffer[0];
        img->tlPixels[i].gcolor = buffer[1];
        img->tlPixels[i].bcolor = buffer[2];
        i++;
    }
}

static void read_raw( FILE* ifile, imgModelType *img ){
    
    readHeader( ifile, img );
    initImgModelPixels( img->col, img->row, img );

    //read the pixels bytes in the file        
    fread( img->tlPixels, sizeof( img->tlPixels[0] ), (img->row * img->col) , ifile  );
}

void read_ppmImg( char *filename, imgModelType *img, int mode ){
    
    FILE *ifile;         //input and output files and tiles
    
    if( mode != 0 )
        ifile = fopen( filename, "r+" );
    else 
        ifile = stdin;

    if ( ifile == NULL ){
        fprintf( stderr, "ERRO AO ABRIR ARQUIVO DE ENTRADA: %s\n", filename );
        exit(1); 
    }
    fread( img->mgNum, sizeof( img->mgNum[0] ), 2, ifile ); //read magic number

    //alternates between raw and plain write mode
    if( img->mgNum[1] == RAW_PPM )
        read_raw( ifile, img );
    else if( img->mgNum[1] == PLAIN_PPM )
        read_plain( ifile, img );
    else{
        fprintf( stderr, "ARQUIVO NAO SUPORTADO: %s\n", filename );
        exit(1);
    }
        
    fclose( ifile );

}


void dominantColor( imgModelType *img ){


    double green, blue, red;
    int i, size;
    
    size = img->col * img->row;
    for( i = 0; i < size; i++ ){
        red += (img->tlPixels[i].rcolor * img->tlPixels[i].rcolor)/size;
        green += (img->tlPixels[i].gcolor * img->tlPixels[i].gcolor)/size;
        blue += (img->tlPixels[i].bcolor * img->tlPixels[i].bcolor)/size;
    }

    red = sqrt( red );
    green = sqrt( green );
    blue = sqrt( blue );

    img->domPixel.rcolor = (unsigned char) red;
    img->domPixel.gcolor = (unsigned char) green;
    img->domPixel.bcolor = (unsigned char) blue;
}


double colorDiff( imgModelType *img1, imgModelType *img2 ){
    long dgreen, dred, dblue;
    long rmean;

    rmean = ( img1->domPixel.rcolor - img2->domPixel.rcolor ) >> 2;

    dred = ( img1->domPixel.rcolor - img2->domPixel.rcolor );
    dred = dred * dred;
    dgreen = ( img1->domPixel.gcolor - img2->domPixel.gcolor );
    dgreen = dgreen * dgreen;
    dblue = ( img1->domPixel.bcolor - img2->domPixel.bcolor );
    dblue =dblue * dblue;

    return sqrt( ( ( 512+rmean )*dred >> 8 ) + ( 4 * dgreen ) + ( ( (767-rmean)*dblue ) >> 8 ) );
}

void deleteImgModel( imgModelType *img ){
    free(img->tlPixels);
    free(img);
}
