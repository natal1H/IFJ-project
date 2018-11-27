#include "semantic_analysis.h"

bool check_if_function_already_defined(tGlobalTableNodePtr global_table, char *function_id) {
    tGlobalTableNodePtr func_node_ptr = get_function_node(global_table, function_id);
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

int function_definition(tGlobalTableNodePtr *global_table_root, char *function_id) {
    if (check_if_function_already_defined(*global_table_root, function_id)) {
        // funkcia už bola definovaná - dochádzalo by k redefinícii - SEMANTIC ERROR
        // TODO - radí sa toto medzi ERR_SEM_ELSE?
        fprintf(stderr, "Chyba! Redefinícia funkcie %s.\n", function_id);
        return ERR_SEM_ELSE;
    }
    else {
        // Funkcia ešte nebola definovaná - treba vytvoriť novú položku v global_table, nastaviť param na 0
        // a vytvoriť pre ňu vlastnú lokálnu tabuľku symbolov

        // Vytvorenie položky v global_table
        function_set_defined(global_table_root, function_id);
        // Nastavenie počtu params na 0
        function_set_number_params((*global_table_root), function_id, 0);

        /* TOTO SA RADŠEJ SPRAVí ručne podľa main potom
        // Získanie uzla novej funkcie v global table
        tGlobalTableNodePtr function_node = get_function_node(*global_table_root, function_id);
        // Vytvorenie novej lokálnej tabuľky
        tLocalTableNodePtr local_table;
        // Inicializácia lokálnej tabuľky
        local_table_init(&local_table);
        // Previazanie lokálnej tabuľky s uzlom v globálnej
        set_function_table(&function_node, &local_table);
        */

        return ERR_OK;
    }
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

// TODO: prerobiť, aby bralo aj 1.2e10 (desatinná časť pred e)
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

bool is_variable(tLocalTableNodePtr current_function_root, char *str) {
    return get_variable_node(current_function_root, str) != NULL;
}

char *expr_parser_create_unique_name(tLocalTableNodePtr local_table) {
    static int n = 1;
    char prefix[] = "tmp";
    char *name = NULL;
    do {
        char *n_str = integer_to_string(n);
        name = realloc(name, sizeof(char) * (strlen(n_str) + strlen(prefix)));
        if (name == NULL) {
            return NULL;
        }
        strcpy(name, prefix);
        strcat(name, n_str);

        n++;

    } while (is_variable(local_table, name)); // Skontrolovať, či už neni premenná s takým názvom v tabuľke symbolov

    return name;
}

int arithmetic_check_compatibility(tSemType type1, tSemType type2) {

    return ERR_OK;
}