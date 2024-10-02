#include <math.h>
#include "helpers.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            // Take each color channel value at this pixel
            float redChannel = image[row][column].rgbtRed;
            float greenChannel = image[row][column].rgbtGreen;
            float blueChannel = image[row][column].rgbtBlue;

            // Average and round the color channel values
            int averagedRGB = round((redChannel + greenChannel + blueChannel) / 3);

            // set each color channel to the averaged RGB value for this pixel
            image[row][column].rgbtRed = averagedRGB;
            image[row][column].rgbtGreen = averagedRGB;
            image[row][column].rgbtBlue = averagedRGB;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the mid-point column in the image width as we are just transposing the first half over the second and vice-versa
        for (int column = 0; column < width / 2; column++)
        {
            // Horizontailly reflect the pixels for this row

            // Take current column pixel and store in a temp variable
            RGBTRIPLE tempPixel = image[row][column];

            // Assign the value from the reflected position to the current column pixel
            image[row][column] = image[row][width - (column + 1)];

            // Assign the original value from current column pixel to the reflected position pixel
            image[row][width - (column + 1)] = tempPixel;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Make temp duplicate of original image data to reference original values whilst new pixel values are written over original image array.
    RGBTRIPLE tempImage[height][width];

    //Copy contents of original image into tempImage
    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            tempImage[row][column] = image[row][column];
        }
    }

    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            // Blur each pixel

            // Initialise temp variables
            float summedRed = 0;
            float summedGreen = 0;
            float summedBlue = 0;
            int pixelCounter = 0;

            // Search 3x3 space around current pixel and sum up the color values for pixels that exist

            // First search the rows
            for (int searchRow = -1; searchRow < 2; searchRow++)
            {
                // Then search the columns for each row
                for (int searchCol = -1; searchCol < 2; searchCol++)
                {
                    // If this pixel in the search space is outside the upper or lower extent of the image, skip this loop.
                    if (row + searchRow < 0 || row + searchRow >= height)
                    {
                        continue;
                    }

                    // Also if this pixel in the search space is outside the left or right extent of the image, skip this loop.
                    if (column + searchCol < 0 || column + searchCol >= width)
                    {
                        continue;
                    }

                    // If this pixel in the blur radius is within the image space, then add its values to the summed values.
                    summedRed += tempImage[row + searchRow][column + searchCol].rgbtRed;
                    summedGreen += tempImage[row + searchRow][column + searchCol].rgbtGreen;
                    summedBlue += tempImage[row + searchRow][column + searchCol].rgbtBlue;

                    // Increment the pixelCounter for this loop, as this pixel exists
                    pixelCounter++;
                }
            }

            // Once all surrounding pixels have been checked and summed for this pixel, calculate the averaged value for each color channel by dividing the summed value by the number of pixels sampled.
            // Assign the average value over the original pixel value for each color channel.
            image[row][column].rgbtRed = round(summedRed / pixelCounter);
            image[row][column].rgbtGreen = round(summedGreen / pixelCounter);
            image[row][column].rgbtBlue = round(summedBlue / pixelCounter);
        }
    }
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Make temp duplicate of original image data to reference original values whilst new pixel values are written over original image array.
    RGBTRIPLE tempImage[height][width];
    //Copy contents of original image into tempImage
    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            tempImage[row][column] = image[row][column];
        }
    }
    // Initialise Sobel matrices as 2D arrays to reuse in loops below.
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // For each row in the image, index through until the last row in the image height
    for (int row = 0; row < height; row++)
    {
        // For each column in each row, index through until the last column in the image width
        for (int column = 0; column < width; column++)
        {
            // Run convolutional matrix on each pixel in image

            // Initialise temp variables to hold the computed value for each direction and color channel
            float Gx_red = 0;
            float Gx_green = 0;
            float Gx_blue = 0;
            float Gy_red = 0;
            float Gy_green = 0;
            float Gy_blue = 0;

            // Search 3x3 space around current pixel and run a multiplication of the sobel matrices against color channel values for pixels that exist in that space.

            // First search the rows
            for (int searchRow = -1; searchRow < 2; searchRow++)
            {
                // Then search the columns for each row
                for (int searchCol = -1; searchCol < 2; searchCol++)
                {
                    // If this pixel in the search space is outside the upper or lower extent of the image, skip this loop. (add nothing to the Gx channel value)
                    if (row + searchRow < 0 || row + searchRow >= height)
                    {
                        continue;
                    }

                    // Also if this pixel in the search space is outside the left or right extent of the image, skip this loop. (add nothing to the Gx channel value)
                    if (column + searchCol < 0 || column + searchCol >= width)
                    {
                        continue;
                    }

                    // If this pixel is in the edge detection radius is within the image space, then calculate its values. First for horiztonal edge dectection.
                    Gx_red += tempImage[row + searchRow][column + searchCol].rgbtRed * Gx[searchRow + 1][searchCol + 1];
                    Gx_green += tempImage[row + searchRow][column + searchCol].rgbtGreen * Gx[searchRow + 1][searchCol + 1];
                    Gx_blue += tempImage[row + searchRow][column + searchCol].rgbtBlue * Gx[searchRow + 1][searchCol + 1];

                    // Repeat for vertical edge detection
                    Gy_red += tempImage[row + searchRow][column + searchCol].rgbtRed * Gy[searchRow + 1][searchCol + 1];
                    Gy_green += tempImage[row + searchRow][column + searchCol].rgbtGreen * Gy[searchRow + 1][searchCol + 1];
                    Gy_blue += tempImage[row + searchRow][column + searchCol].rgbtBlue * Gy[searchRow + 1][searchCol + 1];
                }
            }

            // Calculate Sobel results for each color channel (rounded to the nearest integer)
            int edgeRed = round(sqrt((Gx_red * Gx_red) + (Gy_red * Gy_red)));
            int edgeGreen = round(sqrt((Gx_green * Gx_green) + (Gy_green * Gy_green)));
            int edgeBlue = round(sqrt((Gx_blue * Gx_blue) + (Gy_blue * Gy_blue)));

            // Check if value exceeds 255 and cap it if it is over 255.
            if (edgeRed > 255)
            {
                edgeRed = 255;
            }
            if (edgeGreen > 255)
            {
                edgeGreen = 255;
            }
            if (edgeBlue > 255)
            {
                edgeBlue = 255;
            }

            // Overwrite original color channel values for this pixel in original image with new computed values.
            image[row][column].rgbtRed = edgeRed;
            image[row][column].rgbtGreen = edgeGreen;
            image[row][column].rgbtBlue = edgeBlue;
        }
    }
}
