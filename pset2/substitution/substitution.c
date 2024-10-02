#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

//Declare functions as prototypes to define later in code.
int validate_key(string key);
string encipher_text(string key, string plaintext);

// Main function.
int main(int argc, string argv[])
{
    // Declare variables.
    string key;
    string plaintext;

    // accept a command line variable of 26 alphabetical chars as the substitution cipher.
    // check it has exactly 1 command line argument.
    if (argc < 2)
    {
        printf("Usage: ./substitution [key]\n");

        // return 1 to indicate error in program
        return 1;
    }
    else if (argc > 2)
    {
        printf("Only 1 argument of [key] is accepted.\n");

        // return 1 to indicate error in program
        return 1;
    }
    else
    {
        // Assign argv[1] to key variable to make it more semantic in later use.
        key = argv[1];

        // Check if key is valid or not using validate_key function.
        if (validate_key(argv[1]) != 0)
        {
            // if key is valid proceed to request plain text from user.
            plaintext = get_string("Plaintext: ");

            if (strlen(plaintext) < 1)
            {
                // If no plain text provided return error.
                printf("Plaintext was empty!\n");

                // return 1 to indicate error in program.
                return 1;
            }
            else
            {
                // Encipher the plain text and print it out.
                printf("ciphertext: %s\n", encipher_text(key, plaintext));

                // return 1 to indicate program completed correctly.
                return 0;
            }
        }
        else
        {
            // If validation of key fails.
            // return 1 to indicate error in program.
            return 1;
        }
    }
}

// Define validate_key function.
int validate_key(string key)
{

    // Initialize variable keylen with length of key.
    int keylen = strlen(key);

    // check if keylen is not equal to 26
    if (keylen != 26)
    {
        // Tell user that key must be exactly 26 alpha chars long
        printf("[key] should be exactly 26 alphabetical characters in length\n");

        // return 0 to indicate validation result is false
        return 0;
    }
    else
    {
        for (int i = 0; i < keylen; i++)
        {
            // printf("%i\n",i);
            // Iterate through the chars in the key and evaluate if they are alphabetical chars.
            // If any of the chars return a false result of 0, return 0 to indicate an invalid key.
            if (isalpha(key[i]) < 1)
            {

                // Tell user that key contains non-alphabetical characters.
                printf("[key] contains invalid non-alphabetical characters. Only alphabetical characters are accepted.\n");

                // return 0 to indicate validation result is false
                return 0;
            }

            // Initialize a variable to hold count of instances of this letter in key.
            int letterinstances = 0;

            // For current letter in loop, using a subloop compare it with every letter in the key and increment the letterinstance count for each match.
            for (int k = 0; k < keylen; k++)
            {
                if (key[i] == key[k])
                {
                    letterinstances++;
                }
            }

            if (letterinstances > 1)
            {
                // Tell user that key contains duplicate characters.
                printf("[key] contains duplicate characters. Only one instance of each alphabetical character is allowed.\n");

                // return 0 to indicate validation result is false
                return 0;
            }
        }

        // return 1 to indicate validation result is true
        return 1;
    }
}

// Define encipher_text function.
string encipher_text(string key, string plaintext)
{
    // Declare variable for cipher text.
    string ciphertext = plaintext;

    // Initialize a string with initial value of key, this will be swapped char by char until entirely uppercase.
    string upperkey = key;

    // Initialize variable keylen with length of key.
    int keylen = strlen(key);

    // Initialize variable plainlen with length of plaintext.
    int plainlen = strlen(plaintext);

    // Loop through the key transforming each character to uppercase and adding to the corresponding element in upperkey string.
    for (int i = 0; i < keylen; i++)
    {
        upperkey[i] = toupper(key[i]);
    }

    // Loop through the plaintext and identify alphabetical chars to encipher.
    for (int i = 0; i < plainlen; i++)
    {
        // Initialize a variable to store the identified case of this char (if any), setting default value to true.
        bool uppercased = true;

        // Initialize a char variable with the current index, which may remain the same or be substituted with an enciphered char depending on it being alphabetical or not.
        char cipherchar = plaintext[i];

        // Determine if char is alphabetical or not and encipher if true.
        if (isalpha(cipherchar))
        {
            // Check if the char is lowercase, if it is, then set the uppercased variable to false. (This will persist what the original char case was during enciphering transformations).
            if (islower(cipherchar))
            {
                uppercased = false;
            }

            // Convert the char to its uppercase equivalent so all chars in the plaintext can be normalised to uppercase for finding corresponding ascii codes.
            cipherchar = toupper(cipherchar);

            // Subtract 65 from the char value to produce a 0 indexed value relative to uppercase ascii values A-Z
            int alphaindex = cipherchar - 65;

            // Since the key is indexed to 26 elements, we can use the reletive letter position of each char in the plaintext to index the key array and return its substitution char.
            // This subsutition char is then assigned over the original plaintext char in the cipherchar string.
            cipherchar = upperkey[alphaindex];

            // Because the original case was persisted in the uppercased variable we can check if that variable is now set to true or false.
            // If the char is NOT uppercased then we will use the tolower function to change the enciphered char to lowercase and assign it to the variable.
            if (uppercased != true)
            {
                cipherchar = tolower(cipherchar);
            }

        }

        // Append cipherchar after alphabetical checks and transforms have been performed, to the ciphertext string at the proper index.
        ciphertext[i] = cipherchar;
    }

    // Return cipher text.
    return ciphertext;
}

