#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // first, get input from the user by prompting them for thier name
    string name = get_string("What's your name? ");

    // output a formatted string saying hello to the user by using the name provided by them
    printf("hello, %s\n", name);
}