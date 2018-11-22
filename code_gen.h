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
    // Aritmetické inštrukcie
    int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_add_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_sub_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_sub_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_mul_int(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_mul_float(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_div(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_idiv(char *var_name, char *symbol1, char *symbol2, bool global);
    // Inštrukcie na prevody
    int gen_int2float(char *var_name, char *symbol, bool global);
    int gen_float2int(char *var_name, char *symbol, bool global);
    int gen_int2char(char *var_name, char *symbol, bool global);
    int gen_stri2int(char *var_name, char *symbol1, char *symbol2, bool global);

    // Ďalšie pomocné funkcie
    char *get_string_with_prefix(char *str, char *prefix);
    int add_instruction_with_2_symbols(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix, bool global);
    int add_instruction_with_1_symbol(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix, bool global);


#endif