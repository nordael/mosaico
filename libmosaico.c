

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include "libmosaico.h"



imgModelType * parseInputImage( char* path, int itype ){
    imgModelType *img;
    
    img = initImgModel();
    read_ppmImg( path, img, itype );

    return img;
}


TList* parseTiles( char* path, int ptype ){

    const char *DEFAULT_TILES = "./tiles/";

    imgModelType *img;           //a list of tiles to bex used
    TList *tiles;                 //list of tiles
    struct dirent *entrie;
    DIR *dirTiles;
    char buffer[1024];          //a buffer to build the file path

    img = initImgModel();
    tiles = newList();

    if ( ptype == 0 )
        dirTiles = opendir( DEFAULT_TILES );
    else
        dirTiles = opendir( path );

    
    if ( dirTiles == NULL ){
        fprintf( stderr, "*ERRO AO ABRIR DIRETORIO DE PASTILHAS INFORMADO*\n");
        exit(1); //what code I should return for directory error?
    }
    
    entrie = readdir(dirTiles); //current directory
    entrie = readdir(dirTiles); //prevous directory
    entrie = readdir(dirTiles);
    if( entrie == NULL ){
        fprintf( stderr, "*DIRETORIO DE PASTILHAS VAZIO* \n");
        exit(1); //what code I should return for directory error?
    }
    
    while( entrie ){
        
        if( ptype != 0 )
            snprintf( buffer, sizeof(buffer[0])*1024 ,"%s%s", path, entrie->d_name );
        else
            snprintf( buffer, sizeof(buffer[0])*1024 ,"%s%s", DEFAULT_TILES, entrie->d_name );

        read_ppmImg( buffer, img, 1 );                 
        dominantColor( img );                          //calcule main color of a tile
        _enqueue( tiles, img, sizeof(imgModelType) );  //add img into list of tiles
        entrie = readdir(dirTiles);
    }
    closedir(dirTiles);
    free(img);
    return tiles;
}

//breaks an image into pieces with row and col dimensions
static TList *breakImage( imgModelType *img, long row, long col  ){
    TList *tiles;
    imgModelType *imgChunk;
    long i, j, m, n, index1, index2;

    tiles = newList();
    //creates a new instance of a imgChunk
    imgChunk = initImgModel();

    for( i = 0; i < img->row; i += row ){
        for( j = 0; j < img->col; j += col ){
            index1 = (i*img->col)+j; //move through rol x col blocks
            
            initImgModelPixels( col, row, imgChunk );  //creates a new pixel array with given row and col

            for( m = 0; (i+m) < img->row && m < row; m++ ){
                for( n = 0; (j+n) < img->col && n < col; n++ ){
                    index2 = index1 + (m*img->col) + n;     //index to move inside the chunk

                    imgChunk->tlPixels[(m*col)+n].rcolor = img->tlPixels[index2].rcolor;  //copy each pixel into chunk
                    imgChunk->tlPixels[(m*col)+n].gcolor = img->tlPixels[index2].gcolor;
                    imgChunk->tlPixels[(m*col)+n].bcolor = img->tlPixels[index2].bcolor;
                }
            }
            imgChunk->col = n;
            imgChunk->row = m;
            dominantColor( imgChunk );
            _enqueue( tiles, imgChunk, sizeof(imgModelType) ); //enqueue the chunk for manipulation   
        }
    }
    free(imgChunk);

    return tiles;
}

//copy and paste pixels from src to dest 
static void replaceImageChunk( imgModelType *dest, imgModelType *src ){
    int i,j, index, row, col;

    row = (dest->row < src->row) ? dest->row : src->row ;
    col = (dest->col < src->col) ? dest->col : src->col ;

    for( i = 0; i < row; i++ ){
        for( j = 0; j < col; j++ ){
            index = (i*dest->col)+j;
            dest->tlPixels[ index ].rcolor = src->tlPixels[ index ].rcolor;
            dest->tlPixels[ index ].gcolor = src->tlPixels[ index ].gcolor;
            dest->tlPixels[ index ].bcolor = src->tlPixels[ index ].bcolor;
        }
    }
}

