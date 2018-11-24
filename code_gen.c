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

int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_ADD, var_name, symbol1, symbol2, "int@", "int@", global);
}

int gen_add_float(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_ADD, var_name, symbol1, symbol2, "float@", "float@", global);
}

int gen_sub_int(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_SUB, var_name, symbol1, symbol2, "int@", "int@", global);
}

int gen_sub_float(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_SUB, var_name, symbol1, symbol2, "float@", "float@", global);
}

int gen_mul_int(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_MUL, var_name, symbol1, symbol2, "int@", "int@", global);
}

int gen_mul_float(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_MUL, var_name, symbol1, symbol2, "float@", "float@", global);
}

int gen_div(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_DIV, var_name, symbol1, symbol2, "float@", "float@", global);
}
int gen_idiv(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_IDIV, var_name, symbol1, symbol2, "int@", "int@", global);
}

int gen_int2float(char *var_name, char *symbol, bool global) {
    return add_instruction_with_1_symbol(I_INT2FLOAT, var_name, symbol, "int@", global);
}

int gen_float2int(char *var_name, char *symbol, bool global) {
    return add_instruction_with_1_symbol(I_FLOAT2INT, var_name, symbol, "float@", global);
}

int gen_int2char(char *var_name, char *symbol, bool global) {
    return add_instruction_with_1_symbol(I_INT2CHAR, var_name, symbol, "int@", global);
}

int gen_stri2int(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_STRI2INT, var_name, symbol1, symbol2, "string@", "int@", global);
}

int gen_concat(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_CONCAT, var_name, symbol1, symbol2, "string@", "string@", global);
}

int gen_strlen(char *var_name, char *symbol, bool global) {
    return add_instruction_with_1_symbol(I_STRLEN, var_name, symbol, "string@", global);
}

int gen_setchar(char *var_name, char *symbol1, char *symbol2, bool global) {
    return add_instruction_with_2_symbols(I_SETCHAR, var_name, symbol1, symbol2, "string@", "int@", global);
}

// test
/*
int main() {
    printf("CODE GEN TEST\n");

    code_gen_start();

    //tInstr *first_instr = tInstr_create(I_JUMP, "main", NULL, NULL); // len aby bola nejaká prvá inštrukcia
    //listInsertLast(&instr_list, first_instr);
    //listFirst(&instr_list);

    gen_add_int("a", "10", "42", true);
    gen_add_float("var", "2.5", "1.2", false);
    gen_sub_int("_var", "2", "1", false);
    gen_sub_float("sub_var", "2.5", "1.2", true);
    gen_mul_int("sub_var", "2.5", "1.2", true);
    gen_mul_float("sub_var", "2.5", "1.2", true);
    gen_div("sub_var", "2.5", "1.2", true);
    gen_idiv("sub_var", "2.5", "1.2", true);

    gen_int2float("myvar", "5", false);
    gen_float2int("myvar2", "5.2", true);
    gen_int2char("myvar2", "64", true);
    gen_stri2int("str", "text", "2", true);

    list_print_instructions(&instr_list);

    code_gen_end();

    return 0;
}
*/