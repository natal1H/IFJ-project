#include "code_gen.h"

// Funkcia na prípravu generovania kódu
int code_gen_start() {
    // inicializovať kam sa bude ukladať kód
    listInit(&instr_list);

    // Inicializovať štruktúru, do ktorej sa bude zapisovať aktuálna inštrukcia
    curr_instr = tInstr_init();

    // Vložiť prvú inštrukciu - obyčajný komentár
    return ERR_OK;
}

int code_gen_end() {

    // uvolniť štruktúru pre kód
    listFree(&instr_list);

    // uvoľniť štrukturú, v ktorej bola aktuálna inštrukcia
    tInst_free_instruction(curr_instr);

    return ERR_OK;
}

char *get_string_with_prefix(char *str, char *prefix) {
    char *str_complete = malloc(sizeof(char) * (strlen(str) + strlen(prefix)));
    if (str_complete == NULL) {
        // Chyba
        return NULL;
    }

    strcpy(str_complete, prefix);
    strcat(str_complete, str);

    return str_complete;
}

int add_instruction_with_2_symbols(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix, bool global) {
    if (var_name == NULL || symbol1 == NULL || symbol2 == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }

    // Pred var a pred symboly dať predponu
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@" : "LF@") );
    char *symbol1_complete = get_string_with_prefix(symbol1, symbol1_prefix );
    char *symbol2_complete = get_string_with_prefix(symbol2, symbol2_prefix );
    if (var_complete == NULL || symbol1_complete == NULL || symbol2_complete == NULL) {
        // Chyba pri alokácii
        return -1;
    }

    // Nastaviť správne aktuálnu inštrukciu
    tInstr_set_instruction(curr_instr, type, var_complete, symbol1_complete, symbol2_complete);

    // Vložiť inštrukciu do zoznamu
    listInsertPostActive(&instr_list, curr_instr);

    // Uvoľniť miesto, kde boli var_complete, symbol1_complete, symbol2_complete
    free(var_complete);
    free(symbol1_complete);
    free(symbol2_complete);

    return ERR_OK;
}

int add_instruction_with_1_symbol(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix, bool global) {
    if (var_name == NULL || symbol == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }

    // Pred var a pred symboly dať predponu
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@" : "LF@") );
    char *symbol_complete = get_string_with_prefix(symbol, symbol_prefix );
    if (var_complete == NULL || symbol_complete == NULL) {
        // Chyba pri alokácii
        return -1;
    }

    // Nastaviť správne aktuálnu inštrukciu
    tInstr_set_instruction(curr_instr, type, var_complete, symbol_complete, NULL);

    // Vložiť inštrukciu do zoznamu
    listInsertPostActive(&instr_list, curr_instr);

    // Uvoľniť miesto, kde boli var_complete, symbol1_complete, symbol2_complete
    free(var_complete);
    free(symbol_complete);

    return ERR_OK;

}

char *determine_prefix_arithmetic(tDataType type, bool is_var, bool global) {
    char *prefix;
    if (is_var) { // Premenná
        prefix = malloc(sizeof(char) * VAR_PREFIX_LEN); // strlen
        if (prefix == NULL) return NULL;
        if (global) {
            strcpy(prefix, "GF@");
        }
        else {
            strcpy(prefix, "LF@");
        }
    }
    else { // INT/FLOAT
        if (type == T_INT) { // INT
            prefix = malloc(sizeof(char) * strlen("int@")); // strlen
            if (prefix == NULL) return NULL;
            strcpy(prefix, "int@");
        }
        else { // FLOAT
            prefix = malloc(sizeof(char) * strlen("float@")); // strlen
            if (prefix == NULL) return NULL;
            strcpy(prefix, "float@");
        }
    }

    return prefix;
}

int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix_arithmetic(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix_arithmetic(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_ADD, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix_arithmetic(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix_arithmetic(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_SUB, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix_arithmetic(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix_arithmetic(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_MUL, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix_arithmetic(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix_arithmetic(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_DIV, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix_arithmetic(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix_arithmetic(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_IDIV, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_int2float(char *var_name, char *symbol, bool is_var, bool global) {
    // Získanie prefixu
    char *prefix;
    if (is_var) {
        prefix = malloc(sizeof(char) * VAR_PREFIX_LEN); // TODO ošetriť alokáciu
        if (global) strcpy(prefix, "GF@");
        else strcpy(prefix, "LF@");
    }
    else {
        prefix = malloc(sizeof(char) * strlen("int@")); // TODO ošetriť alokáciu
        strcpy(prefix, "int@");
    }

    int ret_val = add_instruction_with_1_symbol(I_INT2FLOAT, var_name, symbol, prefix, global);
    free(prefix);
    return ret_val;
}

int gen_float2int(char *var_name, char *symbol, bool is_var, bool global) {
    // Získanie prefixu
    char *prefix;
    if (is_var) {
        prefix = malloc(sizeof(char) * VAR_PREFIX_LEN); // TODO ošetriť alokáciu
        if (global) strcpy(prefix, "GF@");
        else strcpy(prefix, "LF@");
    }
    else {
        prefix = malloc(sizeof(char) * strlen("float@")); // TODO ošetriť alokáciu
        strcpy(prefix, "float@");
    }

    int ret_val = add_instruction_with_1_symbol(I_FLOAT2INT, var_name, symbol, prefix, global);
    free(prefix);
    return ret_val;
}

int gen_int2char(char *var_name, char *symbol, bool is_var, bool global) {
    // Získanie prefixu
    char *prefix;
    if (is_var) {
        prefix = malloc(sizeof(char) * VAR_PREFIX_LEN); // TODO ošetriť alokáciu
        if (global) strcpy(prefix, "GF@");
        else strcpy(prefix, "LF@");
    }
    else {
        prefix = malloc(sizeof(char) * strlen("int@")); // TODO ošetriť alokáciu
        strcpy(prefix, "int@");
    }

    return add_instruction_with_1_symbol(I_INT2CHAR, var_name, symbol, prefix, global);
}

// TODO
int gen_stri2int(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_STRI2INT, var_name, symbol1, symbol2, "string@", "int@", global);
}

// TODO
int gen_concat(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_CONCAT, var_name, symbol1, symbol2, "string@", "string@", global);
}

// TODO
int gen_strlen(char *var_name, char *symbol, bool global) {
    return add_instruction_with_1_symbol(I_STRLEN, var_name, symbol, "string@", global);
}

// TODO
int gen_setchar(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_SETCHAR, var_name, symbol1, symbol2, "string@", "int@", global);
}

/*
// test
int main() {
    printf("CODE GEN TEST\n");

    code_gen_start();

    //tInstr *first_instr = tInstr_create(I_JUMP, "main", NULL, NULL); // len aby bola nejaká prvá inštrukcia
    //listInsertLast(&instr_list, first_instr);
    //listFirst(&instr_list);

    gen_add("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);
    gen_sub("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, true);
    gen_mul("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);
    gen_div("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, true);
    gen_idiv("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);

    gen_int2float("my_float", "42", false, true);
    gen_int2float("my_float", "tmp", true, false);

    list_print_instructions(&instr_list);

    code_gen_end();

    return 0;
}
*/