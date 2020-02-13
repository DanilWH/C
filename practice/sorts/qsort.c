#include <stdio.h>

// prototypes of functions.
void printArr(int array[], int start, int end);
void qsort(int array[], int start, int end);
int redistribution(int array[], int start, int end);

int main(void) {
    const int n = 16;
    int array[] = {3, 2, 1, 0, 4, 7, 5, 8, 9, 10, 5, 9, 6, -1, -41, -10};

    printArr(array, 0, n - 1);
    qsort(array, 0, n - 1);
    printArr(array, 0, n - 1);
}

void printArr(int array[], int start, int end) {
    for (int i = start; i <= end; i++) {
        printf("%i ", array[i]);
    }
    printf("\n");
}

void qsort(int array[], int start, int end) {
    if (end - start + 1 > 1) {
        int pivod = redistribution(array, start, end);
        qsort(array, start, pivod - 1);
        qsort(array, pivod, end);
    }
}

int redistribution(int array[], int start, int end) {
    int pivod = array[(start + end + 1) / 2], temp;
    int ls = start - 1, rs = end + 1;

    while(rs > ls) {
        // searching the values the function should swap.
        do
            ls++;
        while(array[ls] < pivod);
        do
            rs--;
        while(array[rs] > pivod);

        if (rs <= ls) return ls;

        // swapping the wrong values.
        temp = array[rs];
        array[rs] = array[ls];
        array[ls] = temp;
    }
}