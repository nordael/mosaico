
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libmosaico.h"
#include <time.h>


/*
This function adjust the program call input to ignore the order of the arguments
*/
int *parseArgument( char ** str, const int count ){
    const char *INPUT_TOKEN = "-i";
    const char *OUTPUT_TOKEN = "-o";
    const char *DIR_TOKEN = "-p";
    const int MAX_PARAM = 3;
    int *input;
    int i, j;
    
    input = malloc( sizeof(int)*MAX_PARAM );
    //the str[0] always is the name of the program
    i = 1, j = 0; 
            
    while( i < count && strcmp( str[i], INPUT_TOKEN  ) != 0 )
        i++;

    input[j++] = ( i < count ) ? i+1: 0;
    
    i = 1;
    while( i < count && strcmp( str[i], OUTPUT_TOKEN  ) != 0 )
        i++;

    input[j++] = ( i < count ) ? i+1: 0;
    
    i = 1;
    while( i < count && strcmp( str[i], DIR_TOKEN  ) != 0 )
        i++;

    input[j++] = ( i < count ) ? i+1: 0;
        
        
    return input;
}


int main( const int argc, char** argv ){

    const int INPUT = 0;
    const int OUTPUT = 1;
    const int TILES = 2;
    
    int *arg_index;                 //stores the order of each paramenter making possible smooth execution
    imgModelType *image;
    TList *tiles;
        
    //parse the out of order program argument for execution
    arg_index = parseArgument( argv, argc );   

    //returns a pointer to a set of images to be used as tiles
    fprintf( stderr, "CARREGANDO DIRETORIO DE PASTILHAS\n" );
    tiles = parseTiles( argv[arg_index[TILES]], arg_index[TILES] );

    //Returns a pointer to a structure container of the image read from file
    fprintf( stderr, "CARREGANDO IMAGEM DE ENTRADA\n" );
    image = parseInputImage( argv[arg_index[INPUT]], arg_index[INPUT] );
    
    //build the mosaic image with tiles available and write it on output directory
    fprintf( stderr, "CRIANDO MOSAICO\n" );
    createMosaic( argv[arg_index[OUTPUT]], image, tiles, arg_index[OUTPUT] ); //creates the final image    

    free(arg_index);
    
    return 0;
}