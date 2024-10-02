#include <cs50.h>
#include <stdio.h>

int main(void)
{
    // Initialise the variable to hold population size.
    int populationSize = 0;
    // Initialise the variable to hold end population size.
    int endSize = 0;
    // Initialise the variable to count years required to reach end size.
    int years = 0;

    // Using a do while loop, get a start size for the population. The miniumum required is 9, so if the provided value is less than 9 the loop will prompt the user until it is at least 9.
    do
    {
        populationSize = get_int("Start size (9 or more): ");
    }
    while (populationSize < 9);

    // Using a do while loop, get an end size for the population. The miniumum required is equal to the starting size, so if the provided value is less than that, then the loop will prompt the user until it is at least equal.
    do
    {
        endSize = get_int("End size (equal to or more than start size): ");
    }
    while (endSize < populationSize);

    // Using a while loop, perform the loop while populationSize is less than endSize
    while (populationSize < endSize)
    {
        // Perform the algorithm of population growth for each loop
        populationSize = populationSize + (populationSize / 3) - (populationSize / 4);

        // Each loop, increment the years taken to reach the endSize
        years++;
    }

    // Print number of years taken to reach endSize
    printf("Years: %i\n", years);

    // Return 0 for successful completion of main function.
    return 0;
}
