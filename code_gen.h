#ifndef _CODE_GEN_H

    #define _CODE_GEN_H

    #include <stdbool.h>
    #include <stdio.h>

    #include "stringlib.h"
    #include "error.h"
    #include "instr_list.h"
    #include "semantic_analysis.h"
    #include "symtable.h"

    #define VAR_PREFIX_LEN 3

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
    int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);

    int gen_lt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_gt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_eq(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);

    int gen_or(char *var_name, char *symbol1, char *symbol2, bool global);


// TODO prerobiť
    // TODO odtadeto to treba prerobiť aby to fungovalo ako aritmetické, že berie aj premenné
    // Inštrukcie na prevody
    int gen_int2float(char *var_name, char *symbol, bool is_var, bool global);
    int gen_float2int(char *var_name, char *symbol, bool is_var, bool global);
    int gen_int2char(char *var_name, char *symbol, bool is_var, bool global);
    int gen_stri2int(char *var_name, char *symbol1, char *symbol2, bool global);
    // Vstupno výstupné inštrukcie
    // TODO - doplniť
    // Práca s reťazcami
    int gen_concat(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_strlen(char *var_name, char *symbol, bool global);
    int gen_setchar(char *var_name, char *symbol1, char *symbol2, bool global);

    int gen_defvar(char *var_name, bool global);

    // Ďalšie pomocné funkcie
    char *get_string_with_prefix(char *str, char *prefix);
    int add_instruction_with_2_symbols(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix, bool global);
    int add_instruction_with_1_symbol(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix, bool global);
    char *determine_prefix(tDataType type, bool is_var, bool global);

    void convert_int_2_float(tLocalTableNodePtr *actual_function_ptr, char *symbol, char **converted_name);


#endif