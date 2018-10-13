#include <stdio.h>
#include <string.h>

#include "stringlib.h"

// Typ tokenu
typedef enum token_type {
    IDENTIFIER,
    INTEGER,
    FLOAT,
    KEYWORD
} Token_Type;

typedef struct token {
    Token_Type type;
    char *attribute;
} Token;

// Stavy konečného automatu
typedef enum state {
    START,
    F_ID,
    F_INT,
    F_FLOAT,
    F_STRING
} TState;

// Reťazec, do ktorého sa bude načítavať
TString *read_string;

Token *token_initialize();

void token_free(Token *token);

int token_set_type_attribute(Token *token, Token_Type type, char *attribute);

void print_token(Token *token);

int get_next_token(Token *token);

int scanner_initialize();