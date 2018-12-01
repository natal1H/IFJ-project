#ifndef _SCANNER_H
    #define _SCANNER_H

    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>

    #include "stringlib.h"
    #include "error.h"

    // Typ tokenu
    //Cislovanie od 1
    typedef enum token_type {
        //Nazov tokenu       //ID_cislo
        NO_TYPE,             //0
        IDENTIFIER,          //1
        INTEGER,             //2
        FLOAT,               //3
        STRING,              //4
        KEYWORD,             //5
        ASSIGN,              //6
        ADDITION,            //7
        MULTIPLICATION,      //8
        SUBTRACTION,         //9
        DIVISION,            //10
        LESS,                //11
        LESS_OR_EQUALS,      //12
        GREATER,             //13
        GREATER_OR_EQUALS,   //14
        EQUALS,              //15
        NOT_EQUALS,          //16
        LEFT_ROUND_BRACKET,  //17
        RIGHT_ROUND_BRACKET, //18
        COMMA,               //19
        EOL,                 //20
        TYPE_EOF,            //21
        LEX_ERROR,           //22
    } Token_Type;

    typedef enum keyword {
        //Nazov tokenu  //ID_cislo
        KEYWORD_DEF,    //0
        KEYWORD_DO,     //1
        KEYWORD_ELSE,   //2
        KEYWORD_END,    //3
        KEYWORD_IF,     //4
        KEYWORD_NOT,    //5
        KEYWORD_NIL,    //6
        KEYWORD_THEN,   //7
        KEYWORD_WHILE,  //8
    } Keyword;

    typedef struct token {
        Token_Type type;
        char *attribute;
    } Token;

    // Stavy konečného automatu
    typedef enum state {
        // Počiatočný stav
        START,

        // Koncové stavy
        F_ID,
        F_INT,
        F_FLOAT,
        F_STRING,
        F_ASSIGN, // =
        F_ADDITION, // +
        F_MULTIPLICATION, // *
        F_SUBTRACTION, // -
        F_DIVISION, // /
        F_LESS, // <
        F_GREATER, // >
        F_EQUALS, // ==
        F_NOT_EQUALS, // !=
        F_LESS_OR_EQUALS, // <=
        F_GREATER_OR_EQUALS, // >=
        F_LEFT_ROUND_BRACKET,  // (
        F_RIGHT_ROUND_BRACKET, // )
        F_COMMA, // ,
        F_EOL, // \n

        // Prechodné stavy
        Q_LINE_COMMENT,

        Q_BLOCK_COMMENT_BEGIN_1, // =b
        Q_BLOCK_COMMENT_BEGIN_2, // =be
        Q_BLOCK_COMMENT_BEGIN_3, // =beg
        Q_BLOCK_COMMENT_BEGIN_4, // =begi
        Q_BLOCK_COMMENT_BEGIN_5, // =begin
		Q_BLOCK_COMMENT_CONTENT, //obsah blokoveho komentara
        Q_BLOCK_COMMENT_END_1, // =
        Q_BLOCK_COMMENT_END_2, // =e
        Q_BLOCK_COMMENT_END_3, // =en
		Q_BLOCK_COMMENT_END_4, // =end
        Q_BLOCK_COMMENT_POSTEND, //obsah komentaru v riadku za "=end"

        Q_NOT_EQUALS,

        Q_FLOAT_1, // x.
        Q_FLOAT_2, // xe / xE
        Q_FLOAT_3, // xe+ / xE+ / xe- / xE-

        Q_STRING,

        F_LEX_ERROR // Lexikálna chyba
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

#endif
