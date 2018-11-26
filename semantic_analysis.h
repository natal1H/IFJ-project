#ifndef _SEMANTIC_ANALYSIS_H

    #define _SEMANTIC_ANALYSIS_H

    #include <stdbool.h>
    #include <string.h>
    #include "symtable.h"
    #include "error.h"

    typedef enum sem_type {
        SEM_TYPE_INT, SEM_TYPE_FLOAT, SEM_TYPE_STRING, SEM_TYPE_FUNCTION, SEM_TYPE_VAR, SEM_TYPE_NIL,
    } tSemType;

    // Funkcie na semantickú analýzu v hlavnom parseri
    bool check_if_function_already_defined(tGlobalTableNodePtr global_table, char *function_id);
    int function_definition(tGlobalTableNodePtr *global_table, char *function_id);

    // Funkcie na semantickú analýzuz v parseri výrazov
    bool is_int(char *str);
    bool is_float(char *str);
    bool is_nil(char *str);
    bool is_variable(tLocalTableNodePtr current_function_root, char *str);
    //bool is_function(char *str);
    int check_type_compatibility_aritmetic(tLocalTableNodePtr *current_function_root, char *id1, char *id2);

#endif