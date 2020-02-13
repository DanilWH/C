/**
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

node* head; // the pointer to the prefix tree.
int words = 0; // the variable that sotores the amount of words in a dict.

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // take a copy of the trie.
    node* ptr = head;
    char currchar;
    int charpos;
    // read each character and find it in the trie.
    for (int i = 0; word[i] != '\0'; i++) {
        // calculate the poition of a character in a node trie.
        currchar = tolower(word[i]) - 'a';
        charpos = (currchar >= 0)? currchar : SUMBNUMB - 1;
        // move to next node if the current character is in the array.
        if (ptr->children[charpos] != NULL) {
            ptr = ptr->children[charpos];
        }
        // return false if not.
        else return false;
    }
    // check the end of a word and return it.
    return ptr->is_end;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // open the given dictionary.
    FILE* infile = fopen(dictionary, "r");
    if (infile == NULL) return false;

    // create the head of prefix tree.
    head = add_node(); // the pointer to the prefix tree.
    if (head == NULL) return false;

    // buffer to store one word.
    char wordbuff[LENGTH + 1]; // (+ 1 for the '\0' symbol).
    // store each word from the given dictionary to the buffer.
    while(fscanf(infile, "%s", wordbuff) != EOF) {
        // store a word into the prefix tree.
        if (!insert(head, wordbuff)) {
            // if something goes wrond then unload,
            unload();
            // zeroize the counter,
            words = 0;
            // fail.
            return false;
        }
        // update counter.
        words++;
    }
    // close the dictionary file.
    fclose(infile);

    //success.
    return true;
}

/**
 * Adds a new node to the prefix tree.
 */
void* add_node(void)
{
    // allocate the block of memory for one node.
    node* newnodeptr = (node*) malloc(sizeof(node));
    if (newnodeptr == NULL) return NULL;

    // initialize the bool variable to false.
    newnodeptr->is_end = false;
    // initialize the array of the new node to NULL values.
    for (int i = 0; i < SUMBNUMB; i++) {
        newnodeptr->children[i] = NULL;
    }
    // return the address of the allocated block.
    return newnodeptr;
}

/**
 * Inserts a key to the prefix tree.
 */
bool insert(node* ptr, const char* key)
{
    int charpos;

    // character by character we store a word into the prefix tree.
    for (int i = 0; key[i] != '\0'; i++) {
        // shearch the position in an array.
        charpos = (key[i] - 'a' >= 0)? key[i] - 'a' : SUMBNUMB - 1;
        // if there is not the address of the current letter in the cell then
        if (ptr->children[charpos] == NULL) {
            // create a new node and store its address to the current node cell.
            ptr->children[charpos] = add_node();
            if (ptr->children[charpos] == NULL) return false;
        }
        // redirect the pointer to the next new node.
        ptr = ptr->children[charpos];
    }
    // the cycle is done, that means it is the end of a word.
    ptr->is_end = true;

    // success.
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // using of the global variable.
    // do like should be!!!
    return words;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // remove the trie by using recursion.
    remove_node(head);

    // success
    return true;
}

/**
 * Removes one node from memory by the recursion approach
 * staring from the left-last node moving to the right-last node
 * and ending with the top, last node (the principle of a binary tree).
 */
void remove_node(node* ptr)
{
    // probing of the array of a node.
    for (int i = 0; i < SUMBNUMB; i++) {
        // if we found an address.
        if (ptr->children[i] != NULL) {
            // go by the address to next node to remove it.
            remove_node(ptr->children[i]);
        }
    }
    // after the whole array is checked, free the current node.
    free(ptr);
}