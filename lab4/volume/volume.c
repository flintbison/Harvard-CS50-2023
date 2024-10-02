// Modifies the volume of an audio file

// Import libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Number of bytes in .wav header stored in a const
const int HEADER_SIZE = 44;

// Declare an array to hold the header data, setting the size equal to the HEADER_SIZE constant.
uint8_t file_header[HEADER_SIZE];

// Declare a buffer of size 16 bits (2 bytes) to hold each sample as it is read out from the input file, prior to writing it to the output file.
int16_t sample_buffer;

//declaring and initializing a pointer to the sample_buffer variable
int16_t *sample_ptr = &sample_buffer;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 4)
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    // Open a file using the input filename argument and assign to input file pointer using read mode
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Open a file using the output filename argument and assign to output file pointer using write mode
    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Take factor from command line arguments and assign to a float variable
    float factor = atof(argv[3]);

    // Copy header from input file to output file
    fread(&file_header, sizeof(uint8_t), 44, input);
    fwrite(&file_header, sizeof(uint8_t), 44, output);

    // printf("%li\n", ftell(input));

    // Read samples from input file and write updated data to output file
    while (fread(&sample_buffer, sizeof(int16_t), 1, input))
    {
        *sample_ptr = sample_buffer * factor;
        fwrite(&sample_buffer, sizeof(int16_t), 1, output);
        // printf("%li\n", ftell(input));
    }

    // Close files
    fclose(input);
    fclose(output);
}