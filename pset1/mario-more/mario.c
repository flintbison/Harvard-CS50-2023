//include libraries
#include <cs50.h>
#include <stdio.h>

//define main funtion
int main(void)
{
    // initialise integer variable to store height of block pyramids
    int height;

    // start do while loop
    do
    {
        // ask user for an integer in a given range and assign it to height variable
        height = get_int("Provide a height between 1 and 8: ");
    }
    //check if provided integer is within the given range
    while (height > 8 || height < 1);

    //begin row loop
    //set counter to start from 1 and end when less than height + 1, increment i by 1 each loop
    for (int i = 1; i < height + 1; i++)
    {
        //begin columns loops

        //first loop set its counter j to the same iteration as the row loop counter i
        //this will loop with the containing loop but end 1 iteration sooner leaving the correct amount for the block to be placed next.
        for (int j = i; j < height; j++)
        {
            printf(" ");
        }

        //this loop begins by subtracting the current row loop from the overall height variable and assigning as the counter k
        //the loop terminates when the counter k is < height. So for the initial loop subtracting 1 from height leaves only 1 loop and 1 created block.
        for (int k = height - i; k < height; k++)
        {
            printf("#");
        }

        //this printf inserts the gap of 2 spaces to separate the block pyramids
        printf("  ");

        //this second for loop repeats the same number blocks on the other side of the 2 space gap, but without the prefixed spaces it becomes symmetrical in layout
        for (int o = height - i; o < height; o++)
        {
            printf("#");
        }

        //the row loop finishes by printing a new line before beginning the next iteration or terminating
        printf("\n");
    }

}