#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
bool is_cycle(int start, int end);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Assign number of candidates to candidate_count variable (subtracting 1 which is the program name at the command line).
    candidate_count = argc - 1;

    // Take that updated candidate_count and see if it exceeds the MAX number of candidates specified in the MAX constant.
    // If it does not pass this check, terminate main with a return value error code of 2.
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }

    // Take the candidate_count and iterate by that number, assigning the name of each candidate into the candidates array.
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Set graph of locked in pairs to a default value of false for all.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    // Set pair_count to default value of zero.
    pair_count = 0;

    // Get int of number of voters from user.
    int voter_count = get_int("Number of voters: ");

    // Query for votes, iterate for each voter.
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank which candidate the voter prefers.
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        // On completion of ranking candidates for that voter, pass ranks to record_preferences function.
        record_preferences(ranks);

        // Add a return line to terminal.
        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote, pass in the current rank index for current rank, candidate name and ranks[] array for that voter.
bool vote(int rank, string name, int ranks[])
{
    // Loop through each candidate.
    for (int i = 0; i < candidate_count; i++)
    {
        // and check if supplied name exists in array of candidates by running string comparision at each candidate.
        if (strcmp(candidates[i], name) == 0)
        {
            // If candidate exists and matches supplied name then add rank to ranks array and return true.
            // the rank index integer is passed into this function from the ith loop in the main function, whilst the value stored at that position in
            // ranks array is the candidate index integer which the conditional statement matched the name at.
            ranks[rank] = i;
            return true;
        }
    }
    // If supplied candidate name does not match any in array, return false
    return false;
}

// Update preferences given current voters ranks. Pass in the ranks array.
void record_preferences(int ranks[])
{
    // Loop through number of candidates.
    for (int i = 0; i < candidate_count; i++)
    {
        // Loop through candidates again but initialise j as i + 1.
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Using i and j, select the appropriate element in preferences array and ++ to increment the integer at that position.
            // This works because it progressively selects the ith and ith+1 element in the ranks array, accessing its value, which
            // in turn is the candidate index integer which the preferences array indexes are analogous to.
            // Using these candidate indices works because in ranks the preference shown as i over i+1 an so on, wheras in preferences it is i over j.
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    // Once done, return void.
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Loop for the same number as there are candidates (this is per row in the 2D preferences array)
    for (int i = 0; i < candidate_count; i++)
    {
        // Loop through columns of the row, but skip the cells in which the candidate is compared to themselves, aswell as previously checked cells.
        // This is done by taking the current value of i and adding 1 to it and assigning that as the initial value of j.
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Check which of the two candidates have the higher number of preferences.
            // If preference integer at [i][j] is more than than preference integer at its opposite, proceed.
            if (preferences[i][j] > preferences[j][i])
            {
                // pair_count int is initially set at 0, this is used as an index for placement in the pairs array. In the first instance it would be pairs[0]
                // assign the correct indexes for candidates in candidates array winner and loser variables in the pair struct.
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                // Once the details are added to that element of the pairs array, increment the pair_count, so a new index may be used for a future loop.
                pair_count++;
            }
            // If previous condition was false, check if the same pair is less than instead. If true change the values in the corresponding manner as above.
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
            // If neither are more than or less than eachother, it must mean they are tied, so we do not write that pair into the array.
        }
    }
    // Once done, return void.
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // For loop which decrements i from n-1, where n = pair_count, until i reaches 0
    // (pair_count is a count not an index so the -1 makes it into an integer which can be used as an index to traverse pairs array).
    for (int i = pair_count - 1; i >= 0 ; i--)
    {
        // For each i'th iteration in parent loop perform a for loop which increments j from 0 till it reaches a number which is equal to or less than i - 1
        // (The -1 in this loop is to accomodate for the fact the j'th element in pairs is being compared to the j'th+1 element, incrementally along the pairs array
        // since the final element in that array wont have a corresponding j'th+1 element to compare against, we stop 1 short of the i'th iteration in the sort)
        for (int j = 0; j <= i - 1; j++)
        {
            // Compare j'th and j'th+1 elements in pair by using thier contents to access the corresponding preference amount in preferences array.
            // If j'th element is less than j'th+1 element swap positions by making use of a temporary pair variable to hold the values of one element whilst assigning.
            // If element is not lower than the +1 element do nothing, it is already in the descending order we want.
            if ((preferences[pairs[j].winner][pairs[j].loser]) < (preferences[pairs[j + 1].winner][pairs[j + 1].loser]))
            {
                pair temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Loop through all pairs in pairs array using pair_count to know when to stop on last element.
    for (int i = 0; i < pair_count; i++)
    {
        // Run is_cycle function, to check if arrow pointing from winner to loser for i'th pair would create a loop amongst existing locked-in pairs,
        // if it returns false, lock the i'th pair into the locked array as true.
        // if it returns true, do nothing, don't add i'th pair to locked, leaving it at its inital value of false.
        if (!is_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}
bool is_cycle(int start, int end)
{
    // (Recursion base case)
    // Return true if at any point the is_cycle function matches the end candidate back to the start candidate, proving a cycle exists.
    // Since only 1 pair cannot form a cycle, we can assume this will always be false for the first pass.
    if (start == end)
    {
        return true;
    }

    // (Recursive case)
    // Loop through each candidate in candidate arrays using candidate_count to know how many elements to iterate over.
    for (int i = 0; i < candidate_count; i++)
    {
        // If end candidate being checked is already the winner in a pair in the locked array, then proceed to check it against others in the array.
        if (locked[end][i])
        {
            // For any pairs where end candidate is winner check where the loser (new end) matches the start candidate by using a recursive call.
            if (is_cycle(start, i))
            {
                // When recursive call returns true for conditional statement above then return true for this call.
                return true;
            }
        }
    }
    // If all checks complete without returning true, return false for no cycle found.
    return false;
}

// Print the winner of the election.
void print_winner(void)
{
    // Loop through each candidate in candidate arrays using candidate_count to know how many elements to iterate over.
    for (int i = 0; i < candidate_count; i++)
    {
        // Initalize a variable to keep count of how many times candidate i has not incurred a loss in the locked array.
        int no_loss = 0;

        // Nested loop, for each candidate i loop through each candidate j.
        for (int j = 0; j < candidate_count; j++)
        {
            // If candidate i did not lose to candidate j in locked array (element at that position is false, aka, no arrow pointing at candidate i), then proceed.
            if (locked[j][i] == false)
            {
                // Increment false_count to tally when candidate j can not lost to candidate i.
                no_loss++;

                // If false_count is equal to number of candidates, then all possible candidates have no arrows pointing at candidate j, who must be the source, therefore the winner.
                if (no_loss == candidate_count)
                {
                    // Print candidate i as the winner.
                    printf("%s\n", candidates[i]);
                }
            }
        }
    }
    return;
}
