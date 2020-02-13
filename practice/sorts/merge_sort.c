#include <stdio.h>
#include <math.h>

// prototypes of the functions.
void printArr(int array[], int n);
void sortArr(int old_array[], int new_array[], int start, int end, int n);
void merge(int new_array[], int n, int left_array[], int n_l, int right_array[], int n_r);

int main(void) {
    const int n = 9;
    int array[9] = {4, 8, 6, 2, 1, 7, 5, 3, 9};
    int new_array[9];

    printf("Nonsorted: ");
    printArr(array, n);
    sortArr(array, new_array, 0, n - 1, n);

    printf("Sorted: ");
    printArr(new_array, n);
}

void printArr(int array[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%i ", array[i]);
    }
    printf("\n");
}

void sortArr(int old_array[], int new_array[], int start, int end, int n) {
    if (n < 2)
        new_array[0] =  old_array[end];
    else {
        int l_arr[(int) round((float)n / 2)], r_arr[n / 2];

        sortArr(old_array, l_arr, start, (int) round((float)n / 2) - 1 + start, (int) round((float)n / 2));
        sortArr(old_array, r_arr, (int) round((float)n / 2) + start, n - 1 + start, n / 2);
        merge(new_array, n, l_arr, (int) round((float)n / 2), r_arr, n / 2);
    }
}

void merge(int new_array[], int n, int left_array[], int n_l, int right_array[], int n_r) {
    int l = 0, r = 0;

    while (l + r < n) {
        // the n / 2 - 1 expressions replase by len(array)(python).
        if ((left_array[l] <= right_array[r] && l < n_l) || r >= n_r) {
            new_array[l + r] = left_array[l];
            l++;
        }
        else if ((right_array[r] < left_array[l] && r < n_r) || l >= n_l) {
            new_array[l + r] = right_array[r];
            r++;
        }
    }
}