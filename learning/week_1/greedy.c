/*
Counts the minimum number of coins for change.
*/

#include <cs50.h>
#include <stdio.h>

int get_change(void);

int main(void) {
    int a = 25, b = 10, c = 5, d = 1;
    int coins_count = 0;

    int change = get_change();

    printf("Your nominal is ");

    while (change > 0) {

        if (a <= change) {
            change -= a;
            printf("%i", a);
        }
        else if (b <= change) {
            change -= b;
            printf("%i", b);
        }
        else if (c <= change) {
            change -= c;
            printf("%i", c);
        }
        else if (d <= change) {
            change -= d;
            printf("%i", d);
        }
        if (change != 0) printf(" + ");
        coins_count++;
    }
    printf("\nYou will get %i coins.\n", coins_count);
}

int get_change(void) {
    float change;

    do
        change = get_float("Enter your change you should ($): ");
    while (change < 0);

    return change * 100;
}
