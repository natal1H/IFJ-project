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
        tBSTNodePtr new_function_table;
        symbol_table_create_new_local_table(&function_global_node, &new_function_table);

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
    // Predpokladá neprázdny reťazec

    if (strlen(str) > 1 && str[0] == '0') {
        // Nepovolený tvar int (začiatočné nuly)
        return false;
    }

    for (int i = 0; i < strlen(str); i++) {
        if ( !(str[i] >= '0' && str[i] <= '9') )
            return false;
    }

    return true;
}

bool is_float(char *str) {
    // Predpokladá neprázdny reťazec
    int N = strlen(str);
    int i = 0; // Index

    // Celočíselná časť
    while (i < N) {
        // Hľadanie desatinnej časti
        if (str[i] == '.') {
            break;
        }
        else if (str[i] == 'e' || str[i] == 'E') {
            break;
        }
        else if ( !(str[i] >= '0' && str[i] <= '9') ) {
            // Nepovolený znak
            return false;
        }
        i++;
    }
    if (i == 0 || i == N - 1) {
        // Nie je desatinná čast, je to INT, nie float
        return false;
    }

    // kontrola, či je niečo za '.' alebo 'e'/'E'
    if (str[i] == '.') {
        for (i = i + 1; i < N; i++) {
            if ( !(str[i] >= '0' && str[i] <= '9') ) return false;
        }
    }
    else if (str[i] == 'e' || str[i] == 'E') {
        i++;
        if (str[i] == '+' || str[i] == '-')  {
            i++;
            if (i == N) return false;
        }
        for (i = i + 1; i < N; i++) {
            if ( !(str[i] >= '0' && str[i] <= '9') ) return false;
        }
    }

    return true;
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
    return symbol_table_get_variable_node(current_function_root, str) != NULL;
}