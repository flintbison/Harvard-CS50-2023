#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

// Update vote totals given a new vote
bool vote(string name)
{
    // Initialise a variable for keeping track of whether the given name was found in the array of candidates,
    // Initial value is false, which will be changed to true if a match is found.
    bool candidate_found = false;

    // search if name is in candidate array, if not return false
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            // If name is found in candidate array:
            // increment the votes variable,
            // set the candidate_found variable to true,
            // and break loop since we have found what we are looking for.
            candidates[i].votes++;
            candidate_found = true;
            break;
        }
    }

    // Refer to candidate_found variable to see if candidate was found,
    // If candidate was found return true for function, else, return false.
    if (candidate_found == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    // Function has a void return type because it only produces side effects via printf()

    // Decare a variable of some kind to keep track of which votes was highest
    int highest_votes = 0;

    // iterate through candidates checking if the votes exceed the current highest votes
    // if they do, update the value of the highest_votes, this will in effect find the highest score if all candidates
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > highest_votes)
        {
            highest_votes = candidates[i].votes;
        }
    }

    // now that the highest score has been identified, iterate through all the candidates again and compare
    // the votes that candidate received to the highest_votes
    // for each candidate with votes matching the highest_votes, print thier name.
    // if there is a tie, it must essentially be two or more candidates with equal votes to eachother and equal to highest votes among the set of candidates.
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == highest_votes)
        {
            printf("%s\n", candidates[i].name);
        }
    }

    return;
}