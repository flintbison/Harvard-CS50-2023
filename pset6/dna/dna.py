import csv
import sys


def main():
    # Check for command-line usage. If number of arguments is not equal to 3 (program name, database path, sequence path), then return an error.
    if len(sys.argv) != 3:
        # Print correct usage of program in terminal.
        print(
            "correct usage: python dna.py [path to dna database] [path to dna sequence]"
        )
        # Exit with 1 to indicate error.
        sys.exit(1)
    else:
        # Else if number of arguments is 3 as expected, assign value of arguments to respective variables for database_file and sequence_file.
        database_file = sys.argv[1]
        sequence_file = sys.argv[2]

    # Read database file into a variable.
    # Declare database variable as dictionary.
    database = []
    # Use DictReader to read csv into database dictionary.
    with open(database_file, "r") as database_csv:
        reader = csv.DictReader(database_csv)
        for row in reader:
            database.append(row)

    # Read DNA sequence file into a variable.
    # As sequence is just one long string, use the read() method to read the string into variable sequence.
    with open(sequence_file, "r") as sequence_txt:
        sequence = sequence_txt.read()

    # Find longest match of each STR in DNA sequence.
    # Determine the STR keys in the database using keys() method, then parse them as a list using list() method and remove the "name" key (index 0) by starting the
    # Access from index 1 onwards by using square bracket notation [1:].
    database_str_list = list(database[0].keys())[1:]

    # Declare test_result as dictionary.
    test_result = {}
    # For each STR in STR list extracted from database, run longest_match function against the sequence
    # and assign resulting integer to test_result dictionary under a key matching the STR.
    for str in database_str_list:
        test_result[str] = longest_match(sequence, str)

    # Check database for matching profiles.
    # For each person in the database, compare if persons STR value matches corresponding STR value in test_result for sequence increment match by 1.
    for person in database:
        # Initialise match at 0 for this loop.
        match = 0
        for str in database_str_list:
            if int(person[str]) == test_result[str]:
                match += 1
        # Once loop checking for matches is finished, check if matches accrued is equal to total number of STRs in database. If all STRs are matched,
        # we can then consider the person to be a likely match for the sequence.
        if match == len(database_str_list):
            print(person["name"])
            # Exit with 0 for successful execution and return.
            sys.exit(0)
            return

    # If none of the persons in the database pass the requirement for total match of all STRs then print "no matches found" and end.
    print("No match")

    # Exit with 0 for successful execution and return.
    sys.exit(0)

    return


def longest_match(sequence, subsequence):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):
        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:
            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
