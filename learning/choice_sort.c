#include <stdio.h>
#include <cs50.h>
#include <string.h>

void print_arr(int arr[], int len);
int nums_num(string s);
bool isnumber(int num);
void get_arr(string nums_str, int nums[]);
void sort(int array[], int size);

int main(void) {

    string nums_str = get_string("Enter the numbers you want to sort by space: ");
    int n = nums_num(nums_str);
    int nums[n];
    for (int i = 0; i < n; i++) {
        nums[i] = 0;
    }
    get_arr(nums_str, nums);
    print_arr(nums, n);
    // sorting the array.
    sort(nums, n);
    // printing the array.
    print_arr(nums, n);
}

void print_arr(int arr[], int len) {
    /* prints the given array. */
    for (int i = 0; i < len; i++) {
        printf("%i ", arr[i]);
    }
    printf("\n");
}

int nums_num(string s) {
    /* calculates the nubmer of the numbers that is in the string. */
    int lenght = 0;

    for (int i = 0, n = strlen(s); i < n; i++) {
        if (isnumber(s[i]) && !isnumber(s[i - 1])){
            lenght++;
        }
    }
    return lenght;
}

bool isnumber(int num) {
    return num >= 48 && num <= 57;
}

void get_arr(string nums_str, int nums[]) {
    /* takes numbers from the given string and puts them to an array. */

    for (int i = 0, j = 0, n = strlen(nums_str); i < n; i++) {
        // skipping all spaces.
        while (!isnumber(nums_str[i])) i++;
        // is the checking number negative?
        bool neg = nums_str[i - 1] == '-';
        // calculating an each number and passing them to the array.
        while (isnumber(nums_str[i])) {
            nums[j] = nums[j] * 10 + (nums_str[i] - '0');
            i++;
        }
        // if the checking number is negative then multiple by -1;
        if (neg) nums[j] *= -1;
        j++;
    }
}

void sort(int array[], int size) {
    // sorts an array by sorting by choice.
    int max, idx_max, temp;

    for (int i = 0; i < size; i++) {
        max = array[i];
        idx_max = i;
        for (int j = i; j < size; j++) {
            if (array[j] > max) {
                max = array[j];
                idx_max = j;
            }
        }
        // swaping the values.
        temp = array[i];
        array[i] = max;
        array[idx_max] = temp;
    }
}