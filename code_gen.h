/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre generovanie kódu
 * @file code_gen.h
 *
 * @author Natália Holková (xholko02)
 */

#ifndef _CODE_GEN_H

    #define _CODE_GEN_H

    #include <stdbool.h>
    #include <stdio.h>

    #include "stringlib.h"
    #include "error.h"
    #include "instr_list.h"
    #include "semantic_analysis.h"
    #include "symtable.h"

    #define VAR_PREFIX_LEN 4 // GF@% LF@%
    #define VAR_PREFIX "LF@%"
    #define LABEL_PREFIX "$"
    #define FUNCTION_PREFIX "?"

    // Štruktúra - špeciálna lokálna tabuľka na názvy návestí
    tLocalTableNodePtr label_table;

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

    /**
     * @brief Vygenerovanie inštrukcie ADD
     *
     * @param var_name Reťazec názvu premennej, do ktorej sa uloží výsledok inštrukcie
     * @param symbol1 Reťazec názvu/hodnoty prvého operandu
     * @param symbol1_type Typ prvého operandu
     * @param s1_is_var Je prvý operand premenná?
     * @param symbol2 Reťazec názvu/hodnoty druhého operandu
     * @param symbol2_type Typ druhého operandu
     * @param s2_is_var Je druhý operand premenná?
     * @param global GF alebo LF?
     * @return Chybový kód
     */
    int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);

    int gen_lt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_gt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);
    int gen_eq(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var, bool global);

    int gen_or(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_not(char *var_name, char *symbol, bool global);


// TODO prerobiť
    // TODO odtadeto to treba prerobiť aby to fungovalo ako aritmetické, že berie aj premenné
    // Inštrukcie na prevody
    int gen_int2float(char *var_name, char *symbol, bool is_var, bool global);
    void convert_int_2_float(tLocalTableNodePtr *actual_function_ptr, char *symbol, char **converted_name);
    int gen_float2int(char *var_name, char *symbol, bool is_var, bool global);
    int gen_int2char(char *var_name, char *symbol, bool is_var, bool global);
    int gen_stri2int(char *var_name, char *symbol1, char *symbol2, bool global);

    // Práca s reťazcami
    int gen_concat(char *var_name, char *symbol1, char *symbol2, bool global);
    int gen_strlen(char *var_name, char *symbol, bool global);
    int gen_setchar(char *var_name, char *symbol1, char *symbol2, bool global);

    int gen_defvar(char *var_name, bool global);
    int gen_move_var(char *var_name, char *symbol, bool global);

    // Ďalšie pomocné funkcie
    char *get_string_with_prefix(char *str, char *prefix);
    int add_instruction_with_2_symbols(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix, bool global);
    int add_instruction_with_1_symbol(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix, bool global);
    int add_instruction_unary(tInstruction_type type, char *symbol, char *symbol_prefix);
    char *determine_prefix(tDataType type, bool is_var, bool global);
    char *get_and_set_unique_label(tLocalTableNodePtr *label_table, char *prefix);

    void set_and_post_instr(tListOfInstr *L, tInstr *I, tInstruction_type type, char *addr1, char *addr2, char *addr3);

    // Generovanie funkcií
    int gen_push_param(char *symbol, tDataType type, bool is_var);
    int gen_pop_var(char *var_name);
    int gen_call(char *function_name);

    // Vstavané funkcie

    /**
     * @brief Generovanie vstavanej funkcie inputs
     *
     * @return Chybový kód
     */
    int gen_inputs();

    /**
     * @brief Generovanie vstavanej funkcie inputi
     *
     * @return Chybový kód
     */
    int gen_inputi();

    /**
     * @brief Generovanie vstavanej funkcie inputf
     *
     * @return Chybový kód
     */
    int gen_inputf();

    /**
     * @brief Generovanie vstavanej funkcie print
     *
     * @return Chybový kód
     */
    int gen_print();

    /**
     * @brief Generovanie vstavanej funkcie length
     *
     * @return Chybový kód
     */
    int gen_length();

    /**
     * @brief Generovanie vstavanej funkcie substr
     *
     * @return Chybový kód
     */
    int gen_substr();

    /**
     * @brief Generovanie vstavanej funkcie ord
     *
     * @return Chybový kód
     */
    int gen_ord();

    /**
     * @brief Generovanie vstavanej funkcie chr
     *
     * @return Chybový kód
     */
    int gen_chr();

#endif