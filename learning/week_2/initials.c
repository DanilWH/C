/* It takes your full name and formats it to the other format.
 * For example: Arbert Enshtein - AE.
*/
/* The programm works without the toupper() method.
 * I don't know, maybe it is wrong but I like to reinvent the wheel.
 * I also made the mistake-proofing (multiple spaces)
 * and implement formatting for the above names:
 * Joseph Gordon-Levitt, Conan O’Brien, David J. Malan and so on.
 */

#include <stdio.h>
#include <cs50.h>
#include <string.h>

void format_name(string name);
bool basic_characters(string s, int index_s);

int main(void) {
    string name;
    
    name = get_string("Enter your name: ");
    printf("%s\n", name);

    // formating the name.
    format_name(name);    
    
}

void format_name(string name) {
    /* formats the given string to upper case by first characters only. */

    for (int i = 0, n = strlen(name); i < n; i++) {
        if (basic_characters(name, i) && !basic_characters(name, i - 1)) {
            if ((int) name[i] > 96 && (int) name[i] < 123) {
                printf("%c", (int) name[i] - 32);
            }
            else {
                printf("%c", name[i]);
            }
        }
    }
    printf("\n");
}

bool basic_characters(string s, int index_s) {
    /* checks "Is the current symbol basic character?". */

    bool upper_characters = (int) s[index_s] > 63 && (int) s[index_s] < 91;
    bool lower_characters = (int) s[index_s] > 96 && (int) s[index_s] < 123;
    return upper_characters || lower_characters;
}