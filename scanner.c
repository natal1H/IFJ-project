#include "scanner.h"
#include "main.h"

#define scanner_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define scanner_is_number(c) (c >= '0' && c <= '9')

int scanner_initialize() {
    // Inicializuj read_string, do ktorého sa budú pri načítaní tokenov ukladať znaky
    read_string = tstring_struct_initialize();

    if (read_string == NULL) {
        return -1; // TODO: return actual error code
    }
    else {
        return 0;
    }
}

Token *token_initialize() {
    // Alokuj miesto pre štruktúru token
    Token *token = (Token *) malloc(sizeof(Token));
    if (token == NULL) {
        return NULL;
    }

    token->attribute = NULL;
    token->type = NO_TYPE;

    return token;
}

void token_free(Token *token) {
    free(token->attribute);
    free(token);
}

bool is_keyword(char *str) {
    if (strcmp(str, "def") == 0)
        return true;
    else if (strcmp(str, "do") == 0)
        return true;
    else if (strcmp(str, "else") == 0)
        return true;
    else if (strcmp(str, "end") == 0)
        return true;
    else if (strcmp(str, "if") == 0)
        return true;
    else if (strcmp(str, "not") == 0)
        return true;
    else if (strcmp(str, "nil") == 0)
        return true;
    else if (strcmp(str, "then") == 0)
        return true;
    else if (strcmp(str, "while") == 0)
        return true;
    else
        return false;
}

int token_set_type_attribute(Token *token, Token_Type type, char *attribute) {
    // Token nesmie byť neinicializovaný
    if (token == NULL) {
        return -1; // TODO: return actual error code
    }
    // Nastav token type
    token->type = type;

    // Nastav attribute - treba alokovať miesto pre string
    token->attribute = (char *) malloc(sizeof(char) * strlen(attribute)+END_OF_STRING);
    if (token->attribute == NULL) {
        return -1; // TODO: return actual error code
    }

    // Skopírovať string do token->attribute
    strcpy(token->attribute, attribute);

    return 0;
}

void print_token(Token *token) {
    printf("Token type: %d\n", token->type);
    printf("Token attribute: %s\n", token->attribute);
}

char* integer_to_string(int x) {
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer)
    {
        sprintf(buffer, "%d", x);
    }

    char* tmp = "";

    printf("%s", tmp);

    return buffer;
}

int string_to_integer(char* x) {
    char *ptr;
    long return_value = strtol(x, &ptr, 10); //Pretypovanie char -> int

    return return_value;
}

char *get_correct_float_format(char *floatStr) {
    // Správny je tvar napr. float@0x1.2666p+0

    char *correctFloat = malloc(sizeof(char) * (strlen("0x") + strlen(floatStr)) + 2);
    if (correctFloat == NULL) return NULL;
    strcpy(correctFloat, "0x");

    // Skopírovať časť od začiatku po e/E (ak je)
    int i;
    int N = strlen(floatStr);
    for (i = 0; i < N; i++) {
        if (floatStr[i] == 'e' || floatStr[i] == 'E') {
            // Časť exponent - break - táto sa bude musieť trochu prerobiť
            break;
        }
        correctFloat[i+2] = floatStr[i];
    }

    if (i == N) {
        // Nebolo e/E na konci
        strcat(correctFloat, "p+0");
    }
    else {
        // Bolo e/E na konci
        // Pozrieť, čo nasledovalo znamienko za e/E
        int j = i + 1 + 2; // nastaviť správny index kde pokračovať
        if (floatStr[i+1] != '+' && floatStr[i+1] != '-') {
            // nenasledovalo +/-
            strcat(correctFloat, "p+");
            j++;
        }
        else {
            // nasledovalo +/-
            strcat(correctFloat, "p");
            correctFloat[j] = floatStr[i+1];
            i++;
        }
        i = i + 1;
        // floatStr + i -> čisté číslo exponenta
        int exp = string_to_integer(floatStr + i);
        exp *= 4;
        char *exp_str = integer_to_string(exp);
        strcat(correctFloat, exp_str);
        free(exp_str);
    }

    return correctFloat;
}


