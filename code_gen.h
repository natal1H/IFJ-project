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

    #define VAR_PREFIX_LEN 4 // LF@%
    #define VAR_PREFIX "LF@%"
    #define LABEL_PREFIX "$"
    #define FUNCTION_PREFIX "?"

    // Štruktúra - špeciálna lokálna tabuľka na názvy návestí
    tLocalTableNodePtr label_table;

    // štruktúra - zoznam inštrukcií
    tListOfInstr instr_list;

    // Pomocná štruktúra - aktuálna inštrukcia
    tInstr *curr_instr;

    // Zásobník parametrov a základné funkcie preň
    /** @struct Element zásobníka parametrov **/
    typedef struct tparam {
       char *id; // Identifikátor parametra
       struct  tparam *next; // Nasledujúci parameter
    } TParam;

    /** @struct Zásobník parametrov **/
    typedef struct {
        TParam *top;
    } TParamStack;

    // Zásobník na parametre
    TParamStack stackParam;

    /**
     * @brief Inicializácia zásobníka parametrov
     *
     * @param s Zásobník parametrov
     */
    void ParamStackInit(TParamStack *s);

    /**
     * @brief Vloženie parametra na zásobník parametrov
     *
     * @param s Zásobník parametrov
     * @param id Identifikátor parametra
     */
    void ParamStackPush(TParamStack *s, char *id);

    /**
     * @brief Odstránenie hodnoty z vrcholu zásobníka parametrov
     *
     * @param s Zásobník parametrov
     */
    void ParamStackPop(TParamStack *s);

    /**
     * @brief Vrátenie hodnoty z vrcholu zásobníka parametrov
     *
     * @param s Zásobník parametrov
     * @return Identifikátor parametra
     */
    char * ParamStackTop(TParamStack *s);

    /**
     * @brief Pozrie, či je zásobník parametrov prázdny
     *
     * @param s Zásobník parametrov
     * @return True ak je prázdny, false ak nie je
     */
    bool ParamStackEmpty(TParamStack *s);

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
     * @return Chybový kód
     */
    int gen_add(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_sub(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_mul(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_div(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_idiv(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    // Operácie porovnávania

    int gen_lt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_gt(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);
    int gen_eq(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    // Logické operácie
    int gen_or(char *var_name, char *symbol1, char *symbol2);
    int gen_not(char *var_name, char *symbol);

    // Inštrukcie na prevody
    int gen_int2float(char *var_name, char *symbol, bool is_var);
    void convert_int_2_float(tLocalTableNodePtr *actual_function_ptr, char *symbol, char **converted_name);

    // Práca s reťazcami
    int gen_concat(char *var_name, char *symbol1, tDataType symbol1_type, bool s1_is_var, char *symbol2, tDataType symbol2_type, bool s2_is_var);

    int gen_defvar(char *var_name);
    int gen_move_var(char *var_name, char *symbol);

    // Ďalšie pomocné funkcie
    char *get_string_with_prefix(char *str, char *prefix);
    int add_instruction_ternal(tInstruction_type type, char *var_name, char *symbol1, char *symbol2, char *symbol1_prefix, char *symbol2_prefix);
    int add_instruction_binary(tInstruction_type type, char *var_name, char *symbol, char *symbol_prefix);
    int add_instruction_unary(tInstruction_type type, char *symbol, char *symbol_prefix);
    char *determine_prefix(tDataType type, bool is_var);
    char *get_and_set_unique_label(tLocalTableNodePtr *label_table, char *prefix);

    void set_and_post_instr(tListOfInstr *L, tInstr *I, tInstruction_type type, char *addr1, char *addr2, char *addr3);

    // Generovanie funkcií
    int gen_push_var(char *symbol, tDataType type, bool is_var);
    int gen_pop_var(char *var_name);
    int gen_call(char *function_name);
    void prepare_for_func();
    void end_function();
    int gen_function_header(char *function_name);



    // Vstavané funkcie


    /**
     * @brief Na základe identifikátora funkcie určí, ktorú vstavanú funkciu má vygenerovať
     * @param function_id Identifikátor funkcie
     */
    void generate_built_in_function(char *function_id);


    /**
     * @brief Generovanie vstavanej funkcie inputs
     *
     * @return Chybový kód
     */
    void gen_inputs();

    /**
     * @brief Generovanie vstavanej funkcie inputi
     *
     * @return Chybový kód
     */
    void gen_inputi();

    /**
     * @brief Generovanie vstavanej funkcie inputf
     *
     * @return Chybový kód
     */
    void gen_inputf();

    /**
     * @brief Generovanie vstavanej funkcie print
     *
     * @return Chybový kód
     */
    void gen_print();

    /**
     * @brief Generovanie vstavanej funkcie length
     *
     * @return Chybový kód
     */
    void gen_length();

    /**
     * @brief Generovanie vstavanej funkcie substr
     *
     * @return Chybový kód
     */
    void gen_substr();

    /**
     * @brief Generovanie vstavanej funkcie ord
     *
     * @return Chybový kód
     */
    void gen_ord();

    /**
     * @brief Generovanie vstavanej funkcie chr
     *
     * @return Chybový kód
     */
    void gen_chr();

#endif