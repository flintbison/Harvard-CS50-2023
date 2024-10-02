#include "helpers.h"

void colorize(int height, int width, RGBTRIPLE image[height][width])
{
    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            // Check if the pixel at the row x width location matches RGB value for black (0,0,0)
            if (
                image[row][column].rgbtRed == 0 &&
                image[row][column].rgbtGreen == 0 &&
                image[row][column].rgbtBlue == 0)
            {
                // if pixel at this location is black, change the RGBTRIPLE value to another color (in this case, pure red 255,0,0)
                image[row][column].rgbtRed = 255;
                image[row][column].rgbtGreen = 0;
                image[row][column].rgbtBlue = 0;
            }
        }
    }
}