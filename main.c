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

/*
// TEST MAIN
int main() {
    printf("TEST MAIN.c\n");


    // Inicializácia globálnej tabuľky symbolov (GTS)
    BSTInit(&global_table);
    // Vytvorenie tabuľky pre main
    symbol_table_define_variable_or_function(&global_table, MAIN);
    tBSTNodePtr main_global_node = symbol_table_get_variable_node(global_table, MAIN);
    symbol_table_create_local_table(&main_global_node);

    Print_tree(global_table);
    if (check_if_function_already_defined(global_table, MAIN)) {
        printf("MAIN defined\n");
    }
    else {
        printf("Main not defined\n");
    }

    int ret = function_definition(&global_table, "new_function");
    printf("ret: %d\n", ret);
    Print_tree(global_table);

    ret = function_definition(&global_table, MAIN);
    printf("ret: %d\n", ret);
    Print_tree(global_table);

    ret = symbol_table_get_params(global_table, "new_function");
    printf("param: %d\n", ret);
    function_increase_number_param(global_table, "new_function");
    ret = symbol_table_get_params(global_table, "new_function");
    printf("param: %d\n", ret);

    // Postupné uvoľnenie všetkých lokálnych tabuliek symbolov

    // Uvoľnenie GTS
    BSTDispose(&global_table);

    return 0;
}
 */