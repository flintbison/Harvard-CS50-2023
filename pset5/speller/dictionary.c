// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Choose number of buckets in hash table
// Reason for choice explained in hash function comments
const unsigned int N = 186019;

// Hash table
node *table[N];

// Initialise counter for words in dictionary
int dictionary_length = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // run each word in text against the hash function to try and match the words already loaded into the hash table
    // the deterministic quality of a hash function should allow the same inputted word to match the correct location in the hash table
    int word_hash = hash(word);

    // Set cursor to start of linked list for the index (bucket) corresponding to the hash value of the word.
    node *cursor = table[word_hash];

    // While loop to iterate through each item in the linked list for this bucket as long as cursor is not pointing to NULL.
    while (cursor != NULL)
    {
        // Compare provided word to the string in the linked list node for this loop.
        // a return value of 0 indicates a matching pair of strings (case is ignored).
        if (strcasecmp(word, cursor->word) == 0)
        {
            // Return true for check function to indicate the word provided has a match in the dictionary.
            return true;
        }

        // If the word at this node did not result in a match, assign the pointer from the next property of the current node to cursor to progress onto the next node in the list.
        // If the next value of the cursor->next is NULL when assigned, this will be caught by the while loop condition on the next iteration and break the loop.
        cursor = cursor->next;
    }

    // Otherwise return false to show that there was no successful match in the dictionary, indicating it was mispelled or is a word not in the dictionary.
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // take the inputted word and using a mathematical transformation return a deterministic number that is < N
    // this will allow us to assign to an available element in the table array, that is not below or above the number of indices available in the table

    // Version 1.0 - default function
    // return toupper(word[0]) - 'A';


    // Version 3.0 - best performance
    // Determine the length of the string "word".
    int word_length = strlen(word);

    // Initialise a variable to hold the summated value of the word's ASCII characters.
    int word_sum = 0;

    // Check if the word is 1 char in length, if so, returning its ASCII value moduloy N is sufficently unique as anagrams of single letters are not possible.
    if (word_length < 2)
    {
        return toupper(word[0]) % N;
    }
    else
    {
        // Iterate through the chars in the word.
        for (int i = 0; i < word_length; i++)
        {
            // i'th char is normalised to uppercase variant of letter and its ASCII value assigned to temp variable.
            int temp = toupper(word[i]);
            // temp variable is multiplied by 5 to ensure it isnt made negative in following subtraction operation.
            temp *= 5;

            // Check if this is not last char in word and avoid using i+1 as that may cause a segmentation fault. If not, then safe to reference i+1 for subtraction operation.
            if (i != word_length - 1)
            {
                // this char in word is subtracted from next char in word and assigned to temp variable.
                // this is to try and reduce anagram collisions using non-commutative operation of subtraction on neighbooring chars in sequence,
                // the order will be different in anagrams so I am relying on this difference plus subtraction to maximise unqiueness of hash.
                temp -= toupper(word[i + 1]);
            }

            // resulting value of temp is addition-assigned to word_sum.
            word_sum += temp;
        }
        // Once loop is complete return word_sum modulo N;
        return word_sum % N;
    }



    // Hashing algorithm performance benchmarks

    // Staff example to beat when using homer.txt
    // WORDS MISSPELLED:     9339
    // WORDS IN DICTIONARY:  143091
    // WORDS IN TEXT:        192975
    // TIME IN load:         0.03
    // TIME IN check:        0.28
    // TIME IN size:         0.00
    // TIME IN unload:       0.03
    // TIME IN TOTAL:        0.33

    // Version 3.0 summing Summing toupper ASCII values for each char in the supplied word, then modding by N of 186019 (nearest prime number to dictionary word count, reducing clustering through minimal common factors).
    // Added a line to subtract from next char in word to use char order and non-comutative operation to decrease collisions that arose from anagrams evaluating to same hash when using a simple summing approach.
    // Time to complete was further reduced by 0.5 from version 2.0 / total improvement of 6.7 from version 1.0 / only 0.20 slower than staff example
    // WORDS MISSPELLED:     10907
    // WORDS IN DICTIONARY:  143091
    // WORDS IN TEXT:        192975
    // TIME IN load:         0.03
    // TIME IN check:        0.50
    // TIME IN size:         0.00
    // TIME IN unload:       0.01
    // TIME IN TOTAL:        0.55

    // Version 2.0: Summing toupper ASCII values for each char in the supplied word, then modding by N of 186019 (nearest prime number to dictionary word count, reducing clustering through minimal common factors).
    // This approach would not create a unique hash for anagrams (eg: chars in deer and reed would sum to same total value) so distribution in table is not a wide as it could be.
    // Time to complete was reduced by 6.2 from version 1.0
    // Time results
    // WORDS MISSPELLED:     9339
    // WORDS IN DICTIONARY:  143091
    // WORDS IN TEXT:        192975
    // TIME IN load:         0.03
    // TIME IN check:        1.02
    // TIME IN size:         0.00
    // TIME IN unload:       0.01
    // TIME IN TOTAL:        1.06

    // Version 1.0 (default example): (toupper(word[0]) - 'A') with homer.txt and N = 26
    // This results in very long linked lists for each alphabetical pidgeonhole, increasing check time for length n of linked-list at hash address.
    // Time results
    // WORDS MISSPELLED:     9339
    // WORDS IN DICTIONARY:  143091
    // WORDS IN TEXT:        192975
    // TIME IN load:         0.03
    // TIME IN check:        7.20
    // TIME IN size:         0.00
    // TIME IN unload:       0.00
    // TIME IN TOTAL:        7.24

    // References when researching principles for writing good hash functions:

    // Wikipedia article listing the features and theory of hash tables.
    // "Acceptable figures of load factor should range around 0.6 to 0.75" - (Load factor being equal to keys divided by buckets).
    // https://en.wikipedia.org/wiki/Hash_table#Overview

    // Article which explains the mathematical benefit of using a prime number for the hash table length.
    // Since primes are only divisible by themselves and 1, this is the route of least common factors, so should reduce the probability of collisions when hashing a value.
    // https://medium.com/swlh/why-should-the-length-of-your-hash-table-be-a-prime-number-760ec65a75d1

    // To find the closest prime to recommended table length.
    // words in dictionary: 143091, multiplied by 1.3 = 186018.3, then look up the closest prime to that, which is: 186019.
    // https://www.dcode.fr/closest-prime-number
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Open a file pointer to the dictionary provided as an argument
    FILE *dict_file = fopen(dictionary, "r");

    // Check to see if the fopen function returns a NULL indicating failure
    if (dict_file == NULL)
    {
        // Communicate error to user
        printf("Could not open %s.\n", dictionary);

        // Return false
        return false;
    }

    // Declare a char array to act as a word buffer (taking the maximum word size const LENGTH and adding +1 for the sentinel character similar to the node typedef)
    char buffer[LENGTH + 1];

    // Run a while loop which scans the file using fscanf in string mode, this will run until the end of file is found
    while (fscanf(dict_file, "%s", buffer) != EOF)
    {
        // Allocate space for a new word node
        node *n = malloc(sizeof(node));

        // Check to see if malloc function returns a NULL indicating failure
        if (n == NULL)
        {
            // Communicate error to user
            printf("Could not allocate memory for word.\n");

            // Return false
            return false;
        }

        // Copy the word in the buffer to the newly created node
        strcpy(n->word, buffer);

        // Hash the value of the current word, using buffered word
        int word_hash = hash(buffer);

        // Use the returned hash value to relate to a coresponding index in the table and assign the contents to the next pointer for the new node.
        // If the table element is NULL, then that is fine as we can assign NULL to the first node added to that bucket (which as the first node will be the tail of a linked list).
        // If the table element contains a pointer to a pre-existing node, this will point the new node's next property at the existing head of a linked list.
        n->next = table[word_hash];

        // Now we can point the contents of the bucket to the address of the new node.
        // This avoids orphaning any existing linked list nodes as the previous operation preserves a pointer to any existing head in the next property of the new node.
        table[word_hash] = n;

        // Increment word counter variable
        dictionary_length++;
    }

    // Close the open dictionary file now that the loop is completed.
    fclose(dict_file);

    // Return true to indicate that file was loaded successfully.
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // If the counter variable for dictionary items is more than 0, return the contents of that variable.
    if (dictionary_length > 0)
    {
        return dictionary_length;
    }

    // Otherwise dictionary length must be the initialised value of 0, so return 0.
    return 0;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // Run a for loop that iterates through each element in the table (using number of buckets defined in N).
    for (int i = 0; i < N; i++)
    {
        // For each i initialise a pointer called cursor to the head of linked list for the given index (bucket) corresponding to i.
        node *cursor = table[i];

        // Using a while loop, at each bucket search along the linked-list at that element (if it exists)
        while (cursor != NULL)
        {
            // Initialise a temporary pointer to point at the current location that cursor is (to avoid orphaning the linked-list when we free nodes).
            node *temp = cursor;

            // Move the cursor onto the next node by assigning the address held in the next property of the current node.
            cursor = cursor->next;

            // Free the allocated memory for the memory address of the node that temp is still pointing at.
            free(temp);
        }

        // If cursor is NULL, then that indicates that the function has reached the end of the linked-list for this loop.
        // We should also check for when i has reached the completion of the last loop.
        // When both these condtions are true it indicates that every linked-list node in every bucket has been freed.
        if (i == N - 1 && cursor == NULL)
        {
            // Return true to indicate that unloading has completed successfully.
            return true;
        }
    }

    // Return false when the for loop has failed or broken for whatever reason and reaches this line without returning true first.
    return false;
}
