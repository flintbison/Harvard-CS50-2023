#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef uint8_t BYTE;

// Define the number of bytes in each block being scanned.
const int BLOCK_SIZE = 512;

int main(int argc, char *argv[])
{
    // Initialise file pointer for input file as NULL until defined in program.
    FILE *forensic_file = NULL;

    // Check number of command line arguments, if no forensic image is provided prompt user with correct usage.
    if (argc != 2)
    {
        // When no file is provided give user feedback and return with code 1;
        printf("Usage: ./recover [forensic_file]\n");
        return 1;
    }
    else
    {
        // Get the filename as a string
        char *input_filename = argv[1];
        // Open a file pointer using the argument passed (which should be a file in the same directory).
        forensic_file = fopen(input_filename, "r");

        // Check if file pointer was opened successfully.
        if (forensic_file == NULL)
        {
            // If file failed to open, give user feedback and return with code 1;
            printf("Could not open %s\n", input_filename);
            return 2;
        }
    }

    // Initialise variables to manage the image extraction.
    BYTE buffer[BLOCK_SIZE]; // this buffer holds enough bytes to store each memory block as it is being handled.
    int img_number = 0;      // this keeps track of the number of jpgs found, for use in sequential filename.
    FILE *output_img = NULL; // file pointer is NULL until defined in program.
    char filename[8];        // 8 chars allows for 3 sequential digits, a period, jpg and the sentinel char /0 (###.jpg/0).

    // Run a while loop which runs until fread returns the BLOCK_SIZE not matching 512. Indicating that the end of file has been reached.
    while (fread(&buffer, 1, BLOCK_SIZE, forensic_file) == BLOCK_SIZE)
    {
        // Check if this block matches the hex values for a jpg header.
        if (
            buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // Having confirmed a jpg header, check if this is the first jpg detected in forensic_file
            if (img_number != 0)
            {
                // File is not first found, so close the previous file before proceeding.
                fclose(output_img);
            }

            // Begin a new file

            // Generate filename
            sprintf(filename, "%03i.jpg", img_number);
            // Open a file pointer in write mode with generated filename
            output_img = fopen(filename, "w");
            // Increment the img_number
            img_number++;
        }
        // If block is not a jpg header continue writing data from block into file pointer
        // Check if img_number is incremented above 0, indicating furst file header has been detected and number incremented.
        if (img_number != 0)
        {
            // Write this block from buffer into output_img file pointer.
            fwrite(&buffer, 1, BLOCK_SIZE, output_img);
        }
    }

    // Now that the while loop has terminated and the file end reached, close the file pointers for the input and latest output files.
    fclose(forensic_file);
    fclose(output_img);

    // End main function with successful return
    return 0;
}