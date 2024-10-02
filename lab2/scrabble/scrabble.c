#include <ctype.h>
#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Points assigned to each letter of the alphabet
int points[] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};

// Declare compute_score prototype function
int compute_score(string word);

int main(void)
{
    // Get input words from both players
    string word1 = get_string("Player 1: ");
    string word2 = get_string("Player 2: ");

    // Score both words
    int score1 = compute_score(word1);
    int score2 = compute_score(word2);

    // Compare player scores and print the winner
    if (score1 > score2)
    {
        // If player 1 has the higher score declare them the winner
        printf("Player 1 wins!\n");
    }
    else if (score1 < score2)
    {
        // If player 2 has the higher score declare them the winner
        printf("Player 2 wins!\n");
    }
    else
    {
        // If both players have an equal score, the game is a tie
        printf("Tie!\n");
    }
}

// Define compute_score function
int compute_score(string word)
{
    // Compute and return score for string

    // Determine the length of the string provided and store as variable
    int wordlength = strlen(word);

    // Initialize an int variable to hold cumulative score for the string
    int wordscore = 0;

    // Loop through the string provided
    for (int i = 0; i < wordlength; i++)
    {
        // Check if char is alphabetical - if it is proceed to scoring it, if not skip this char
        if (isalpha(word[i]))
        {
            // For each loop take the char for that iteration and convert to uppercase
            char convertedletter = toupper(word[i]);

            // Subtract 65 from the char value to produce a 0 indexed value relative to uppercase ascii values A-Z
            int letterindex = convertedletter - 65;

            // Use resulting value as the index to access the related letter score from the points array
            int letterscore = points[letterindex];

            // Add onto the cumulative score for the string
            wordscore += letterscore;
        }
    }

    // When loop has finished return cumulative score as int
    return wordscore;

}