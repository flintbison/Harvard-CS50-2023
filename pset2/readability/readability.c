#include <cs50.h>
#include <stdio.h>
//Include ctype to work with checking if characters are space, punctuation or alphabetical.
#include <ctype.h>
//Include string to use the strlen function to count the length of elements in a string.
#include <string.h>
//Include math to get access to the round function, so the index formula can be rounded to an integer.
#include <math.h>

//Declare functions as prototypes to define later in code.
int count_letters(string text);
int count_words(string text);
int count_sentences(string text);
int calc_grade(int letters, int words, int sentences);

int main(void)
{

    //Get text input from user with get_string.
    string text = get_string("Text: ");

    //Initialise variable to hold grade integer returned from calc_grade.
    int grade = calc_grade(count_letters(text), count_words(text), count_sentences(text));

    //Compare grade to different cases to decide on how to format the printf result shown to the user.
    //If grade is below 1, display "Before Grade 1".
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    //Else if grade is equal to or more than 16 display "Grade 16+".
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    //Finally, if min and max cases dont apply, then insert the grade integer into the message: "Grade i".
    else
    {
        printf("Grade %i\n", grade);
    }

}

//count_letters function definition.
//Count letters contained in string - not punctuation, digits, or other symbols.
int count_letters(string text)
{
    //Determine length of string using strlen.
    int text_len = strlen(text);
    //Initialize letters variable to 0.
    int letters = 0;


    for (int i = 0; i < text_len; i++)
    {
        //For each element in string, check if it is an alphabetical character using isalpha and set char_check to 0 or 1 for this loop.
        int char_check = isalpha(text[i]);

        //If char_check is not false (0) increment the letters variable.
        if (char_check > 0)
        {
            letters ++;
        }

    }
    //After looping through all string elements return letters.
    return letters;
}

//count_words function definition.
//Count words contained in string by counting spaces - hyphenated words count as one word - assume min 1 word, no start or end space, no sequential spaces.
int count_words(string text)
{
    //Determine length of string using strlen.
    int text_len = strlen(text);
    //Initialize the words variable to 1 (spec asks that we assume at least one word in text)
    int words = 1;

    for (int i = 0; i < text_len; i++)
    {
        //For each element in string determine if it is a space or not. Assign returned result to char_check variable.
        int char_check = isspace(text[i]);

        //If char_check is not false (0) increment the words variable.
        if (char_check > 0)
        {
            words ++;
        }

    }
    //After looping through all string elements return words.
    return words;
}

//count_sentences function definition.
//Count sentences contained in string by counting . ? and ! on the end of words.
int count_sentences(string text)
{
    //Determine length of string using strlen.
    int text_len = strlen(text);
    int sentences = 0;

    for (int i = 0; i < text_len; i++)
    {
        //For each character in the text string, check if it matches the ASCII numbers for period OR exclaimation mark OR question mark
        //if any match, increment the sentences variable.
        if (text[i] == 46 || text[i] ==  33 || text[i] == (char) 63)
        {
            sentences ++;
        }
    }
    //After looping through all string elements return sentences.
    return sentences;
}

//calc_grade function definition.
int calc_grade(int letters, int words, int sentences)
{
    //Calculate the average letters per 100 words and average sentences per 100 words, passed values are integers, so they have been type cast as floats to avoid truncation.
    float L = 100 * ((float)letters / (float)words);
    float S = 100 * ((float)sentences / (float)words);

    //Use calculated L and S variables in the Coleman-Liau index formula, rounding the result to the nearest integer.
    int index = round(0.0588 * L - 0.296 * S - 15.8);

    //Return index.
    return index;
}