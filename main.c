#include "main.h"

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

    // Inicializácia globálnej tabuľky symbolov (GTS)
    BSTInit(&global_table);
    // Vytvorenie tabuľky pre main
    symbol_table_define_variable_or_function(&global_table, MAIN);
    tBSTNodePtr main_global_node = symbol_table_get_variable_node(global_table, MAIN);
    symbol_table_create_local_table(&main_global_node);

    if (ret != ERR_SCANNER) {

        int ret = prog(token);
        printf("\nVýsledok: %d\n", ret);
    }

    token_free(token);

    // po skončení práce uvoľni miesto po read_string
    tstring_free_struct(read_string);

    // Postupné uvoľnenie všetkých lokálnych tabuliek symbolov

    // Uvoľnenie GTS
    BSTDispose(&global_table);

    return 0;
}