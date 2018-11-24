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

int function_increase_number_param(tBSTNodePtr global_table, char *function_id) {
    // Predpokladá, že funkcia je definovaná (kedže sa bude volať tesne za function_definition, ktorá ju definuje)
    // Vlastne iba volá funkciu z symtable.c, ale takto je to peknejšie zaobalené
    return symbol_table_add_param(global_table, function_id);
}

int define_variable(tBSTNodePtr *current_function_root, char *variable_id) {
    return symbol_table_define_variable_or_function(current_function_root, variable_id);
}

void set_variable_type(tBSTNodePtr *current_function_root, char *variable_id, tDataType type) {
    symbol_table_set_variable_type(*current_function_root, variable_id, type);
}

// Parser výrazov funkcie
bool is_int(char *str) {
    // TODO

}

bool is_float(char *str) {
    // TODO

}

bool is_nil(char *str) {
    if (strlen(str) != 3) {
        return false;
    }
    else {
        return (str[0] == 'n' && str[1] == 'i' && str[2] == 'l');
    }
}

bool is_variable(tBSTNodePtr current_function_root, char *str) {
    // TODO
}
/*
int check_type_compatibility_aritmetic(tBSTNodePtr *current_function_root, char *symbol1, char *symbol2) {
    // Predpokladá, že ak je nejaké id, tak už bolo definované
    // Zisti, či je symbol1 INT, FLOAT alebo STRING

    return -1;
}*/