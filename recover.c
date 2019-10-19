#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
//ensure proper //usagensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    //remember file names
    char *infile = argv [1];

    //open the file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s\n", infile);
        return 1;
    }

    //declare the data type
    unsigned char buffer [512];
    char filename [8];
    int jpg_number = 0 ;
    FILE *img;

    //read the file
    while (fread(&buffer, 512, 1, inptr))
    {
        //detect JPEG Signature
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)

        {
            if (jpg_number > 0)
            {
                fclose(img);
            }

            //making a new JPEG
            sprintf(filename, "%03i.jpg", jpg_number);

            //open output file
            img = fopen(filename, "w");
            if (img  == NULL)
            {
                fclose(inptr);
                fprintf(stderr, "Could not create %s.\n", filename);
                return 3;
            }
            jpg_number++;
        }


        fwrite(&buffer, 512, 1, img);


    }


    // close infile
    fclose(inptr);

    // close outfile
    fclose(img);

    // success
    return 0;

}
