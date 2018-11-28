#include "code_gen.h"

// Funkcia na prípravu generovania kódu
int code_gen_start() {
    // inicializovať kam sa bude ukladať kód
    listInit(&instr_list);

    // Inicializovať štruktúru, do ktorej sa bude zapisovať aktuálna inštrukcia
    curr_instr = tInstr_init();

    // Inicializácia špeciálnej lokálnej tabuľky s návestiami
    local_table_init(&label_table);

    // Vložiť hlavičku, JUMP $main, LABEL $main
    // Hlavička
    tInstr_set_instruction(curr_instr, I_HEADER, NULL, NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);
    // JUMP $main
    tInstr_set_instruction(curr_instr, I_JUMP, "$main", NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);
    // LABEL $main
    tInstr_set_instruction(curr_instr, I_LABEL, "$main", NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);
    // Vytvorenie lokálneho rámca pre $main
    // CREATEFRAME
    tInstr_set_instruction(curr_instr, I_CREATEFRAME, NULL, NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);
    // PUSHFRAME
    tInstr_set_instruction(curr_instr, I_PUSHFRAME, NULL, NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);

    return ERR_OK;
}

int code_gen_end() {

    // uvolniť štruktúru pre kód
    listFree(&instr_list);

    // uvoľniť štrukturú, v ktorej bola aktuálna inštrukcia
    tInst_free_instruction(curr_instr);

    // Uvoľnenie lokálnej tabuľky s návestiami
    local_table_dispose(&label_table);

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
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@%" : "LF@%") );
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
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@%" : "LF@%") );
    char *symbol_complete = get_string_with_prefix(symbol, symbol_prefix );
    if (var_complete == NULL || symbol_complete == NULL) {
        // Chyba pri alokácii
        return ERR_INTERNAL;
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

int add_instruction_unary(tInstruction_type type, char *symbol, char *symbol_prefix) {
    if (symbol == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }

    // Pred var a pred symboly dať predponu
    char *symbol_complete = get_string_with_prefix(symbol, symbol_prefix );
    if (symbol_complete == NULL) {
        // Chyba pri alokácii
        return ERR_INTERNAL;
    }

    // Nastaviť správne aktuálnu inštrukciu
    tInstr_set_instruction(curr_instr, type, symbol_complete, NULL, NULL);

    // Vložiť inštrukciu do zoznamu
    listInsertPostActive(&instr_list, curr_instr);

    // Uvoľniť miesto
    free(symbol_complete);

    return ERR_OK;
}

char *determine_prefix(tDataType type, bool is_var, bool global) {
    char *prefix;
    if (is_var) { // Premenná
        prefix = malloc(sizeof(char) * VAR_PREFIX_LEN);
        if (prefix == NULL) return NULL;
        if (global) {
            strcpy(prefix, "GF@%");
        }
        else {
            strcpy(prefix, "LF@%");
        }
    }
    else { // INT/FLOAT
        if (type == T_INT) { // INT
            prefix = malloc(sizeof(char) * strlen("int@"));
            if (prefix == NULL) return NULL;
            strcpy(prefix, "int@");
        }
        else if (type == T_FLOAT) { // FLOAT
            prefix = malloc(sizeof(char) * strlen("float@"));
            if (prefix == NULL) return NULL;
            strcpy(prefix, "float@");
        }
        else if (type == T_STRING) {
            prefix = malloc(sizeof(char) * strlen("string@"));
            if (prefix == NULL) return NULL;
            strcpy(prefix, "string@");
        }
        else if (type == T_BOOLEAN) {
            prefix = malloc(sizeof(char) * strlen("boolean@"));
            if (prefix == NULL) return NULL;
            strcpy(prefix, "boolean@");
        }
        else { // NIL
            prefix = malloc(sizeof(char) * strlen("nil@"));
            if (prefix == NULL) return NULL;
            strcpy(prefix, "nil@");
        }
    }
    return prefix;
}

int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_ADD, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_SUB, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_MUL, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_DIV, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

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
        if (global) strcpy(prefix, "GF@%");
        else strcpy(prefix, "LF@%");
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
        if (global) strcpy(prefix, "GF@%");
        else strcpy(prefix, "LF@%");
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
        if (global) strcpy(prefix, "GF@%");
        else strcpy(prefix, "LF@%");
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

int gen_defvar(char *var_name, bool global) {
    printf("\nDEFVAL: %s\n", var_name);
    if (var_name == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }
    // Pred var a pred symboly dať predponu
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@%" : "LF@%") );
    if (var_complete == NULL) {
        // Chyba pri alokácii
        return -1;
    }

    // Nastaviť správne aktuálnu inštrukciu
    tInstr_set_instruction(curr_instr, I_DEFVAR ,var_complete, NULL, NULL);

    // Vložiť inštrukciu do zoznamu
    listInsertPostActive(&instr_list, curr_instr);

    // Uvoľniť miesto, kde boli var_complete
    free(var_complete);

    return ERR_OK;
}

int gen_lt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_LT, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_gt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_GT, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

int gen_eq(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global) {
    // Nastavenie prefixov
    char *prefix1 = determine_prefix(symbol1_type, s1_is_var, global);
    char *prefix2 = determine_prefix(symbol2_type, s2_is_var, global);

    int ret_val = add_instruction_with_2_symbols(I_EQ, var_name, symbol1, symbol2, prefix1, prefix2, global);
    free(prefix1);
    free(prefix2);
    return ret_val;
}

void convert_int_2_float(tLocalTableNodePtr *actual_function_ptr, char *symbol, char **converted_name) {
    *converted_name = expr_parser_create_unique_name(*actual_function_ptr); // Získam meno premennej, do ktorej sa bude ukladať konverzia

    // Vytvorenie premennej v loc. tabuľke symbolov
    variable_set_defined(actual_function_ptr, *converted_name);
    variable_set_type(*actual_function_ptr, *converted_name, T_FLOAT);

    // Vygenerovanie premennej, ktorá vznikne pri konverzii
    gen_defvar(*converted_name, false);

    // Pridať inštrukciu INT2FLOAT
    gen_int2float(*converted_name, symbol, is_variable(*actual_function_ptr, symbol), false);
}

int gen_or(char *var_name, char *symbol1, char *symbol2, bool global) {
    // Nastavenie prefixov
    char *prefix1 = global ? "GF@%" : "LF@%";
    char *prefix2 = global ? "GF@%" : "LF@%";

    int ret_val = add_instruction_with_2_symbols(I_OR, var_name, symbol1, symbol2, prefix1, prefix2, global);
    return ret_val;
}

int gen_not(char *var_name, char *symbol, bool global) {
    // Nastavenie prefixov
    char *prefix = global ? "GF@%" : "LF@%";

    int ret_val = add_instruction_with_1_symbol(I_OR, var_name, symbol, prefix, global);
    return ret_val;
}

int gen_move_var(char *var_name, char *symbol, bool global) {
    char *prefix = global ? "GF@%" : "LF@%";

    return add_instruction_with_1_symbol(I_MOVE, var_name, symbol, prefix, global);
}

char *get_and_set_unique_label(tLocalTableNodePtr *label_table, char *prefix) {
    int n = 1;
    char *name = NULL;

    do {
        char *n_str = malloc(sizeof(char) * sizeof(int) * 4 + 1);
        name = realloc(name, sizeof(char) * (strlen(n_str) + strlen(prefix)));
        if (n_str) {
            sprintf(n_str, "%d", n);
        }

        if (name == NULL) {
            return NULL;
        }
        strcpy(name, prefix);
        strcat(name, n_str);

        free(n_str);
        n++;
    } while (is_variable(*label_table, name));

    // Vložiť do lokálnej tabuľky symbolov
    variable_set_defined(label_table, name);

    return name;
}

int gen_push_var(char *symbol, tDataType type, bool is_var) {
    int ret;
    if (is_var)
        ret = add_instruction_unary(I_PUSHS, symbol, VAR_PREFIX);
    else if (type == T_INT)
        ret = add_instruction_unary(I_PUSHS, symbol, "int@");
    else if (type == T_FLOAT)
        ret = add_instruction_unary(I_PUSHS, symbol, "float@");
    else if (type == T_STRING)
        ret = add_instruction_unary(I_PUSHS, symbol, "string@");
    else if (type == T_NIL)
        ret = add_instruction_unary(I_PUSHS, symbol, "nil@");

    return ret;
}

int gen_pop_var(char *var_name) {
    return add_instruction_unary(I_POPS, var_name, VAR_PREFIX);
}

int gen_call(char *function_name) {
    return add_instruction_unary(I_CALL, function_name, FUNCTION_PREFIX);
}

int gen_function_label(char *function_name) {
    return add_instruction_unary(I_LABEL, function_name, FUNCTION_PREFIX);
}

void prepare_for_func() {
    listFirst(&instr_list); // Aktívny je prvý -> .IFJcode2018
    listNext(&instr_list); // Aktívny je druhý -> JUMP $main
    // teraz sa bude vkladať za JUMP $main
}

void end_function() {
    // RETURN a presune aktivitu na posledný prvok
    tInstr_set_instruction(curr_instr, I_RETURN, NULL, NULL, NULL);
    listInsertPostActive(&instr_list, curr_instr);

    listLast(&instr_list);
}

/*
// test
int main() {
    printf("CODE GEN TEST\n");

    code_gen_start();

    //tInstr *first_instr = tInstr_create(I_JUMP, "main", NULL, NULL); // len aby bola nejaká prvá inštrukcia
    //listInsertLast(&instr_list, first_instr);
    //listFirst(&instr_list);

    printf("Label Table:\n");
    local_table_print(label_table);

    char *label = get_and_set_unique_label(&label_table, "if");
    get_and_set_unique_label(&label_table, "if");
    get_and_set_unique_label(&label_table, "while");
    get_and_set_unique_label(&label_table, "else");
    get_and_set_unique_label(&label_table, "while");

    //local_table_print(label_table);

    gen_push_var("42", T_INT, false);
    gen_pop_var("ret");
    gen_call("func");

    prepare_for_func();
    gen_function_label("func");

    gen_add("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);
    gen_sub("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, true);
    gen_mul("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);
    gen_div("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, true);
    gen_idiv("my_var", "tmp1", T_INT, true, "1.2", T_FLOAT, false, false);
    gen_push_var("42", T_INT, false);

    end_function();

    gen_int2float("my_float", "42", false, true);
    gen_int2float("my_float", "tmp", true, false);

    list_print_instructions(&instr_list);

    code_gen_end();

    return 0;
}
*/