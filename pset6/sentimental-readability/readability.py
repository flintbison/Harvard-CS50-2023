# Define main function.
def main():
    
    # Get text input from user with python function input() and assign to text variable.
    text = input("Text: ")

    # Initialise variable to hold grade integer returned from calc_grade. Each argument in calc_grade function call are each themselves function calls
    # to evaluate number of letters, words and sentences then passing the values to the grade calculation inside the calc_grade function.
    grade = calc_grade(count_letters(text), count_words(text), count_sentences(text))

    # Compare grade to different cases to decide on how to format the print result shown to the user.
    # If grade is below 1, display "Before Grade 1".
    if grade < 1:
        print("Before Grade 1")

    # Else if grade is equal to or more than 16 display "Grade 16+".
    elif grade >= 16:
        print("Grade 16+")

    # Finally, if min and max cases dont apply, then insert the grade integer into the message: "Grade i".
    else:
        print(f"Grade {grade}")


# Count_letters function definition.
# Count letters contained in string - not punctuation, digits, or other symbols.
def count_letters(text):

    # Determine length of string using len.
    text_len = len(text)

    # Initialize letters variable to 0.
    letters = 0

    for char in range(text_len):

        # For each element in string, check if it is an alphabetical character using isalpha and set char_check to 0 or 1 for this loop.
        char_check = text[char].isalpha()

        # If char_check is not false (0) increment the letters variable.
        if char_check > 0:
            letters += 1

    # After looping through all string elements return letters.
    return letters


# count_words function definition.
# Count words contained in string by counting spaces - hyphenated words count as one word - assume min 1 word, no start or end space, no sequential spaces.
def count_words(text):

    # Determine length of string using len.
    text_len = len(text)

    # Initialize the words variable to 1 (spec asks that we assume at least one word in text)
    words = 1

    for char in range(text_len):
        # For each element in string determine if it is a space or not. Assign returned result to char_check variable.
        char_check = text[char].isspace()

        # If char_check is not false (0) increment the words variable.
        if char_check > 0:
            words += 1

    # After looping through all string elements return words.
    return words


# count_sentences function definition.
# Count sentences contained in string by counting . ? and ! on the end of words.
def count_sentences(text):

    # Determine length of string using len.
    text_len = len(text)
    sentences = 0

    for char in range(text_len):
        # For each character in the text string, check if it matches the ASCII numbers for period OR exclaimation mark OR question mark
        # if any match, increment the sentences variable. the python function ord() is used to convert string chars into ASCII code equivalents as integers.
        if ord(text[char]) == 46 or ord(text[char]) == 33 or ord(text[char]) == 63:
            sentences += 1

    # After looping through all string elements return sentences.
    return sentences


# calc_grade function definition.
def calc_grade(letters, words, sentences):

    # Calculate the average letters per 100 words and average sentences per 100 words, passed values are integers, so they have been parsed as floats to avoid truncation and enable arithmetic operation.
    L = 100 * float(letters / words)
    S = 100 * float(sentences / words)

    # Use calculated L and S variables in the Coleman-Liau index formula, rounding the result to the nearest integer.
    index = round(0.0588 * L - 0.296 * S - 15.8)

    # Return index.
    return index


# Call main function.
if __name__ == "__main__":
    main()
