#ifndef _SYMTABLE_H
    #define _SYMTABLE_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdbool.h>

    #include "stringlib.h"
    #include "error.h"

    // Datový typ premennej
    typedef enum {
        T_INT,
        T_FLOAT,
        T_STRING,
        T_BOOLEAN,
        T_NIL,
        T_UNDEFINED,
        T_PARAM
    } tDataType;

    typedef struct {
        tDataType type;
        bool defined;
    } tDataNodeLocal;

    // Lokálna tabuľka symbolov
    typedef struct tLocalTableNode {
        char *id; // Identifikátor
        tDataNodeLocal *data;

        struct tLocalTableNode * lPtr;
        struct tLocalTableNode * rPtr;
    } * tLocalTableNodePtr;

    typedef struct {
        bool defined; // Bola definovaná funkcia
        int params; // Počet parametrov funkcie
        tLocalTableNodePtr  *function_table; // Odkaz na lokálnu tabuľku symbolov funkcie
    } tDataNodeGlobal;

    // Globálna tabuľka symbolov
    typedef struct tGlobalTableNode {
        char *id; // Identifikátor
        tDataNodeGlobal *data;

        struct tGlobalTableNode * lPtr;
        struct tGlobalTableNode * rPtr;
    } * tGlobalTableNodePtr;

    // Funkcie pre prácu s tabuľkami symbolov
    // Globálna
    // Všeobecné funkcie
    void global_table_init(tGlobalTableNodePtr *rootPtr);
    int global_table_insert(tGlobalTableNodePtr *rootPtr, char *id, tDataNodeGlobal *data);
    bool global_table_search(tGlobalTableNodePtr rootPtr, char *id, tDataNodeGlobal **data);
    void global_table_replace_by_rightmost(tGlobalTableNodePtr ptrReplaced, tGlobalTableNodePtr *rootPtr);
    void global_table_delete(tGlobalTableNodePtr *rootPtr, char *id);
    void global_table_dispose(tGlobalTableNodePtr *rootPtr);
    void global_table_print_tmp(tGlobalTableNodePtr TempTree, char* sufix, char fromdir);
    void global_table_print(tGlobalTableNodePtr TempTree);
    // Špeciálne pre globálnu tabuľku
    int function_get_number_params(tGlobalTableNodePtr rootPtr, char *function_id);
    void function_increase_number_params(tGlobalTableNodePtr rootPtr, char *function_id);
    void function_set_number_params(tGlobalTableNodePtr rootPtr, char *function_id, int number);
    int function_set_defined(tGlobalTableNodePtr *rootPtr, char *id);
    tGlobalTableNodePtr get_function_node(tGlobalTableNodePtr rootPtr, char *id);
    void set_function_table(tGlobalTableNodePtr *function_node_ptr, tLocalTableNodePtr *local_table_ptr);

    // Lokálna
    // Všeobecné funkcie
    void local_table_init(tLocalTableNodePtr *rootPtr);
    int local_table_insert(tLocalTableNodePtr *rootPtr, char *id, tDataNodeLocal *data);
    bool local_table_search(tLocalTableNodePtr rootPtr, char *id, tDataNodeLocal **data);
    void local_table_replace_by_rightmost(tLocalTableNodePtr ptrReplaced, tLocalTableNodePtr *rootPtr);
    void local_table_delete(tLocalTableNodePtr *rootPtr, char *id);
    void local_table_dispose(tLocalTableNodePtr *rootPtr);
    void local_table_print_tmp(tLocalTableNodePtr TempTree, char* sufix, char fromdir);
    void local_table_print(tLocalTableNodePtr TempTree);
    // Špeciálne pre lokálnu tabuľku
    int variable_set_defined(tLocalTableNodePtr *rootPtr, char *id);
    void variable_set_type(tLocalTableNodePtr function_table, char *id, tDataType type);
    tLocalTableNodePtr get_variable_node(tLocalTableNodePtr rootPtr, char *id);

#endif