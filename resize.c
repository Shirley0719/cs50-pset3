// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    int  times  = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    //ensure n is a positive number less or equal than 100
    if (times <= 0 || times > 100)
    {
        fprintf(stderr, "Please provide a positive number less or equal than 100");
        return 1;
    }


    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //declare the new head file
    BITMAPFILEHEADER new_bf = bf;
    BITMAPINFOHEADER new_bi = bi;

    // determine infile padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;


    //update the new_width & height
    new_bi.biWidth = bi.biWidth * times;
    new_bi.biHeight = bi.biHeight * times;

    //determine padding for outfile
    int new_padding = (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    new_bi.biSizeImage = (new_bi.biWidth * sizeof(RGBTRIPLE) + new_padding) * abs(new_bi.biHeight);
    new_bf.bfSize = new_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // resize vertically
        for (int l = 0; l < times; l++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage

                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                //resize horizontally
                for (int m = 0; m < times; m++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);


            // then add it back (to demonstrate how)
            for (int k = 0; k < new_padding; k++)
            {
                fputc(0x00, outptr);
            }

            // put the pointer back to the begining of the row
            if (l < times - 1)
            {
                long row_length = (bi.biWidth * sizeof(RGBTRIPLE)) + padding;
                fseek(inptr, -row_length, SEEK_CUR);
            }
        }


    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

