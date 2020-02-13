/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
#include <cs50.h>
#include <math.h>
#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n, int start, int end)
{
    int middle = (int) round((float)n / 2) + start - 1;

    if (value < values[middle + 1] && n > 1) {
        return search(value, values, (int) round((float)n / 2), start, middle);
    }
    else if (value > values[middle] && n > 1) {
        return search(value, values, n / 2, middle + 1, end);
    }
    else if (values[middle] == value) {
        return true;
    }
    return false;
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    int j, curr_val;

    for (int i = 1; i < n; i++) {
        j = i;
        curr_val = values[j];
        while (curr_val < values[j - 1] && j > 0) {
            values[j] = values[j - 1];
            j--;
        }
        values[j] = curr_val;
    }
}

