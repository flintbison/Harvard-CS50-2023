#include <cs50.h>
#include <stdio.h>

int main(void)
{
    //get credit card number by prompting user - store as a long datatype
    long cardnum = get_long("Card number: ");

    //define an integer variable to count loop iterations for upcoming loop
    int digitcount = 0;

    //define a long variable for handling progressive division of provided card number to count digits
    long carddigits = cardnum;

    //perform a while loop which will loop as long as the carddigits variable is more than 0
    while (carddigits > 0)
    {
        //each loop will divide the card number by 10 (to reduce base10 number by one digit position) and iterate the digitcount by one to keep count of digits between loops
        carddigits = carddigits / 10;
        //once the result of the division is assigned by to the variable, increment the digitcount and loop
        digitcount++;
    }

    //check to see if digitcount is not one of the valid card number lengths (visa = 13, amex = 15, mastercard = 16)
    if (digitcount != 13 && digitcount != 15 && digitcount != 16)
    {
        //if check fails print INVALID
        printf("INVALID\n");
        return 0;
    }

    //declare variables required to calculate checksum
    long carddigitschecksum = cardnum;
    int checksumtotal = 0;
    int a = 0;
    int b = 0;
    int mod_a = 0;
    int mod_b = 0;
    int b_a = 0;
    int b_b = 0;

    //calculate the checksum by starting a do while loop
    do
    {
        //use modulo 10 to get the last digit from the cardnumber assign to mod_a
        mod_a = carddigitschecksum % 10;

        //add the modulus result to the a variable, as this loops this will accumulate the sum of the alternating last numbers
        a = a + mod_a;

        //divide the card number by 10 to knock off the last number and assign back to carddigitchecksum, the last number is now the b set
        carddigitschecksum = carddigitschecksum / 10;

        //use modulo 10 to get the last digit from the cardnumber assign to mod_b
        mod_b = carddigitschecksum % 10;

        //divide the card number by 10 again to knock off the last number and assign back to carddigitchecksum, the last number is now the a set again
        carddigitschecksum = carddigitschecksum / 10;

        //multiply mod_b digit by 2 and assign back to mod_b
        mod_b = mod_b * 2;

        //to add the digits of the product of multiplying mod_b by 2, we can use the modulo 10 and divide by 10 methods to derive the 2 digits of the product
        //the maximum product possuble is 18 (9 * 2) so we can safely assume we are working with 2 digits
        b_a = mod_b % 10;
        b_b = mod_b / 10;

        //add the digits of the product of mod_b * 2 to the cumulative value of b and assign back to b
        b = b + b_a + b_b;
    }
    while (carddigitschecksum > 0);

    //sum the total of the a and b calculations
    checksumtotal = a + b;

    //check if card number does not pass Luhn Algorithm
    if (checksumtotal % 10 != 0)
    {
        //if check fails print INVALID
        printf("INVALID\n");
        return 0;
    }

    //get first two digits of card number so we can identify the card type
    long firsttwocarddigits = cardnum;
    do
    {
        //using the divide by 10 method, remove the last digit of the card number for every loop
        firsttwocarddigits = firsttwocarddigits / 10;
    }
    while (firsttwocarddigits > 100); //loop continues to remove digits until carddigits fall below 3 digits to 2 digits

    printf("%ld\n", firsttwocarddigits);


    //run checks on firsttwocarddigits and digitcount to identify card type
    //to be Mastercard card number must match a length of 16 digits and its first two numbers must be 51,52,53,54 or 55
    if (firsttwocarddigits > 50 && firsttwocarddigits < 56 && digitcount == 16)
    {
        printf("MASTERCARD\n");
    }
    //to be American Express card number must match a length of 15 and its first two digits must be either 34 or 37
    else if ((firsttwocarddigits == 34 || firsttwocarddigits == 37) && digitcount == 15)
    {
        printf("AMEX\n");
    }
    //to be Visa card number must match a length of 16 and its first digit must be 4
    else if ((firsttwocarddigits / 10 == 4) && (digitcount == 13 || digitcount == 16))
    {
        printf("VISA\n");
    }
    //if there is somehow an exception and the card number matches none of the criteria above, print invalid
    else
    {
        printf("INVALID\n");
        return 0;
    }

}