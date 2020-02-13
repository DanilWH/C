/*
Calculates how many bottles you need to take a shower.
*/

#include <cs50.h>
#include <stdio.h>

int main(void) {
    int bottle_num = 12;
    int minutes, answer;

    minutes = get_int("Enter number of minuter you take a shower: ");
    answer = minutes * bottle_num;
    printf("You will spend %i number of water botles.\n", answer);
}
