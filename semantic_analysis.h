/**
 * IFJ Projekt - Team 11
 *
 * @brief Hlavičkový súbor pre sémantické funkcie
 * @file semantic_analysis.h
 *
 * @author Natália Holková (xholko02)
 */

#ifndef _SEMANTIC_ANALYSIS_H

    #define _SEMANTIC_ANALYSIS_H

    #include <stdbool.h>
    #include <string.h>
    #include "symtable.h"
    #include "error.h"

    #include "expression_parser.h" // Kvôli funkcie na prevod int na string

    // Funkcie na semantickú analýzu v hlavnom parseri

    /**
     * @brief Pozrie, či funkcia už bola definovaná v globálnej tabuľke symbolov
     *
     * @param global_table Globálna tabuľka symbolov
     * @param function_id Názov funkcie
     * @return True ak už bola definovaná, false ak nebola definovaná
     */
    bool check_if_function_already_defined(tGlobalTableNodePtr global_table, char *function_id);

    /**
     * @brief Vytvorí uzol pre funkciu v globálnej tabuľke symbolov
     *
     * @param global_table Ukazovateľ na globálnu tabuľku symbolov
     * @param function_id Názov funkcie
     * @return Chybový kód (napr. redefinícia)
     */
    int function_definition(tGlobalTableNodePtr *global_table, char *function_id);

    // Funkcie na semantickú analýzu v parseri výrazov

    /**
     * @brief Zistí, či môže byť string INT
     *
     * @param str Skúmaný Reťazec
     * @return True ak môže byť INT, false ak nemôže
     */
    bool is_int(char *str);

    // TODO: upraviť aby bralo aj float v tvare 1.2e10
    /**
     * @brief Zistí, či môže byť string FLOAT
     * @param str Skúmaný reťazec
     * @return True ak môže byť FLOAT, false ak nemôže
     */
    bool is_float(char *str);

    /**
     * @brief Zistí, či môže byť string NIL
     * @param str Skúmaný reťazec
     * @return True ak môže byť NIL, false ak nemôže
     */
    bool is_nil(char *str);

    /**
     * @brief Zistí, či môže byť reťazec premennou
     *
     * @param current_function_root Aktuálna lokálna tabuľka symbolov
     * @param str Skúmaný reťazec
     * @return True ak môže byť premennou, false ak nemôže
     */
    bool is_variable(tLocalTableNodePtr current_function_root, char *str);

    // TODO
    /**
     * @brief Zistí, či je reťazec reťazcový literál (začína sa " a končí " )
     *
     * @param str Skúmaný reťazec
     * @return True ak môže byť reťazcový literál, false ak nemôže
     */
    bool is_string_literal(char *str);

    /**
     * @brief Vytvára unikátne meno pre premennú
     *
     * @param local_table Lokálna tabuľka
     * @return Reťazec s unikátny menom alebo NULL pri chybe pri alokovaní
     */
    char *expr_parser_create_unique_name(tLocalTableNodePtr local_table);

    // TODO: ošetriť rozoznanie string
    /**
     * @brief Uloží typ tokenu podľa reťazca, kotroluje zároveň sémantické chyby
     *
     * @param current_function_root Lokálna tabuľka symbolov
     * @param token_ID Reťazec
     * @param type Ukazovateľ na typ, kde sa uloží výsledný typ
     * @return Chybový kód (ERR_OK/ERR_SEM_UNDEF)
     */
    int get_type_from_token(tLocalTableNodePtr *current_function_root, char *token_ID, tDataType *type);

    /**
     * @brief Skúma kompatibilitu typov pri aritmetický operáciách
     *
     * @param type1 Datový typ prvého operanda
     * @param type2 Datový typ druhého operanda
     * @return Chybový kód
     */
    int arithmetic_check_compatibility(tDataType type1, tDataType type2);

    /**
     * @brief Vráti výsledný typ operácie
     * @param token1
     * @param token2
     * @return
     */
    tDataType aritmetic_get_final_type(tDataType token1, tDataType token2);

#endif