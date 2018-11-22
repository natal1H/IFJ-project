#ifndef _CODE_GEN_H

    #define _CODE_GEN_H

    #include <stdbool.h>
    #include <stdio.h>

    #include "stringlib.h"
    #include "error.h"
    #include "instr_list.h"

    // štruktúra - zoznam inštrukcií
    tListOfInstr instr_list;

    // Pomocná štruktúra - aktuálna inštrukcia
    tInstr *curr_instr;

    // Funkcia na prípravu generovania kódu
    int code_gen_start();

    // Funkcia, ktorá uprace po generovaní kódu
    int code_gen_end();

    // Pomocné funkcie pre generovanie rôznych situácií
    int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_add_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_sub_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_sub_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_mul_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_mul_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_div(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_idiv(char *var_name, char *symbol1, char *symbol2, bool global);

    // Ďalšie pomocné funkcie
    char *get_string_with_prefix(char *str, char *prefix);
    int add_aritmetic_instruction(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol_prefix, bool global);

#endif