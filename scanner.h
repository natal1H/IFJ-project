#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stringlib.h"

// Typ tokenu
typedef enum token_type {
    NO_TYPE,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,
    KEYWORD,
    ASSIGN,
    ADDITION,
    MULTIPLICATION,
    SUBTRACTION,
    DIVISION,
    LESS,
    GREATER
} Token_Type;

typedef enum keyword {
    KEYWORD_DEF,
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_END,
    KEYWORD_IF,
    KEYWORD_NOT,
    KEYWORD_NIL,
    KEYWORD_THEN,
    KEYWORD_WHILE
} Keyword;

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
    F_STRING,
    F_ASSIGN,
    F_ADDITION,
    F_MULTIPLICATION,
    F_SUBTRACTION,
    F_DIVISION,
    F_LESS,
    F_GREATER
} TState;

// Reťazec, do ktorého sa bude načítavať
TString *read_string;

Token *token_initialize();

void token_free(Token *token);

bool is_keyword(char *str);

Keyword get_keyword_type(char *str);

int token_set_type_attribute(Token *token, Token_Type type, char *attribute);

void print_token(Token *token);

int get_next_token(Token *token);

int scanner_initialize();