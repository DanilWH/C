/*
Crypts a message using Caesar algorithm.
*/

#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int crypt_char(char c, int k);

int main(int argc, string argv[]) {
    if (argc == 2 ) {
        // converting the string to int.
        int k = atoi(argv[1]);
        // getting the message, that will crypt.
        string p = get_string("Enter the message you want to crypt: ");
        string c = p;

        for (int i = 0, n = strlen(p); i < n; i++) {
            c[i] = crypt_char(p[i], k);
        }
        printf("%s\n", c);

        return 0;
    }
    else {
        printf("You should give 2nd argument only - the crypt key!\n");
        return 1;
    }
}

int crypt_char(char c, int crypt_key) {
    if (isalpha(c)) {
        // the variable that stores a ASCI character number of the crypted string. 
        int ci;
        if (isupper(c)){
            ci = (((int) c - 65 + crypt_key) % 26) + 65;
            return ci;
        }
        else if (islower(c)) {
            ci = (((int) c + crypt_key - 97) % 26) + 97;
            return ci;
        }
    }
    return c;
}
