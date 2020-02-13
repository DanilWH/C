#include <stdio.h>

void printArr(int array[], int n);
void sortArr(int array[], int n);

int main(void) {
    const int n = 13;
    int array[13] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3};

    printArr(array, n);
    sortArr(array, n);
    printArr(array, n);
}

void printArr(int array[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%i ", array[i]);
    }
    printf("\n");
}

void sortArr(int array[], int n) {
    int j, element, temp;

    for (int i = 1; i < n; i++) {
        // the algoritm more reflecs the essence its work for me.
        j = i;
        element = array[j];
        while (j > 0 && element < array[j - 1]) {
            array[j] = array[j - 1];
            j--;
        }
        array[j] = element;

        // but you can do so.
        /*
        for (int j = i; j > 0 && array[j] < array[j - 1]; j--) {
            temp = array[j];
            array[j] = array[j - 1];
            array[j - 1] = temp;
        }
        */
    }
}