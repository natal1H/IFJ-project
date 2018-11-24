#include "semantic_analysis.h"

bool check_if_function_already_defined(tBSTNodePtr global_table, char *function_id) {
    tBSTNodePtr func_node_ptr = symbol_table_get_variable_node(global_table, function_id);
    if (func_node_ptr == NULL) {
        return false;
    }
    else {
        if (func_node_ptr->data != NULL) {
            return func_node_ptr->data->defined;
        }
        else {
            return false;
        }
    }
}

int function_definition(tBSTNodePtr *global_table_root, char *function_id) {
    if (check_if_function_already_defined(*global_table_root, function_id)) {
        // funkcia už bola definovaná - dochádzalo by k redefinícii - SEMANTIC ERROR
        // TODO - radí sa toto medzi ERR_SEM_ELSE?
        return ERR_SEM_ELSE;
    }
    else {
        // Funkcia ešte nebola definovaná - treba vytvoriť novú položku v global_table, nastaviť param na 0
        // a vytvoriť pre ňu vlastnú lokálnu tabuľku symbolov
        symbol_table_define_variable_or_function(&(*global_table_root), function_id); // vytvorenie položky v global_table
        symbol_table_set_param(*global_table_root, function_id, 0); // Nastavenie param na 0
        tBSTNodePtr function_global_node = symbol_table_get_variable_node(*global_table_root, function_id); // Získanie uzla novej funkcie v GTS
        symbol_table_create_local_table(&function_global_node); // Vytvorenie lokálne TS

        return ERR_OK;
    }
}
