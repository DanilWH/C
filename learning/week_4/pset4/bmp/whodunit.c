/*
 * The programm restores a noised photo.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./whodunit infile outfile\n");
        return 1;
    }

    // remember the file names.
    char* infile = argv[1];
    char* outfile = argv[2];

    // open the input file.
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL) {
        printf("Could not open the %s file\n", infile);
        return 2;
    }

    // open the output file.
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL) {
        fclose(inptr);
        fprintf(stderr, "Could not create the %s file\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER.
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER.
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER.
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER.
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines.
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines.
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++) {

        RGBTRIPLE tripl;
        fread(&tripl, sizeof(RGBTRIPLE), 1, inptr);
        tripl.rgbtRed = 0xff;
        tripl.rgbtGreen = 0xff;
        tripl.rgbtBlue = 0xff;
        fwrite(&tripl, sizeof(RGBTRIPLE), 1, outptr);

        // iterate over pixels scanlines.
        for (int j = 1; j < bi.biWidth; j++) {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB from infile.
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            if (triple.rgbtRed == 0xff && triple.rgbtGreen == 0x00 &&
                triple.rgbtBlue == 0x00)
            {
                //triple = *(&triple + abs(bi.biHeight) * i + j);
                triple = tripl;
            }
            else {
                tripl = triple;
            }
            
            // write RGB triple to outfile.
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any.
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how).
        for (int k = 0; k < padding; k++) {
            fputc(0x00, outptr);
        }
    }

    // close infile.
    fclose(inptr);

    // close outfile.
    fclose(outptr);

    // that's all folks.
    return 0;
}
