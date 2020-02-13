/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h> // uint_least8_t.

#define PORTION 512

bool isjpg(uint_least8_t* data);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: ./recover *.raw\n");
        return 1;
    }

    // get the file name.
    char* infile = argv[1];

    // open the infile.
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL) {
        printf("Could not open the %s file.\n", infile);
        return 2;
    }

    // read the first 512 bytes.
    uint_least8_t* data = malloc(PORTION);
    int count = 0;
    // while it is not the end of the file.
    while(fread(data, PORTION, 1, inptr) == 1) {
        // check if the current 512 bytes header of a jpg file.
        if (isjpg(data)) {
            // create the filename.
            char* filename = (char*) malloc(sizeof(char) * 16);
            sprintf(filename, "Photo(%i).jpg", count);
            // create a new jpg file.
            FILE* outptr = fopen(filename, "w");
            // write the data from *.raw to the new *.jpg file.
            count++;
            do {
                // write the 512 bytes.
                fwrite(data, PORTION, 1, outptr);
                // read next 512 bytes.
                fread(data, PORTION, 1, inptr);
            }
            while(!isjpg(data) && !feof(inptr));

            //close the just created jpg file.
            fclose(outptr);
            // move back by 512 bytes.
            fseek(inptr, -PORTION, SEEK_CUR);
        }
    }


    // close the infile.
    fclose(inptr);
    // success.
    return 0;    
}

bool isjpg(uint_least8_t* data) {
    return *data == 0xff && *(data + 1) == 0xd8 && *(data + 2) == 0xff &&
    (*(data + 3) >= 0xe0 && *(data + 3) <= 0xef);
}