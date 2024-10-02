# Import cs50 module to access the get_int function
import cs50


# Define main function
def main():

    # Get card number and store as variable "cardnum".
    cardnum = cs50.get_int("Card number: ")

    # Define an variable "digitcount" to count loop iterations for upcoming loop.
    digitcount = int(0)

    # Define a variable for handling progressive division of provided card number to count digits.
    carddigits = int(cardnum)

    # Perform a while loop which will loop as long as the carddigits variable is more than 0.
    while carddigits > 0:
        # Each loop will divide the card number by 10 (to reduce base10 number by one digit position) and iterate the digitcount by one to keep count of digits between loops.
        carddigits = int(carddigits / 10)
        # Once the result of the division is assigned by to the variable, increment the digitcount and loop.
        digitcount += 1

    # Print out value of digitcount
    print(digitcount)

    # Check to see if digitcount is not one of the valid card number lengths (visa = 13, amex = 15, mastercard = 16).
    if int(digitcount) != 13 and int(digitcount) != 15 and int(digitcount) != 16:
        # If check fails print INVALID
        print("INVALID")
        # Return 1 as exit code for error.
        return 1

    # Declare variables required to calculate checksum.
    carddigitschecksum = int(cardnum)
    checksumtotal = int(0)
    a = int(0)
    b = int(0)
    mod_a = int(0)
    mod_b = int(0)
    b_a = int(0)
    b_b = int(0)

    # Calculate the checksum by starting a while loop.
    while carddigitschecksum > 0:

        # Use modulo 10 to get the last digit from the cardnumber assign to mod_a.
        mod_a = int(carddigitschecksum % 10)

        # Add the modulus result to the a variable, as this loops this will accumulate the sum of the alternating last numbers.
        a = int(a + mod_a)

        # Divide the card number by 10 to knock off the last number and assign back to carddigitchecksum, the last number is now the b set.
        carddigitschecksum = int(carddigitschecksum / 10)

        # Use modulo 10 to get the last digit from the cardnumber assign to mod_b
        mod_b = int(carddigitschecksum % 10)

        # Divide the card number by 10 again to knock off the last number and assign back to carddigitchecksum, the last number is now the a set again.
        carddigitschecksum = int(carddigitschecksum / 10)

        # Multiply mod_b digit by 2 and assign back to mod_b.
        mod_b = int(mod_b * 2)

        # To add the digits of the product of multiplying mod_b by 2, we can use the modulo 10 and divide by 10 methods to derive the 2 digits of the product.
        # The maximum product possuble is 18 (9 * 2) so we can safely assume we are working with 2 digits.
        b_a = int(mod_b % 10)
        b_b = int(mod_b / 10)

        # Add the digits of the product of mod_b * 2 to the cumulative value of b and assign back to b.
        b = int(b + b_a + b_b)

    # Sum the total of the a and b calculations
    checksumtotal = int(a + b)

    # Check if card number does not pass Luhn Algorithm.
    if int(checksumtotal % 10) != 0:
        # If check fails print INVALID.
        print("INVALID")
        # Return 1 as exit code for error.
        return 1

    # Get first two digits of card number so we can identify the card type.
    firsttwocarddigits = int(cardnum)

    # Create a string version of firsttwocarddigits to enable substring access in conditional statements below.
    firsttwocarddigitsstr = str(firsttwocarddigits)

    # Loop continues to remove digits until carddigits fall below 3 digits to 2 digits.
    while int(firsttwocarddigits) > 100:

        # Using the divide by 10 method, remove the last digit of the card number for every loop.
        firsttwocarddigits = int(firsttwocarddigits / 10)

    print(f"{firsttwocarddigits}")

    # Run checks on firsttwocarddigits and digitcount to identify card type.
    # To be Mastercard card number must match a length of 16 digits and its first two numbers must be 51,52,53,54 or 55.
    if int(firsttwocarddigits) > 50 and int(firsttwocarddigits) < 56 and int(digitcount) == 16:
        print("MASTERCARD")
        # Return 0 as exit code for success.
        return 0

    # To be American Express card number must match a length of 15 and its first two digits must be either 34 or 37.
    elif (int(firsttwocarddigits) == 34 or int(firsttwocarddigits == 37)) and int(digitcount) == 15:
        print("AMEX")
        # Return 0 as exit code for success.
        return 0

    # To be Visa card number must match a length of 16 or 13 and its first digit must be 4.
    elif (firsttwocarddigitsstr[:1] == "4") and (int(digitcount) == 13 or int(digitcount) == 16):
        print("VISA")
        # Return 0 as exit code for success.
        return 0

    # If there is somehow an exception and the card number matches none of the criteria above, print invalid.
    else:
        print("INVALID")
        # Return 1 as exit code for error.
        return 1


if __name__ == "__main__":
    main()