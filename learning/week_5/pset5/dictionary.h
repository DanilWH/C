/**
 * dictionary.h
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Declares a dictionary's functionality.
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45
#define SUMBNUMB 28 // 27 for letters and one more for a apostrophe.
/**
 * A structure of the prefix tree.
 */
typedef struct node
{
    bool is_end;
    struct node* children[SUMBNUMB];
}
node;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word);

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary);

/**
 * Adds a new node to the prefix tree.
 */
void* add_node(void);

/**
 * Inserts a key to the prefix tree. 
 */
bool insert(node* ptr, const char* key);

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void);

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void);

/**
 * Removes one node from memory.
 */
void remove_node(node* ptr);

#endif // DICTIONARY_H
