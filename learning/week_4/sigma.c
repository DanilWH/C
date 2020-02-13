#include <stdio.h>
#include <stdlib.h>

int sigma(int number);

int main(int argc, char* argv[]) {
    if (argc == 2) {
        int result = sigma(atoi(argv[1]));
        printf("%i\n", result);
        return 0;
    }
    printf("You should give the number!\n");
    return 1;
}

int sigma(int number) {
    if (number <= 1)
        return 1;

    return number + sigma(number - 1);
}
