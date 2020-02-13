#include <stdio.h>
#include <math.h>

void printArr(int arr[], int n);
void heapsort(int array[], int n);
void buildTree(int array[], int i, int n);
void swap(int* a, int* b);

#define N 18

int main(void) {
    int array[] = {9, 2, 7, 8, 1, 4, 3, 5, 6, 0, 10, 11,
        60, -543, -1, 0, 1, 6};
    printArr(array, N);
    // sorting...
    heapsort(array, N);
    printArr(array, N);

}

void printArr(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%i ", arr[i]);
    }
    printf("\n");
}

void heapsort(int array[], int n) {
    for (int i = 0; i < n; i++) {
        buildTree(array, 0, n - i);
        // setup the first element of the build tree
        // at the end of the sorting array.
        swap(&array[0], &array[n - 1 - i]);
    }
}

void buildTree(int array[], int i, int n) {
    if (2 * i + 1 < n) {
        // check the left vertex.
        if (array[i] < array[2 * i + 1]) {
            // swaping...
            swap(&array[i], &array[2 * i + 1]);
        }
        // move to the left vertex.
        buildTree(array, 2 * i + 1, n);
    }
    if (2 * i + 2 < n) {
        // check the right vertex.
        if (array[i] < array[2 * i + 2]) {
            // swaping...
            swap(&array[i], &array[2 * i + 2]);
        }
        // move to the right vertex.
        buildTree(array, 2 * i + 2, n);
    }
    // check "is the current root bigger than its parent?".
    if (array[i] > array[(int) round((float) i / 2) - 1] && i > 0) {
        // swaping...
        swap(&array[i], &array[(int) round((float) i / 2) - 1]);
    }

}

void swap(int* a, int* b) {
    if (a != b) {
        *a = *a + *b;
        *b = *a - *b;
        *a = *a - *b;
    }
}