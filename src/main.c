#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] ) {
    FILE *inputFile;
    char *inputFileNames[5] = { "2018.txt", "2019.txt", "2021.txt", "2022.txt", "2023.txt" };

    for ( uint i = 0; i < 5; ++i ) {
        inputFile = fopen( inputFileNames[i], "r" );
        if ( !inputFile ) {
            fprintf( stderr, "Cannot open %s\n", inputFileNames[i] );
            continue;
        }
        size_t lineSize = 1024;
        char *lineBuffer;

        while ( getline( &lineBuffer, &lineSize, inputFile )  != -1 ) {
            if ( !lineBuffer ) {
                fprintf( stderr, "Cannot get line of file %s\n", inputFileNames[i] );
                continue;
            }
            if ( !strncmp( lineBuffer, "Place", 5 ) ) {
                printf( "%s: %s", inputFileNames[i], lineBuffer );
            }
        }

        free( lineBuffer );
    }

    return 0;
}
