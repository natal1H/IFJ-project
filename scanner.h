#include <stdio.h>

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

int get_next_token();

int scanner_initialize();