int get_next_token(Token *token) {

    static bool eol_previously = false;

    tstring_clear_string(read_string); // str := '';

    TState state = START; // Nastavenie počiatočného stavu v DKA
    int c = getc(stdin);

    do {
        switch(state) {
            case START: // Starting point

                if (scanner_is_alpha(c) || c == (int) '_') {
                    // START -> F_ID
                    state = F_ID;

                    tstring_append_char(read_string, c); // str := symbol
                    eol_previously = false;
                }
                else if (scanner_is_number(c)) {
                    // START -> F_INT

                    state = F_INT;

                    tstring_append_char(read_string, c); // str := symbol
                    eol_previously = false;
                }
                else if (c == (int) '"') {
                    // START -> Q_STRING
                    state = Q_STRING;
                    eol_previously = false;
                }
                else if (c == (int) '=') {
                    // START -> F_ASSIGN
                    state = F_ASSIGN;

                    tstring_append_char(read_string, c); // str := symbol
                    eol_previously = false;
                }
                else if (c == (int) '+') {
                    // START -> F_ADDITION
                    token_set_type_attribute(token, ADDITION, "");
                    eol_previously = false;
                    return ERR_OK;
                }
                else if (c == (int) '*') {
                    // START -> F_MULTIPLICATION
                    token_set_type_attribute(token, MULTIPLICATION, "");
                    eol_previously = false;
                    return ERR_OK;
                }
                else if (c == (int) '-') {
                    // START -> F_SUBSTRACTION
                    state = F_SUBTRACTION;
                    tstring_append_char(read_string, c);
                    eol_previously = false;
                }
                else if (c == (int) '/') {
                    // START -> F_DIVISION
                    token_set_type_attribute(token, DIVISION, "");
                    eol_previously = false;
                    return ERR_OK;
                }
                else if (c == (int) '<') {
                    // START -> F_LESS
                    state = F_LESS;
                    tstring_append_char(read_string, c); // str := symbol
                    eol_previously = false;
                }
                else if (c == (int) '>') {
                    // START -> F_GREATER
                    state = F_GREATER;
                    tstring_append_char(read_string, c); // str := symbol
                    eol_previously = false;
                }
                else if (c == (int) '#') {
                    // START -> Q_LINE_COMMENT
                    state = Q_LINE_COMMENT;
                    eol_previously = false;
                }
                else if (c == (int) '!') {
                    // START -> Q_NOT_EQUALS
                    state = Q_NOT_EQUALS;
                    eol_previously = false;
                }
                else if (c == (int) '(') {
                    // START -> F_LEFT_ROUND_BRACKET
                    state = F_LEFT_ROUND_BRACKET;
                    eol_previously = false;
                }
                else if (c == (int) ')') {
                    // START -> F_RIGHT_ROUND_BRACKET
                    state = F_RIGHT_ROUND_BRACKET;
                    eol_previously = false;
                }
                else if (c == (int) '\n') {
                    // Ak bol ihneď predtým EOL, tak nech ďalší ignoruje
                    if (!eol_previously) {
                        eol_previously = true;
                        // START -> F_EOL
                        state = F_EOL;
                    }
                    //else zostáva v stave START
                }
                else if (c == (int) ',') {
                    // START -> F_COMMA
                    state = F_COMMA;
                    eol_previously = false;
                }
                else if (c == (int) ' ' || c == (int) '\t') {
                    // START -> START
                }
                else if (c == EOF) {
                    // Koniec súboru
                    token->type = TYPE_EOF;
                    token->attribute = NULL;
                    eol_previously = false;
                    return EOF;
                }
                else {
                    // START -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                    eol_previously = false;
                }
                break; //case START:

            case F_ID:
                if (scanner_is_alpha(c) || scanner_is_number(c) || c == (int) '_') {
                    // F_ID -> F_ID
                    tstring_append_char(read_string, c);
                }
                else {
                    // Znak mimo povolených pre identifikátor

                    // unget znak
                    ungetc(c, stdin);

                    // zisti, či ten string je medzi kľúčovými slovami
                    if (is_keyword(read_string->string)) {
                        token_set_type_attribute(token, KEYWORD, read_string->string);
                    }
                    else {
                        token_set_type_attribute(token, IDENTIFIER, read_string->string);
                    }

                    return ERR_OK;
                }
                break; //case F_ID:

            case F_INT:
                if (scanner_is_number(c)) {
                    // F_INT -> F_INT
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '.') {
                    // F_INT -> Q_FLOAT_1
                    state = Q_FLOAT_1;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) 'e' || c == (int) 'E') {
                    // F_INT -> Q_FLOAT_2
                    state = Q_FLOAT_2;
                    tstring_append_char(read_string, c);
                }
                else {
                    ungetc(c, stdin);
                    token_set_type_attribute(token, INTEGER, read_string->string);
                    return ERR_OK;
                }
                break; //case F_INT:

            case Q_FLOAT_1: // x.
                if (scanner_is_number(c)) {
                    // Q_FLOAT_1 -> F_FLOAT
                    state = F_FLOAT;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_1 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_1:

            case Q_FLOAT_2: // xe / xE
                if (scanner_is_number(c)) {
                    // Q_FLOAT_2 -> F_FLOAT
                    state = F_FLOAT;
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) '+' || c == (int) '-') {
                    // Q_FLOAT_2 -> Q_FLOAT_3
                    state = Q_FLOAT_3;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_2 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_2:

            case Q_FLOAT_3: // xe+ / xE+ / xe- / xE-
                if (scanner_is_number(c)) {
                    // Q_FLOAT_3 -> F_FLOAT
                    state = F_FLOAT;
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_FLOAT_3 -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_FLOAT_3:

            case F_FLOAT:
                if (scanner_is_number(c)) {
                    // F_FLOAT -> F_FLOAT
                    tstring_append_char(read_string, c);
                }
                else if (c == (int) 'e' || c == (int) 'E') {
                    // F_FLOAT -> Q_FLOAT_2
                    state = Q_FLOAT_2;
                    tstring_append_char(read_string, c);
                }
                else {
                    // TOKEN FLOAT
                    ungetc(c, stdin);
                    // Získaj správny tvar FLOAT
                    char *correctFloat = get_correct_float_format(read_string->string);
                    tstring_clear_string(read_string);
                    tstring_add_line(read_string, correctFloat);

                    token_set_type_attribute(token, FLOAT, read_string->string);
                    return ERR_OK;
                }
                break; //case F_FLOAT:

            case Q_STRING:
                if (c != (int) '"') {
                    // Q_STRING -> Q_STRING
                    tstring_append_char(read_string, c);
                }
                else {
                    // Q_STRING -> F_STRING
                    //ungetc(c, stdin);
                    token_set_type_attribute(token, STRING, read_string->string);
                    return ERR_OK;
                }
                break; //case Q_STRING:

            case F_SUBTRACTION:
                // TOKEN -
                ungetc(c, stdin);
                token_set_type_attribute(token, SUBTRACTION, "");
                return ERR_OK;

            case Q_LINE_COMMENT:
                if (c == (int) '\n') {
                    // Q_LINE_COMMENT -> START
                    state = START;
                }
                break; //case Q_LINE_COMMENT:


            case F_ASSIGN:
                if (c == (int) '=') {
                    // F_ASSIGN -> F_EQUALS
                    state = F_EQUALS;
                }
                else {
                    // TOKEN =
                    ungetc(c, stdin);
                    token_set_type_attribute(token, ASSIGN, "");
                    return ERR_OK;
                }

            case F_EQUALS:
                // TOKEN ==
                token_set_type_attribute(token, EQUALS, "");
                return ERR_OK;

            case Q_NOT_EQUALS:
                if (c == (int) '=') {
                    // Q_NOT_EQUALS -> F_NOT_EQUALS
                    state = F_NOT_EQUALS;
                }
                else {
                    // Q_NOT_EQUALS -> F_LEX_ERROR
                    state = F_LEX_ERROR;
                }
                break; //case Q_NOT_EQUALS:


            case F_NOT_EQUALS:
                // TOKEN !=
                token_set_type_attribute(token, NOT_EQUALS, "");
                return ERR_OK; //F_NOT_EQUALS:


            case F_GREATER:
                if (c == (int) '=') {
                    // F_GREATER -> F_GREATER_OR_EQUALS
                    state = F_GREATER_OR_EQUALS;
                }
                else {
                    // TOKEN >
                    ungetc(c, stdin);
                    token_set_type_attribute(token, GREATER, "");
                    return ERR_OK;
                }
                break; //case F_NOT_EQUALS:


            case F_LESS:
                if (c == (int) '=') {
                    // F_LESS -> F_LESS_OR_EQUALS
                    state = F_LESS_OR_EQUALS;
                }
                else {
                    // TOKEN <
                    ungetc(c, stdin);
                    token_set_type_attribute(token, LESS, "");
                    return ERR_OK;
                }
                break; //case F_LESS:


            case F_GREATER_OR_EQUALS:
                // TOKEN >=
                token_set_type_attribute(token, GREATER_OR_EQUALS, "");
                return ERR_OK; //case F_GREATER_OR_EQUALS:


            case F_LESS_OR_EQUALS:
                // TOKEN >=
                token_set_type_attribute(token, LESS_OR_EQUALS, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_LEFT_ROUND_BRACKET:
                // TOKEN (
                ungetc(c, stdin);
                token_set_type_attribute(token, LEFT_ROUND_BRACKET, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_RIGHT_ROUND_BRACKET:
                // TOKEN )
                ungetc(c, stdin);
                token_set_type_attribute(token, RIGHT_ROUND_BRACKET, "");
                return ERR_OK; //case F_LESS_OR_EQUALS:

            case F_EOL:
                // TOKEN EOL
                ungetc(c, stdin);
                token_set_type_attribute(token, EOL, "");
                return ERR_OK;

            case F_COMMA:
                // TOKEN ,
                ungetc(c, stdin);
                token_set_type_attribute(token, COMMA, "");
                return ERR_OK;

            case F_LEX_ERROR:
                // LEX ERROR
                token_set_type_attribute(token, LEX_ERROR, "");
                return ERR_SCANNER;
        } //switch(state)

        //Pokym nie je EOF suboru
    } while((c = getc(stdin)) != EOF);

    if (c == EOF && state == F_EOL) { // Doplnenie posledného EOL pred koncom súboru
        token_set_type_attribute(token, EOL, "");
        return ERR_OK;
    }

    token->type = TYPE_EOF;
    token->attribute = NULL;
    return EOF;

}
