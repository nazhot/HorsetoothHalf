#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

#define SPLIT_STRING_LENGTH 1024
#define NUM_INPUT_FILES 5

const char inputDirectoryName[] = "input";
const char outputFileName[] = "output/results.csv";

static uint splitLine( const uint numSplits, char splitArray[numSplits][SPLIT_STRING_LENGTH], const char *line, const char delimeter ) {
    uint splitArrayIndex = 0;
    uint splitArrayIndexIndex = 0;
    uint index = 0;
    
    while ( line[index] == delimeter ) index++; //skip all beginning delimeters

    bool lastWasDelimiter = false;
    while ( line[index] ) {
        if ( line[index] == delimeter ) {
            lastWasDelimiter = true;
            index++;
            continue;
        } else if ( lastWasDelimiter ) {
            splitArrayIndex++;
            splitArrayIndexIndex = 0;
            lastWasDelimiter = false;
        }
        splitArray[splitArrayIndex][splitArrayIndexIndex++] = line[index];
        index++;
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

    FILE *outputFile = fopen( outputFileName, "w" );
    if ( !outputFile ) {
        fprintf( stderr, "Cannot open output file\n" );
        exit( 1 );
    }

    struct dirent *fileStruct = NULL;
    fprintf( outputFile, "Year, Pace (seconds), Total Time (seconds), Total Time (minutes)\n" );

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

        printf( "Working on %s\n", inputFileName );

        size_t lineSize = 1024;
        char *lineBuffer = NULL;

        //find the line with the column headers
        while ( getline( &lineBuffer, &lineSize, inputFile )  != -1 ) {
            if ( !lineBuffer ) {
                fprintf( stderr, "Cannot get line of file %s\n", inputFileName );
                continue;
            }
            if ( !strncmp( lineBuffer, "Place", 5 ) ) { //lines I looked at all start with "Place"
                break;
            }
        }

        getline( &lineBuffer, &lineSize, inputFile ); //skip line of ====='s

        while ( getline( &lineBuffer, &lineSize, inputFile )  != -1 ) {
            if ( !lineBuffer ) {
                fprintf( stderr, "Cannot get line of file %s\n", inputFileName );
                continue;
            }

            char splitArray[20][SPLIT_STRING_LENGTH] = {0};
            const uint splitSize = splitLine( 20, splitArray, lineBuffer, ' ' );
            if ( splitSize == 0 ) {
                continue;
            }

            for ( uint i = 0; i < splitSize; ++i ) {
                printf( "%s ", splitArray[i] );
            }
            printf( "%s\n", inputFileName );


            const char *paceString = splitArray[splitSize - 1];
            int paceStringIndex = strlen( paceString ) - 1;
            uint paceSeconds = 0;

            //seconds
            //always 2 characters at the end
            paceSeconds += paceString[paceStringIndex--] - 48;
            paceSeconds += ( paceString[paceStringIndex--] - 48 ) * 10;
            paceStringIndex--;

            //minutes
            //can be 1-2 characters
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

            const uint timeSeconds = paceSeconds * 13.1; //half marathon

            fprintf( outputFile, "%s, %u, %u, %.2f\n",  fileStruct->d_name, paceSeconds, timeSeconds, timeSeconds * 1.0 / 60.0 );

        }

        free( lineBuffer );
        fclose( inputFile );
    }

    fclose( outputFile );

    return 0;
}
