#include <stdio.h>
#include <math.h>
#include <cs50.h>
#include <stdlib.h>

void printArr(int array[], int n);
void sortArr(int old_array[], int new_array[], int start, int end, int n);
void merge(int new_array[], int n, int left_array[], int n_l, int right_array[], int n_r);
int fillArrAndReturnN(int array[]);
bool search(int value, int array[], int n, int start, int end);

const int MAX = 65536;

int main(int argc, string argv[]) {
    if (argc != 2) {
        printf("You must give the needle value. Try again!");
        return 1;
    }

    int nonsorted_array[MAX];

    // filling the array.
    int n = fillArrAndReturnN(nonsorted_array);
    int sorted_array[n];

    // printing the nonsorted array.
    printf("\nNonsorted: ");
    printArr(nonsorted_array, n);
    // soring by the merge sort.
    sortArr(nonsorted_array, sorted_array, 0, n - 1, n);
    //printing the sorted array.
    printf("Sorted: ");
    printArr(sorted_array, n);

    // searching the needle value.
    int value = atoi(argv[1]);
    if (search(value, sorted_array, n, 0, n - 1))
        printf("Found!\n");
    else
        printf("Didn't find!\n");

    return 0;
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

int fillArrAndReturnN(int array[]) {
    int n, value;

    for (n = 0; n < MAX; n++) {
        value = get_int("\nEnter the array[%i]: ", n);
        if (value == __INT_MAX__)
            break;
        array[n] = value;
    }
    return n;
}

bool search(int value, int array[], int n, int start, int end) {
    int middle = (int) round((float)n / 2) + start - 1;

    if (value < array[middle + 1] && n > 1)
        return search(value, array, (int) round((float)n / 2), start, middle);
    else if (value > array[middle] && n > 1)
        return search(value, array, n / 2, middle + 1, end);
    else if (value == array[middle]) return true;
    return false;
}