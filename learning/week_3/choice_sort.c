#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void printArr(int array[], int n);
void sortArr(int array[], int n);

int main(int argc, string argv[]) {
    int array[argc - 1];

    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < strlen(argv[i]); j++) {
            if (!isdigit(argv[i][j]) && (argv[i][j] != '-')) {
                printf("Something went wrong!\n");
                return 1;
            }
        }
        array[i - 1] = atoi(argv[i]);
    }
    printf("Unsorted: ");
    printArr(array, argc - 1);
    sortArr(array, argc - 1);
    printf("Sorted: ");
    printArr(array, argc - 1);

    return 0;
}

void printArr(int array[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%i ", array[i]);
    }
    printf("\n");
}

void sortArr(int array[], int n) {
    int val_idx, temp;

    for (int i = 0; i < n - 1; i++) {
        val_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (array[j] < array[val_idx]) val_idx = j;
        }
        temp = array[i];
        array[i] = array[val_idx];
        array[val_idx] = temp;
    }
}