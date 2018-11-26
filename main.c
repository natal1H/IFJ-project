#include "main.h"


int main() {
    printf("MAIN.c\n");


    // inicializuj scanner najprv cez scanner_initialize()
    if ( scanner_initialize() != 0 ) {
        // chyba pri inicializácii
        return ERR_INTERNAL;
    }

    Token *token =  token_initialize();
    int ret = get_next_token(token);

    // Inicializácia globálnej tabuľky symbolov (GTS)
    global_table_init(&global_table);


    // Vytvorenie tabuľky pre main
    function_set_defined(&global_table, MAIN); // Pridá uzol MAIN do GTS
    tGlobalTableNodePtr main_global_node = get_function_node(global_table, MAIN); // Vráti ukazovvateľ na uzol s MAIN v GTS
    // Vytvorenie novej lokálnej tabuľky
    tLocalTableNodePtr new_local_table;
    local_table_init(&new_local_table); // Inicializácia novej lokálnej tabuľky
    set_function_table(&main_global_node, &new_local_table); // Naviazanie uzla v globálnej na novú lokálnu


variable_set_defined(&new_local_table, "var@main");
local_table_print(*(main_global_node->data->function_table));
actual_function_ptr = (main_global_node->data->function_table);

    /*
    printf("1\n");
    function_definition(&global_table, MAIN);
    printf("2\n");
    tGlobalTableNodePtr main = get_function_node(global_table, MAIN);
    printf("3\n");
    tLocalTableNodePtr main_loc = *(main->data->function_table);
    printf("4\n");
    variable_set_defined(&main_loc, "var@main");
    printf("5\n");
    actual_function_ptr = (main->data->function_table);
    */

    actual_function_name = MAIN; // Nastaviť actual_function_name na MAIN

printf("Actual TREE\n");
local_table_print(*actual_function_ptr);

    if (ret != ERR_SCANNER) {

        int ret = prog(token);
        printf("\nVýsledok: %d\n", ret);
    }

    // Upratanie po programe

    token_free(token);

    // po skončení práce uvoľni miesto po read_string
    tstring_free_struct(read_string);

    // Postupné uvoľnenie všetkých lokálnych tabuliek symbolov

    // Uvoľnenie GTS
    global_table_dispose(&global_table);

    return ret;
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

    printf("TEST: %d\n", is_int("12"));
    printf("TEST: %d\n", is_int("1s2"));
    printf("TEST: %d\n", is_float("12"));
    printf("TEST: %d\n", is_float("1.2"));
    printf("TEST: %d\n", is_float("1,2"));
    printf("TEST: %d\n", is_float("1e+"));


    // Postupné uvoľnenie všetkých lokálnych tabuliek symbolov

    // Uvoľnenie GTS
    BSTDispose(&global_table);

    return 0;
}
 */