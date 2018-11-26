#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

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