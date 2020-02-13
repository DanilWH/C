#include <stdio.h>
#include <stdlib.h>

int factorial(int num);

int main(int argc, char* argv[]) {
    int num = atoi(argv[1]);
    int answer = factorial(num);

    printf("%i\n", answer);
}

int factorial(int num) {
    if (num > 1) {
        return factorial(num - 1) * num;
    }
    return 1;
}