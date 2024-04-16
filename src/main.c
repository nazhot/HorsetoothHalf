#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

#define SPLIT_STRING_LENGTH 256
#define NUM_INPUT_FILES 5

const char inputDirectoryName[] = "input";

static uint splitLine( const uint numSplits, char splitArray[numSplits][SPLIT_STRING_LENGTH], const char *line, const char delimeter ) {
    uint splitArrayIndex = 0;
    uint splitArrayIndexIndex = 0;
    for ( uint i = 0; line[i]; ++i ) {
        if ( line[i] == delimeter ) {
            continue;
        } else if ( i != 0 && line[i - 1] == delimeter ) {
            splitArrayIndex++;
            splitArrayIndexIndex = 0;
        }
        splitArray[splitArrayIndex][splitArrayIndexIndex++] = line[i];
    }
    return splitArrayIndex;
}

int main( int argc, char *argv[] ) {
    DIR *inputDirectory = opendir( inputDirectoryName );
    if ( !inputDirectory ) {
        fprintf( stderr, "Could not open input directory (%s)\n", inputDirectoryName );
        fprintf( stderr, "Error: %i\n", errno );
        exit( 1 );
    }

    FILE *outputFile = fopen( "results.csv", "w" );
    if ( !outputFile ) {
        fprintf( stderr, "Cannot open output file\n" );
        exit( 1 );
    }
    fprintf( outputFile, "Year, Pace (seconds), Total Time (seconds), Total Time (minutes)\n" );
    struct dirent *fileStruct;

    while ( ( fileStruct = readdir( inputDirectory ) ) ) {
        char inputFileName[256];
        strcpy( inputFileName, inputDirectoryName );
        strcat( inputFileName, "/" );
        strcat( inputFileName, fileStruct->d_name );
        FILE *inputFile = fopen( inputFileName, "r" );
        if ( !inputFile ) {
            fprintf( stderr, "Cannot open %s\n", inputFileName );
            continue;
        }
        size_t lineSize = 1024;
        char *lineBuffer = NULL;
        uint netTimeColumn = 0;

        //find the line with the column headers
        while ( getline( &lineBuffer, &lineSize, inputFile )  != -1 ) {
            if ( !lineBuffer ) {
                fprintf( stderr, "Cannot get line of file %s\n", inputFileName );
                continue;
            }
            if ( strncmp( lineBuffer, "Place", 5 ) ) {
                continue;
            }
            printf( "%s: %s", inputFileName, lineBuffer );
            char splitArray[20][SPLIT_STRING_LENGTH] = {0};
            uint splitSize = splitLine( 20, splitArray, lineBuffer, ' ' );
            printf( "Size: %u\n", splitSize );
            for ( uint j = 0; j < splitSize; ++j ) {
                printf( "%u: %s\n", j, splitArray[j] );
                if ( !strncmp( splitArray[j], "Nettime", 7 ) ) {
                    netTimeColumn = j;
                    break;
                }
            }
            break;
        }

        getline( &lineBuffer, &lineSize, inputFile ); //skip line of ====='s

        while ( getline( &lineBuffer, &lineSize, inputFile )  != -1 ) {
            if ( !lineBuffer ) {
                fprintf( stderr, "Cannot get line of file %s\n", inputFileName );
                continue;
            }
            char splitArray[20][SPLIT_STRING_LENGTH] = {0};
            uint splitSize = splitLine( 20, splitArray, lineBuffer, ' ' );
            if ( splitSize == 0 ) {
                continue;
            }
            char *paceString = splitArray[splitSize - 1];
            int paceStringIndex = strlen( paceString ) - 1;
            uint paceSeconds = 0;

            //seconds
            paceSeconds += paceString[paceStringIndex--] - 48;
            paceSeconds += ( paceString[paceStringIndex--] - 48 ) * 10;
            paceStringIndex--;

            //minutes
            uint minuteMultiplier = 1;
            while ( paceStringIndex > -1 && paceString[paceStringIndex] != ':' ) {
                paceSeconds += ( paceString[paceStringIndex--] - 48 ) * 60 * minuteMultiplier;
                minuteMultiplier *= 10;
            }

            paceStringIndex--;

            //hours
            uint hourMultiplier = 1;
            while ( paceStringIndex > -1 && paceString[paceStringIndex] != ':' ) {
                paceSeconds += ( paceString[paceStringIndex--] - 48 ) * 3600 * hourMultiplier;
                hourMultiplier *= 10;
            }

            uint timeSeconds = paceSeconds * 13.1;

            fprintf( outputFile, "%s, %u, %u, %.2f\n",  inputFileName, paceSeconds, timeSeconds, timeSeconds * 1.0 / 60.0 );

        }

        free( lineBuffer );
        fclose( inputFile );
    }

    fclose( outputFile );

    return 0;
}
