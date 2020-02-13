/*
 * Changes the size of a photo.
 */


#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[]) {
    // check the number of arguments.
    if (argc != 4) {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // getting the data of the command line arguments.
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];

    // open infile.
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL) {
        printf("Could not open the %s file.\n", infile);
        return 2;
    }

    // create outfile.
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL) {
        fclose(inptr);
        fprintf(stderr, "Could not create the %s file.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER.
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER.
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // is the file format wrong?
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biBitCount != 24 ||
        bi.biSize != 40 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        fprintf(stderr, "Unsupported file format!\n");
        return 4;
    }

    // calculate the infile's padding for scanlines.
    int infile_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // change the important fields.
    bf.bfSize = bi.biSizeImage * n + bf.bfOffBits;
    bi.biWidth *= n;
    bi.biHeight *= n;
    bi.biSizeImage *= n;

    // write outfile's BITMAPFILEHEADER.
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER.
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // calculate the outfile's padding for scanlines.
    int outfile_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    printf("%i, %i\n", infile_padding, outfile_padding);

    // iterate over infile's scanlines.
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++) {
        RGBTRIPLE triples[bi.biWidth];
        fread(triples, sizeof(RGBTRIPLE), bi.biWidth / n, inptr);

        // draw the scanline as much as it is neccesary.
        for (int r = 0; r < n; r++) {
            // interate over pixels in a scanline.
            for (int j = 0; j < bi.biWidth / n; j++) {
                
                // draw the pixsel as much as it is neccesary.
                for (int c = 0; c < n; c++) {
                    // write RGB triple to outfile.
                    fwrite(&triples[j], sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // then add it back (to demonstrate how).
            for(int k = 0; k < outfile_padding; k++) {
                fputc(0x00, outptr);
            }
        }
        // skip over padding, if any.
        fseek(inptr, infile_padding, SEEK_CUR);
    }

    // close the infile.
    fclose(inptr);

    // close the outfile.
    fclose(outptr);

    // success.
    return 0;
}
