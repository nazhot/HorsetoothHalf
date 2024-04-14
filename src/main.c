#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {
    FILE *inputFile;
    char *inputFileNames[5] = { "2018.txt", "2019.txt", "2021.txt", "2022.txt", "2023.txt" };

    for ( uint i = 0; i < 5; ++i ) {
        inputFile = fopen( inputFileNames[i], "r" );
        if ( !inputFile ) {
            fprintf( stderr, "Cannot open %s\n", inputFileNames[i] );
            continue;
        }
    }

    return 0;
}
