#include <stdio.h>

#include "scanner.h" // treba?
#include "parser.h"

int main() {
    printf("MAIN.c\n");


    // inicializuj scanner najprv cez scanner_initialize()
    if ( scanner_initialize() != 0 ) {
        // chyba pri inicializácii
        printf("Chyba pri inicializácii scannera");
        return ERR_INTERNAL;
    }


    Token *token =  token_initialize();
    int ret = get_next_token(token);
    tStack *endElseStack;
    StackInit(endElseStack);

    if (ret != EOF && ret != ERR_SCANNER) {
        int ret = prog(token, endElseStack);
        printf("%d\n", ret);
    }

    token_free(token);

    // po skončení práce uvoľni miesto po read_string
    tstring_free_struct(read_string);


    return 0;
}