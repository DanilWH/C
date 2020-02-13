/*
Makes the stairway of stars.
*/
#include <cs50.h>
#include <stdio.h>

int input(void);

int main(void) {
    int height = input();

    for (int i = height; i > 0; i--) {
        for (int j = 0; j < height; j++) {
            if (j < i - 1){
                printf(" ");
            }
            else {
                printf("#");
            }
        }
        printf("#\n");
    }
}

int input(void) {
    int height;

    do {
        height = get_int("Height(a positive number): ");
    }
    while(height <= 0);

    return height;
}
