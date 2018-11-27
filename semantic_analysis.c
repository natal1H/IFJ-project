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

// TODO: najprv prerobiť vrátenie string v scannery aby to vrátilo ako \"txt\", lebo inak neni šanca to rozoznať od symbolu iba z pohľadu na reťazec
bool is_string_literal(char *str) {
    int N = strlen(str); // Dĺžka reťazca

    // Kontrola, či je dĺžka aspoň 2 - prázdny reťazec je ""
    if (N < 2) return false;

    // Kontrola, či sa začína na "
    if (str[0] != '"') return false;

    for (int i = 1; i < N; i+i) {

    }

    //

    return true;
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


int arithmetic_check_compatibility(tDataType type1, tDataType type2) {
    if ( (type1 == T_INT && type2 == T_STRING) || (type1 == T_STRING && type2 == T_INT) ) {
        // INT op STRING -> chyba
        return ERR_SEM_TYPE;
    }
    else if ( (type1 == T_FLOAT && type2 == T_STRING) || (type1 == T_STRING && type2 == T_FLOAT) ) {
        // FLOAT op STRING -> chyba
        return ERR_SEM_TYPE;
    }
    else if ( (type1 == T_INT && type2 == T_NIL) || (type1 == T_NIL && type2 == T_INT)) {
        // INT op NIL -> chyba
        return ERR_SEM_TYPE;
    }
    else if ( (type1 == T_FLOAT && type2 == T_NIL) || (type1 == T_NIL && type2 == T_FLOAT) ) {
        // FLOAT op NIL -> chyba
        return ERR_SEM_TYPE;
    }
    else if ( (type1 == T_STRING && type2 == T_NIL) || (type1 == T_NIL && type2 == T_STRING) ) {
        // STRING op NIL -> chyba
        return ERR_SEM_TYPE;
    }
    else if (type1 == T_BOOLEAN || type2 == T_BOOLEAN) {
        // BOOLEAN nemôže byť medzi aritmetickými operáciami (ok?)
        return ERR_SEM_TYPE;
    }
    else {
        return ERR_OK;
    }
}

int get_type_from_token(tLocalTableNodePtr *current_function_root, char *token_ID, tDataType *type) {

    if (is_int(token_ID)) {
        *type = T_INT;
        return ERR_OK;
    }
    else if (is_float(token_ID)) {
        *type = T_FLOAT;
        return ERR_OK;
    }
    else if (is_variable(*current_function_root, token_ID)) {
        // Pozrieť sa do tabuľky symbolov a vrátiť typ
        *type = variable_get_type(*current_function_root, token_ID);
        if (*type != T_UNDEFINED) return ERR_OK;
        else return ERR_SEM_UNDEF; // Sémantická chyba: nedefinovaná premenná
    }
    else if (is_nil(token_ID)) {
        *type = T_NIL;
        return ERR_OK;
    }
    // TODO
    //else if (is_string_literal(token_ID)) {
    //    *type = T_STRING;
    //    return ERR_OK;
    //}
    else {
        // Sémantická chyba - nedefinovaná premenná
        return ERR_SEM_UNDEF;
    }

}

tDataType aritmetic_get_final_type(tDataType token1, tDataType token2) {
    if (token1 == T_INT && token2 == T_INT) return T_INT;
    else if ( (token1 == T_INT && token2 == T_FLOAT) || (token1 == T_FLOAT && token2 == T_INT) ) return T_FLOAT;
    else if ( (token1 == T_STRING && token2 == T_STRING)) return T_STRING;
    //else if ( (token1 == T_INT && token2 == T_NIL)) return T_NIL;
    else return T_UNDEFINED;
}
