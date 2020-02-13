/*
Crypts a message using Vigenere algorithm.
*/

#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>

bool check_alpha(string word);
void crypt_p(string p, string c, string k);
void convert_tolower(string s);

int main(int argc, string argv[]) {
    string k = argv[1];

    if (argc != 2 || !check_alpha(k)) {
        printf("You should give two arguments only - the file name and key word!\n");
        return 1;
    }

    string p = get_string("Enter the message you want to ctypt: ");
    string c = p;
    crypt_p(p, c, k);

    // shows the result.
    printf("%s\n", c);

    return 0;
}

bool check_alpha(string word) {
    /* checks "Is the characters of the string alpha?" */

    for (int i = 0, n = strlen(word); i < n; i++)
        if (!isalpha(word[i])) return false;

    return true;
}

void crypt_p(string p, string c, string k) {
    // convetring of the key to the lower case.
    convert_tolower(k);
    
    // the lenght of the key word.
    int k_len = strlen(k);

    // crypting...
    for (int i = 0, j = 0, n = strlen(p); i < n; i++) {
        if (isalpha(p[i])) {
            // for the upper case.
            if (isupper(p[i]))
                c[i] = (((p[i] - 'A') + (k[j % k_len] - 'a')) % 26) + 'A';
            // for the lower case.
            else if (islower(p[i]))
                c[i] = (((p[i] - 'a') + (k[j % k_len] - 'a')) % 26) + 'a';
            j++;    
        }
        // for another case.
        else
            c[i] = p[i];
    }
}

void convert_tolower(string s) {
    /* converts the given string to the lower case. */

    for (int i = 0, n = strlen(s); i < n; i++)
        s[i] = tolower(s[i]);
}