//restore the fragmented image made with tiles
static void uniteImageChunks( TList *chunks, imgModelType *img ){
    
    imgModelType *imgChunk;
    long i, j, m, n, index1, index2;

    //creates a new instance of a imgChunk
    imgChunk = initImgModel();
    _dequeue( chunks, imgChunk, sizeof(imgModelType) ); //recover the the modified chunk 
    
    //move through blocks inside of the image
    for( i = 0; i < img->row; i += imgChunk->row ){
        for( j = 0; j < img->col; j += imgChunk->col ){

            index1 = (i*img->col)+j;
            //move inside the tiles 
            for( m = 0; m < imgChunk->row; m++ ){
                for( n = 0; n < imgChunk->col; n++ ){

                    index2 = index1 + (m*img->col) + n;

                    img->tlPixels[index2].rcolor = imgChunk->tlPixels[(m*imgChunk->col)+n].rcolor;
                    img->tlPixels[index2].gcolor = imgChunk->tlPixels[(m*imgChunk->col)+n].gcolor;
                    img->tlPixels[index2].bcolor = imgChunk->tlPixels[(m*imgChunk->col)+n].bcolor;
                }
            }
            _enqueue( chunks, imgChunk, sizeof(imgModelType) ); //returns the tile to end of the line
            _dequeue( chunks, imgChunk, sizeof(imgModelType) ); //get the next chunk to be overriden
        }
    }
    free(imgChunk);
}

//deallocate the data from list;
static void purgeData( TList * target ){

    imgModelType *imgdump;
    imgdump = initImgModel();

    while( length(target) > 0 ){
        _dequeue( target, imgdump, sizeof(imgModelType) );
        free(imgdump->tlPixels);
    }
    free(imgdump);
    deleteList(target);
}

void createMosaic( char *filename, imgModelType *img, TList *tiles, int mode ){
    imgModelType *imgChunk, *imgTile, *target;
    TList *chunks;
    long i, j, clen, tlen;
    double diff, mindiff;

    imgTile = initImgModel();
    imgChunk = initImgModel();

    _pop( tiles, imgTile, sizeof(imgModelType) );
    chunks = breakImage( img, imgTile->row, imgTile->col  );         //breaks the original image into chunks of a given row and columns
    
    //getting a redmean value for comparison
    //_pop( chunks, imgChunk, sizeof(imgModelType) );

    //mindiff = colorDiff( imgChunk, imgTile );
    //retuns to original state
    _push( tiles, imgTile, sizeof(imgModelType) );
    //_push( chunks, imgChunk, sizeof(imgModelType) );

    clen = length( chunks );
    tlen = length( tiles );

    for( i = 0; i < clen; i++ ){
        mindiff = MAXCOLOR;
        _dequeue( chunks, imgChunk, sizeof(imgModelType) );
        for( j = 0; j < tlen; j++ ){
            _dequeue( tiles, imgTile, sizeof(imgModelType) );
            diff = colorDiff( imgChunk, imgTile );
            if( diff < mindiff ){
                mindiff = diff;
                //save the last tile with best similarity ratio
                target = (imgModelType*) _enqueue( tiles, imgTile, sizeof(imgModelType) );
                continue; 
            }
            _enqueue( tiles, imgTile, sizeof(imgModelType) );
        }
        //replace the block of the original image with similar tile
        replaceImageChunk( imgChunk, target );     
        _enqueue( chunks, imgChunk, sizeof(imgModelType) ); //restores it
    }    

    uniteImageChunks( chunks, img );
    write_ppmImg( filename, img, mode );
    
    //this can be troublesome, because we need to deal with the pixels array allocated
    //before completely delete list
    purgeData( chunks );
    purgeData( tiles );

    free(imgTile);
    free(imgChunk);
    deleteImgModel( img );
}
