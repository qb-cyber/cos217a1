#include <stdio.h>
#include <stdlib.h>

/* State definitions for the DFA */
enum State {NORMAL, SLASH, COMMENT, STAR, STRING, CHAR_LITERAL, ESCAPE_STRING, ESCAPE_CHAR};

int main(void) {
    int c;
    int line = 1;
    int start_line = 0;
    enum State state = NORMAL;

    while ((c = getchar()) != EOF) {
        if (c == '\n') line++;

        switch (state) {
            case NORMAL:
                if (c == '/') state = SLASH;
                else if (c == '\"') { state = STRING; putchar(c); }
                else if (c == '\'') { state = CHAR_LITERAL; putchar(c); }
                else putchar(c);
                break;

            case SLASH:
                if (c == '*') {
                    state = COMMENT;
                    start_line = (c == '\n') ? line - 1 : line;
                    putchar(' '); /* Replace comment with a space */
                } else if (c == '/') {
                    putchar('/'); /* Stay in SLASH state essentially */
                } else if (c == '\"') {
                    putchar('/'); putchar('\"'); state = STRING;
                } else if (c == '\'') {
                    putchar('/'); putchar('\''); state = CHAR_LITERAL;
                } else {
                    putchar('/'); putchar(c); state = NORMAL;
                }
                break;

            case COMMENT:
                if (c == '*') state = STAR;
                else if (c == '\n') putchar('\n'); /* Keep line sync */
                break;

            case STAR:
                if (c == '/') state = NORMAL;
                else if (c == '*') state = STAR;
                else {
                    if (c == '\n') putchar('\n');
                    state = COMMENT;
                }
                break;

            case STRING:
                putchar(c);
                if (c == '\\') state = ESCAPE_STRING;
                else if (c == '\"') state = NORMAL;
                break;

            case ESCAPE_STRING:
                putchar(c); state = STRING;
                break;

            case CHAR_LITERAL:
                putchar(c);
                if (c == '\\') state = ESCAPE_CHAR;
                else if (c == '\'') state = NORMAL;
                break;

            case ESCAPE_CHAR:
                putchar(c); state = CHAR_LITERAL;
                break;
        }
    }

    /* Error Handling for Unterminated Comments */
    if (state == COMMENT || state == STAR) {
        fprintf(stderr, "Error: line %d: unterminated comment\n", start_line);
        return EXIT_FAILURE;
    }

    /* Handle trailing slash */
    if (state == SLASH) putchar('/');

    return EXIT_SUCCESS;
}